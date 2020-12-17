
#include "stdafx.h"

#include "WinmmPlayer.h"

namespace GuruGuruSmf { namespace Core { namespace WindowsMultimedia {

	// �^�C�}�[�R�[���o�b�N
	static void CALLBACK TimeCallback(UINT timerId, UINT msg, DWORD user, DWORD param1, DWORD param2)
	{
		((Player*)user)->OnTimer();
	}

	// �R���X�g���N�^
	Player::Player()
	{
		device = new Device();
		times = new PlayerTimes();
		volume = new Volume(device);

		timerId = 0;
		doMasterPitch = false;

		state = PlayerState::Stop;
		loopCount = 0;
		sentReset = false;
		resetedTime = 0;
	}
	
	// �f�X�g���N�^
	Player::~Player()
	{
		CloseDevice();

		delete volume;
		delete times;
		delete device;
	}

	// �^�C�}�[���s��
	void Player::OnTimer()
	{
		if(doRestart){
			WorkerRestart();
		}
		
		if(state != PlayerState::Play) return;
		
		if(isPause){
			WorkerPause();
			return;
		}
		
		if(volume->Worker()){
			StopTimer();
			// �I�[���m�[�g�I�t�{���Z�b�g
			for (int i=0; i<16; i++){
				device->SendShortMessage(0xb0+i, 123, 0);
			}
			SendReset();
			state = PlayerState::Stop;
			smfController->Close();
			return;
		}
		
		if(GetCanDoMasterPitch() && doMasterPitch){
			SendMasterPitchBase(device);
			doMasterPitch = false;
		}
		
		while(times->PassedMarkerTick()){
			CodeEvent ev;
			GetCode()->ReadEvent(&ev);
			PlayingEventExecute(&ev, this);
			
			// �����̋󔒔�΂�
			if(GetDoSkipEnding()){
				if(times->PassedLastNote()){
					isMusicEnd = true;
				}
			}
			
			// ���[�v���I����
			if(isMusicEnd){
				++loopCount;
				if (GetDoLoop()){
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
						times->GotoLoopPoint(GetDoSkipEnding());
						isMusicEnd = false;
						break;
					case LoopType::End:
						state = PlayerState::Stop;
						StopTimer();
						smfController->Close();
						return;
					}
				}else{
					state = PlayerState::Stop;
					StopTimer();
					smfController->Close();
					return;	// ���R�I��
				}
			}
		}
		
		times->UpdateNowTick();
	}

	// �f�o�C�X���J��
	GGSERROR Player::OpenDevice(int deviceId, HWND handle)
	{
		Stop(0);
		GGS_LOGGING_INITIALIZE();
		return device->Open(deviceId);
	}
	
	// �f�o�C�X�����
	void Player::CloseDevice()
	{
		Stop(0);
		device->Close();
	}
	
	// ���t�I
	GGSERROR Player::Play(ISmfController* smfCtrl, int option, int seek, int fadeInTime, int keyShift)
	{
		if (!device->GetIsOpen()) return GgsError::NotReady;
		Stop(0);
		
		Smf* newSmf = smfCtrl->GetSmf();

		if(!newSmf->GetIsReady()) return GgsError::NotReady;
		if(newSmf->GetData()->FirstNoteTick == -1) return GgsError::NoError;	// �����f�[�^�̓X���[

		GGSERROR err = smfCtrl->Open();
		if(err != GgsError::NoError) return err;
		smfController = smfCtrl;

		Initialize(newSmf);
		ParsePlayOption(option);
		SetKeyShift(keyShift);
		
		SendReset();
		WaitAfterReset();
		sentReset = false;
		
		InitializePitchBendSensitivity(device);
		volume->SetFadeIn(fadeInTime);
		volume->InitializeModule(GetDoNormalize(), newSmf->GetData()->MaxVolume);
		
		// �V�[�N����
		if (seek > 0){
			Seek(seek);
		}else{
			SkipBeginning();
		}
		
		err = StartTimer();
		if (err != GgsError::NoError) return err;
		state = PlayerState::Play;			
		return GgsError::NoError;
	}
	
