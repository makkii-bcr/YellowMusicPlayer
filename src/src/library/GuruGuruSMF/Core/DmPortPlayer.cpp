
#include "stdafx.h"

#include <dxmus.core/Controller.h>
#include "DmPortPlayer.h"



namespace GuruGuruSmf { namespace Core { namespace DirectMusicPort {

	// �X���b�h�Ăяo���p
	static DWORD WINAPI PlayerWorker(LPVOID object)
	{
		((Player*)object)->OnWorker();
		return 0;
	}

	// �R���X�g���N�^
	Player::Player()
	{
		times = new PlayerTimes();
		device = new Device(times);
		volume = new Volume(device);

		workerThread = new Thread();

		sentReset = false;
		isWorkerOn = false;
		doMasterPitch = false;
		loopCount = 0;
		state = PlayerState::Stop;
	}

	// �f�X�g���N�^
	Player::~Player()
	{
		TRACE(L"��DmPort.Player:�f�X�g���N�^:CloseDevice\n");
		CloseDevice();

		TRACE(L": volume\n");
		delete volume;
		TRACE(L": device\n");
		delete device;
		TRACE(L": times\n");
		delete times;

		TRACE(L": thread\n");
		delete workerThread;
	}

	// �f�o�C�X���J��
	GGSERROR Player::OpenDevice(int deviceId, HWND handle)
	{
		CloseDevice();

		GGS_LOGGING_INITIALIZE();

		return device->Open(deviceId, handle);
	}

	// �f�o�C�X�����
	void Player::CloseDevice()
	{
		if (!device->GetIsOpen()) return;
		
		TRACE(L"DmPort.Player: CloseDevice : Stop\n");
		Stop(0);
		TRACE(L"DmPort.Player: CloseDevice : Sleep\n");
		Sleep(150);
		TRACE(L"DmPort.Player: CloseDevice : device->Close()\n");
		device->Close();
	}

	// ���t�I
	GGSERROR Player::Play(ISmfController* smfCtrl, int option, int seek, int fadeInTime, int newKeyShift)
	{
		if (!device->GetIsOpen()) return GgsError::NotReady;
		Stop(0);

		Smf* newSmf = smfCtrl->GetSmf();

		if (!newSmf->GetIsReady()) return GgsError::NotReady;
		if (newSmf->GetData()->FirstNoteTick == -1) return GgsError::NoError;	// �����f�[�^�̓X���[

		GGSERROR err = smfCtrl->Open();
		if(err != GgsError::NoError) return err;
		smfController = smfCtrl;

		Initialize(newSmf);

		VoiceList* voiceList = newSmf->GetData()->VoiceList;
		GuruGuruSmf::Dxmus::Controller::GetInstance()->PortSetVoiceList((DWORD*)voiceList->GetItems(), voiceList->GetCount());
		ParsePlayOption(option);
		SetKeyShift(newKeyShift);
		
		
		// ���O�ɉ������M���Ă�����1�~���b�̃E�F�C�g��������
		times->ReadMasterClock();
		times->SetMarkerDmTimeByQueueDmTime(10000);
	
		// Stop�ȂǂŃ��Z�b�g�𑗂��ĂȂ������ꍇ�AGS���Z�b�g�𑗂��Ă���
		if (!sentReset){
			device->SendLongMessage(Exclusive::GSReset, Exclusive::GSResetLength);
			times->Wait(10000);		// �s�v�Ȃ悤�ł����ꉞ1�~���b�E�F�C�g
		}
		sentReset = false;
		
		// RPN0�Ԃɂ̓��Z�b�g�������Ȃ��悤�Ȃ̂ő��M���Ă���
		InitializePitchBendSensitivity(device);
		
		volume->SetFadeIn(fadeInTime);
		volume->InitializeModule(GetDoNormalize(), newSmf->GetData()->MaxVolume);
		times->Wait(10000);		// ���������������Ƃ�����悤�Ȃ̂ŃE�F�C�g
		
		if (seek > 0){
			Seek(seek);
		}else{
			SkipBeginning();
		}

		times->UpdateQueueDmTime();
	
		if (TimeEventStep()) return GgsError::NoError;
		state = PlayerState::Play;
		StartWorker();
		
		return GgsError::NoError;
	}

	// ��~
	void Player::Stop(int fadeOutTime)
	{
		if (!device->GetIsOpen() || state == PlayerState::Stop) return;
		
		// �ꎞ��~��Ԃ̎��͂������ɒ�~
		if (state != PlayerState::Pause && fadeOutTime > 0){	// �t�F�[�h�A�E�g����
			volume->SetFadeOut(fadeOutTime);
			return;
		}
		
		StopWorker();
		FinalizeMusic(false, true);
	}