	// ��~�I
	void Player::Stop(int fadeOutTime)
	{
		if (state == PlayerState::Stop) return;

		// �ꎞ��~��Ԃ̎��͂������ɒ�~
		if (state != PlayerState::Pause && fadeOutTime > 0){	// �t�F�[�h�A�E�g����
			volume->SetFadeOut(fadeOutTime);
			return;
		}
		
		StopTimer();
		Sleep(Constant::TimeInterval);	// �ی�

		// �I�[���m�[�g�I�t�{���Z�b�g
		for (int i=0; i<16; i++){
			device->SendShortMessage(0xb0+i, 123, 0);
		}
		SendReset();
		state = PlayerState::Stop;
		
		smfController->Close();
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
	
	// ���t��Ԃ��擾
	void Player::GetStatus(PlayerStatus* status)
	{
		status->State = state;
		status->LoopCount = loopCount;
		status->Tick = times->GetNowTick();
		status->Time = times->GetNowTime();
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
	void Player::SetTempo(int newTempo)
	{
		times->SetTempo(newTempo);
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
	
	// �^�C�}�[�J�n
	GGSERROR Player::StartTimer()
	{
		StopTimer();
		
		timeBeginPeriod(1);
		timerId = timeSetEvent(Constant::TimeInterval, 1, TimeCallback, (DWORD_PTR)this, 1);
		if (timerId == 0){
			timeEndPeriod(1);
			return GgsError::CannotStartTimer;
		}
		
		return GgsError::NoError;
	}
		
	// �^�C�}�[��~
	void Player::StopTimer()
	{
		if (timerId != 0){
			timeKillEvent(timerId);
			timeEndPeriod(1);
			timerId = 0;
		}
	}
		
	// �ꎞ��~
	void Player::WorkerPause()
	{
		if (state == PlayerState::Play){
			// ���ʂ�0�ɂ���
			volume->Mute(true);
			
			state = PlayerState::Pause;
		}
		isPause = false;
	}
		
	// �ꎞ��~����
	void Player::WorkerRestart()
	{
		volume->Mute(false);
		
		state = PlayerState::Play;
		isPause = false;
		doRestart = false;
	}
		
	// �`���̋󔒂��X�L�b�v
	void Player::SkipBeginning()
	{
		if (GetDoSkipBeginning()){
			while (!times->PassedFirstNote()){
				CodeEvent ev;
				GetCode()->ReadEvent(&ev);
				PlayingEventExecute(&ev, this);
			}
			
			times->GotoFirstNote();
		}
	}
	
	// �V�[�N����
	void Player::Seek(int tick)
	{
		if (tick > GetSmf()->GetData()->LastNoteTick){
			tick = GetSmf()->GetData()->LastNoteTick;
		}
		
		seekingCodeEventTable.clear();
		while (!times->PassedTick(tick)){
			CodeEvent ev;
			GetCode()->ReadEvent(&ev);
			SeekingEventExecute(&ev, this);
			if (isMusicEnd) break;
		}
		
		times->FixNowTime(tick);
		
		// �ۑ����Ă������f�[�^�𑗐M
		std::map<int, CodeEvent>::iterator it = seekingCodeEventTable.begin();
		while(it != seekingCodeEventTable.end()){
			PlayingEventExecute(&(*it).second, this);
			++it;
		}
		seekingCodeEventTable.clear();
	}
	
	// ���Z�b�g���߂𑗂�
	void Player::SendReset()
	{
		if (!sentReset){
			// �I�[���T�E���h�I�t�{���Z�b�g�I�[���R���g���[��
			for (int i=0; i<16; i++){
				device->SendShortMessage(0xb0+i, 120, 0);
				device->SendShortMessage(0xb0+i, 121, 0);
			}
			device->SendLongMessage(Exclusive::GMSystemOn, Exclusive::GMSystemOnLength);
			
			LARGE_INTEGER t;
			if(QueryPerformanceCounter(&t)){
				resetedTime = t.QuadPart;
			}else{
				resetedTime = timeGetTime();
			}
		}
		
		sentReset = true;
	}

	// ���Z�b�g��̃E�F�C�g������
	void Player::WaitAfterReset()
	{
		double interval;

		LARGE_INTEGER t;
		if(QueryPerformanceCounter(&t)){
			interval = (double)(t.QuadPart - resetedTime);

			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			interval = interval * 1000 / freq.QuadPart;
		}else{
			interval = (double)(resetedTime - timeGetTime());
		}

		if (interval < 50){
			int wait = (int)(50 - interval) + 1;
			Sleep(wait);
		}
	}


}}}