	// �ꎞ��~
	void Player::Pause()
	{
		if (state == PlayerState::Play){
			isPause = true;
			doRestart = false;
			Sleep(Constant::TimeInterval);	// �ی�
		}
	}

	// �ꎞ��~����
	void Player::Restart()
	{
		if (state == PlayerState::Pause){
			doRestart = true;
			Sleep(Constant::TimeInterval);	// �ی�
		}
	}

	// ���t�����擾
	void Player::GetStatus(PlayerStatus* status)
	{
		status->State = state;
		status->LoopCount = loopCount;
		status->Tick = times->GetNowTick();
		status->Time = (int)times->GetNowTime();
		status->Tempo = times->GetMusicTempo();
	}

	// �}�X�^�[�e���|
	void Player::SetMasterTempo(float masterTempo)
	{
		times->SetMasterTempo(masterTempo);
	}

	// �}�X�^�[�{�����[��
	void Player::SetMasterVolume(int masterVolume)
	{
		volume->SetMasterVolume(masterVolume);
	}

	// �}�X�^�[�s�b�`
	void Player::SetMasterPitch(int pitch)
	{
		SetMasterPitchBase(pitch);
		doMasterPitch = true;
	}
		
	// �}�[�J�[Tick�ɉ��Z����
	void Player::AddMarkerTick(int deltaTime)
	{
		times->UpdateMarkerTick(deltaTime);
		
		// ���[�v�ʒu��ݒ�
		if (loopPosition == -1 && times->PassedLoopTick()){
			loopPosition = GetCode()->GetPosition();
		}
	}
		
	// �I�_����
	void Player::MusicEnd()
	{
		isMusicEnd = true;
	}

	// �e���|���Z�b�g
	void Player::SetTempo(int tempo)
	{
		times->SetTempo(tempo);
	}

	// �{�����[�����b�Z�[�W�𑗐M
	void Player::SendVolume(int channel, int vol)
	{
		volume->SetVolume(channel, vol);
	}
		
	// �s�b�`�x���h���b�Z�[�W�𑗐M
	void Player::SendPitchBend(int channel, int pitch)
	{
		SendPitchBendBase(device, channel, pitch);
	}

	// �G�N�X�N���[�V�u�𑗐M
	void Player::SendExclusive(CodeEvent* codeEvent)
	{
		SendExclusiveBase(device, codeEvent);
	}
		
	// Seek�p�ɃC�x���g��o�^
	void Player::RegisterEvent(CODE_EVENTTYPE type, int channel, int data1, CodeEvent* codeEvent)
	{
		int key = (byte)type << 16 | channel << 8 | data1;
		seekingCodeEventTable[key] = *codeEvent;
	}

	// �V�t�g���ꂽ�m�[�g�ԍ����擾
	int Player::GetShiftedNote(int note, int channel)
	{
		return GetShiftedNoteBase(note, channel);
	}
		
	// ���ݎ���+PrepareTime�܂ł̃C�x���g������
	bool Player::TimeEventStep()
	{
		while(!times->PassedPrepareTime()){
			CodeEvent ev;
			GetCode()->ReadEvent(&ev);
			PlayingEventExecute(&ev, this);
			
			// �����̋󔒔�΂�
			if(GetDoSkipEnding()){
				if(times->PassedLastNote()){
					times->RewindQueueTime();	// �����ςݎ��Ԃ܂Ŗ߂��Ă���
					isMusicEnd = true;
				}
			}
			
			// ���[�v���I����
			if(isMusicEnd){
				++loopCount;
				if(GetDoLoop()){
					switch(GetSmf()->GetData()->LoopType){
					case LoopType::Beginning:
						Rewind();
						SkipBeginning();
						break;
					case LoopType::LoopPointAtBeginning:
						Rewind();
						break;
					case LoopType::LoopPoint:
						GetCode()->SetPosition(loopPosition);
						times->GotoLoopPoint();
						isMusicEnd = false;
						break;
					case LoopType::End:
						return true;
					}
				}else{
					return true;	// ���R�I��
				}
			}
		}
		
		// ���݂�Tick�Ɖ��t���Ԃ��v�Z
		times->CalcNowTime();
		return false;
	}
		
	// �`���̋󔒂��X�L�b�v
	void Player::SkipBeginning()
	{
		if(GetDoSkipBeginning()){
			times->SetSkipping(true);
			
			while(!times->PassedFirstNote()){
				CodeEvent ev;
				GetCode()->ReadEvent(&ev);
				PlayingEventExecute(&ev, this);
			}
			times->SetSkipping(false);
		}
	}
		
	// �V�[�N����
	void Player::Seek(int tick)
	{
		seekingCodeEventTable.clear();
		times->SetSkipping(true);
		while(!times->PassedTick(tick)){
			CodeEvent ev;
			GetCode()->ReadEvent(&ev);
			SeekingEventExecute(&ev, this);
			if (isMusicEnd) break;
		}
		
		// �ۑ����Ă������f�[�^�𑗐M
		std::map<int, CodeEvent>::iterator it = seekingCodeEventTable.begin();
		while(it != seekingCodeEventTable.end()){
			PlayingEventExecute(&(*it).second, this);
			++it;
		}
		seekingCodeEventTable.clear();

		times->SetSkipping(false);
	}
	
	// �X���b�h�J�n
	void Player::StartWorker()
	{
		workerThread->Join();	// �X���b�h�����S�ɏI�����ĂȂ��P�[�X������̂ő҂�

		isWorkerOn = true;
		workerThread->Start(this, PlayerWorker, THREAD_PRIORITY_TIME_CRITICAL);
	}

	// �X���b�h��~
	void Player::StopWorker()
	{
		if (isWorkerOn){
			isWorkerOn = false;
			workerThread->Join();
		}
	}
		
	// �ꎞ��~
	void Player::WorkerPause()
	{
		if (state == PlayerState::Play){
			// ���ʂ�0�ɂ���
			times->EnterRealtimeMode();
			volume->Mute(true);
			times->LeaveRealtimeMode();
			times->Pause();
			
			state = PlayerState::Pause;
		}
	}
		
	// �ꎞ��~����
	void Player::WorkerRestart()
	{
		times->Restart();
		
		times->EnterRealtimeMode();
		volume->Mute(false);
		times->LeaveRealtimeMode();
		
		state = PlayerState::Play;
		isPause = false;
		doRestart = false;
	}
		
	// �X���b�h���s��
	void Player::OnWorker()
	{
		while(isWorkerOn){
			times->ReadMasterClock();
			
			if (doRestart) WorkerRestart();
			
			if (isPause){
				WorkerPause();
			}else{
				times->EnterRealtimeMode();
				bool terminated = volume->Worker();
				times->LeaveRealtimeMode();
				if (terminated){
					// �I�[���m�[�g�I�t
					for (int i=0; i<16; i++){
						device->SendShortMessage(0xb0+i, 123, 0);
					}
					times->UpdateQueueDmTime();
					FinalizeMusic(true, true);
					return;
				}
				
				if (GetCanDoMasterPitch() && doMasterPitch){
					SendMasterPitch();
					doMasterPitch = false;
				}
				
				if (TimeEventStep()){
					FinalizeMusic(true, false);
					return;
				}
			}
			
			Sleep(Constant::TimeInterval);
			times->FixQueueDmTime();
		}
	}
	
	// ���y��~���̍ŏI����
	void Player::FinalizeMusic(bool isWorker, bool doReset)
	{
		if (isWorker){
			isWorkerOn = false;
		}
		state = PlayerState::Stop;
		smfController->Close();
	
		if (doReset){
			// GS���Z�b�g�𑗂�
			times->ReadMasterClock();
			times->SetMarkerDmTimeByQueueDmTime(10000);
			device->SendLongMessage(Exclusive::GSReset, Exclusive::GSResetLength);
			sentReset = true;
			times->UpdateQueueDmTime();
		}
	}
	
	// �}�X�^�[�s�b�`�𑗐M
	void Player::SendMasterPitch()
	{
		times->EnterRealtimeMode();
		SendMasterPitchBase(device);
		times->LeaveRealtimeMode();
	}
		
	// SMF�ŏ�����
	void Player::Initialize(Smf* newSmf)
	{
		PlayerBase::Initialize(newSmf);
		
		times->Initialize(newSmf);
		loopPosition = -1;
	
		loopCount = 0;
		isPause = false;
		doRestart = false;
	
		Rewind();
	}

	// �`���Ɋ����߂�
	void Player::Rewind()
	{
		PlayerBase::Rewind();
		isMusicEnd = false;

		times->Rewind();
		GetCode()->SetPosition(0);
	}



}}}

