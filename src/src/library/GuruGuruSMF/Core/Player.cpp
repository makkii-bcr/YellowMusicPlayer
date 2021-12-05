
#include "stdafx.h"

#include "Player.h"


namespace GuruGuruSmf { namespace Core { namespace Player {

	//
	// MusicEndEventExecutor
	//

	// �I�_�p
	void MusicEndEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->MusicEnd();
	}

	
	//
	// DeltaTimeEventExecutor
	//

	// �f���^�^�C���p
	void DeltaTimeEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->AddMarkerTick(codeEvent->IntData);
	}
	
	
	//
	// TempoEventExecutor
	//

	// �e���|�p
	void TempoEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->SetTempo(codeEvent->IntData);
	}
	
	
	//
	// PlayingNoteOnEventExecutor
	//

	// Play: �m�[�g�I���p
	void PlayingNoteOnEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		int note = core->GetShiftedNote(codeEvent->ByteData1, codeEvent->Channel);
		core->GetDevice()->SendShortMessage(0x90+codeEvent->Channel,
		                             note, codeEvent->ByteData2);
	}
	
	
	//
	// PlayingNoteOffEventExecutor
	//

	// Play: �m�[�g�I�t�p
	void PlayingNoteOffEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		int note = core->GetShiftedNote(codeEvent->ByteData1, codeEvent->Channel);
		core->GetDevice()->SendShortMessage(0x80+codeEvent->Channel,
		                             note, codeEvent->ByteData2);
	}
	

	//
	// PlayingProgramChangeEventExecutor
	//

	// Play: �v���O�����`�F���W�p
	void PlayingProgramChangeEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->GetDevice()->SendShortMessage(0xc0+codeEvent->Channel,
		                             codeEvent->ByteData1, 0);
	}
	
	
	//
	// PlayingControlChangeEventExecutor
	//

	// Play: �R���g���[���`�F���W�p
	void PlayingControlChangeEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->GetDevice()->SendShortMessage(0xb0+codeEvent->Channel,
		                             codeEvent->ByteData1, codeEvent->ByteData2);
	}

	
	//
	// PlayingVolumeEventExecutor
	//

	/// Play: �{�����[���p
	void PlayingVolumeEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->SendVolume(codeEvent->Channel, codeEvent->ByteData2);
	}
	
	
	//
	// PlayingPitchBendEventExecutor
	//

	// Play: �s�b�`�x���h�p
	void PlayingPitchBendEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->SendPitchBend(codeEvent->Channel, codeEvent->IntData);
	}
	
	
	//
	// PlayingCAfEventExecutor
	//

	// Play: �`�����l���v���b�V���[�p
	void PlayingCAfEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->GetDevice()->SendShortMessage(0xd0+codeEvent->Channel,
		                             codeEvent->ByteData1, 0);
	}
	
	
	//
	// PlayingPAfEventExecutor
	//

	// Play: �|���t�H�j�b�N�L�[�v���b�V���[�p
	void PlayingPAfEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		int note = core->GetShiftedNote(codeEvent->ByteData1, codeEvent->Channel);
		core->GetDevice()->SendShortMessage(0xa0+codeEvent->Channel,
		                             note, codeEvent->ByteData2);
	}
	
	
	//
	// PlayingExclusiveEventExecutor
	//

	// Play: �G�N�X�N���[�V�u�p
	void PlayingExclusiveEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->SendExclusive(codeEvent);
	}
	
	
	
	//
	// SeekingProgramChangeEventExecutor
	//

	// Seek: �v���O�����`�F���W�p
	void SeekingProgramChangeEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->RegisterEvent((CODE_EVENTTYPE)codeEvent->Type, codeEvent->Channel, 0, codeEvent);
	}
	
	
	//
	// SeekingControlChangeEventExecutor
	//

	// Seek: �R���g���[���`�F���W�p
	void SeekingControlChangeEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->RegisterEvent((CODE_EVENTTYPE)codeEvent->Type, codeEvent->Channel,
		                   codeEvent->ByteData1, codeEvent);
	}
	
	
	//
	// SeekingVolumeEventExecutor
	//

	// Seek: �{�����[���p
	void SeekingVolumeEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->RegisterEvent((CODE_EVENTTYPE)codeEvent->Type, codeEvent->Channel, 0, codeEvent);
	}
	
	
	//
	// SeekingPitchBendEventExecutor
	//

	// Seek: �s�b�`�x���h�p
	void SeekingPitchBendEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->RegisterEvent((CODE_EVENTTYPE)codeEvent->Type, codeEvent->Channel, 0, codeEvent);
	}
	
	
	//
	// SeekingCAfEventExecutor
	//

	// Seek: �`�����l���v���b�V���[�p
	void SeekingCAfEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->RegisterEvent((CODE_EVENTTYPE)codeEvent->Type, codeEvent->Channel, 0, codeEvent);
	}
	
	
	//
	// SeekingPAfEventExecutor
	//

	// Seek: �|���t�H�j�b�N�L�[�v���b�V���[�p
	void SeekingPAfEventExecutor::Execute(CodeEvent* codeEvent, IPlayerCore* core)
	{
		core->RegisterEvent((CODE_EVENTTYPE)codeEvent->Type, codeEvent->Channel,
		                   codeEvent->ByteData1, codeEvent);
	}


	//
	// PlayerBase
	//

	// �R���X�g���N�^
	PlayerBase::PlayerBase()
	{
		rhythmChannels = new bool[16];
		pitches = new int[16];
		masterPitch = 0;

		MakePlayCodeEventExcutors();
		MakeSeekCodeEventExcutors();
	}

	// �f�X�g���N�^
	PlayerBase::~PlayerBase()
	{
		TRACE(L"��PlayerBase:�f�X�g���N�^:pitchs\n");
		delete[] pitches;
		TRACE(L":rhythmChannels\n");
		delete[] rhythmChannels;

		TRACE(L":EventExecutors\n");
		for (int i=0; i<CodeEventType::MusicEnd+1; i++){
			delete playingEventExecutors[i];
			delete seekingEventExecutors[i];
		}
		TRACE(L":playingEventExecutors\n");
		delete[] playingEventExecutors;
		TRACE(L":seekingEventExecutors\n");
		delete[] seekingEventExecutors;
	}

	// �V�t�g���ꂽ�m�[�g�ԍ����擾
	int PlayerBase::GetShiftedNoteBase(int note, int channel)
	{
		if (rhythmChannels[channel]) return note;
		
		note += keyShift;
		if (note < 0){
			note = 0;
		}else if (note > 127){
			note = 127;
		}
		return note;
	}

	// �}�X�^�[�s�b�`�𑗐M
	void PlayerBase::SendMasterPitchBase(IDevice* device)
	{
		for (int i=0; i<16; i++){
			SendPitchBendBase(device, i, pitches[i]);
		}
	}

	// �s�b�`�x���h���b�Z�[�W�𑗐M
	void PlayerBase::SendPitchBendBase(IDevice* device, int channel, int pitch)
	{
		int p = pitch + 8192 + masterPitch;
		pitches[channel] = pitch;
		if (p < 0){
			p = 0;
		}else if (p > 16383){
			p = 16383;
		}
		
		device->SendShortMessage(0xe0+channel, p & 0x7f, (p >> 7) & 0x7f);
	}

	// �G�N�X�N���[�V�u�𑗐M
	void PlayerBase::SendExclusiveBase(IDevice* device, CodeEvent* codeEvent)
	{
		TRACE1(L"PlayerBase::SendExclusiveBase(%d)\n", doSendExclusive);
		if (doSendExclusive){
			if (codeEvent->Type == CodeEventType::UseRhythmPart){
				rhythmChannels[codeEvent->Channel] = (codeEvent->Option1 != 0);
			}
			
			ExclusiveData* ex = code->GetExclusive(codeEvent->IntData);
			device->SendLongMessage(ex->GetData(), ex->GetLength());
		}
	}

	// �s�b�`�x���h�Z���V�r���e�B��������
	void PlayerBase::InitializePitchBendSensitivity(IDevice* device)
	{
		int n;
		if (canDoMasterPitch){
			n = 12;
		}else{
			n = 2;
		}
		
		for (int i=0; i<16; i++){
			device->SendShortMessage(0xb0+i, 101, 0);
			device->SendShortMessage(0xb0+i, 100, 0);
			device->SendShortMessage(0xb0+i, 6, n);
		}
	}

	// SMF�ŏ�����
	void PlayerBase::Initialize(Smf* newSmf)
	{
		smf = newSmf;
		code = newSmf->GetCode();
		
		for (int i=0; i<16; i++){
			pitches[i] = 0;
		}
	}

	// �`���֊����߂�
	void PlayerBase::Rewind()
	{
		for (int i=0; i<16; i++){
			rhythmChannels[i] = (i == 9);
		}
	}
	
	// PlayOption������
	void PlayerBase::ParsePlayOption(int option)
	{
		doLoop = ((option & PlayOption::Loop) != 0);
		doSkipBeginning = ((option & PlayOption::SkipBeginning) != 0);
		doSkipEnding = ((option & PlayOption::SkipEnding) != 0);
		canDoMasterPitch = ((option & PlayOption::MasterPitch) != 0);
		doNormalize = ((option & PlayOption::Normalize) != 0);
		doSendExclusive = ((option & PlayOption::Exclusive) != 0);
	}

	// �C�x���g�����s(Play)
	void PlayerBase::PlayingEventExecute(CodeEvent* ev, IPlayerCore* core)
	{
		playingEventExecutors[ev->Type]->Execute(ev, core);
	}

	// �C�x���g�����s(Seek)
	void PlayerBase::SeekingEventExecute(CodeEvent* ev, IPlayerCore* core)
	{
		seekingEventExecutors[ev->Type]->Execute(ev, core);
	}

	// �C�x���g���s�I�u�W�F�N�g�̔z����쐬(Play)
	void PlayerBase::MakePlayCodeEventExcutors()
	{
		playingEventExecutors = new CodeEventExecutor*[CodeEventType::MusicEnd+1];
		playingEventExecutors[CodeEventType::DeltaTime] = new DeltaTimeEventExecutor();
		playingEventExecutors[CodeEventType::NoteOn] = new PlayingNoteOnEventExecutor();
		playingEventExecutors[CodeEventType::NoteOff] = new PlayingNoteOffEventExecutor();
		playingEventExecutors[CodeEventType::ProgramChange] = new PlayingProgramChangeEventExecutor();
		
		playingEventExecutors[CodeEventType::ControlChangeA] = new PlayingControlChangeEventExecutor();
		playingEventExecutors[CodeEventType::ControlChangeB] = new PlayingControlChangeEventExecutor();
		playingEventExecutors[CodeEventType::ControlChangeC] = new NullEventExecutor();
		playingEventExecutors[CodeEventType::Volume] = new PlayingVolumeEventExecutor();
		
		playingEventExecutors[CodeEventType::PAf] = new PlayingPAfEventExecutor();
		playingEventExecutors[CodeEventType::CAf] = new PlayingCAfEventExecutor();
		playingEventExecutors[CodeEventType::PitchBend] = new PlayingPitchBendEventExecutor();
		
		playingEventExecutors[CodeEventType::Tempo] = new TempoEventExecutor();
		playingEventExecutors[CodeEventType::Exclusive] = new PlayingExclusiveEventExecutor();
		playingEventExecutors[CodeEventType::UseRhythmPart] = new PlayingExclusiveEventExecutor();
		
		playingEventExecutors[CodeEventType::MusicEnd] = new MusicEndEventExecutor();
	}
	
	// �C�x���g���s�I�u�W�F�N�g�̔z����쐬(Seek)
	void PlayerBase::MakeSeekCodeEventExcutors()
	{
		seekingEventExecutors = new CodeEventExecutor*[CodeEventType::MusicEnd+1];
		seekingEventExecutors[CodeEventType::DeltaTime] = new DeltaTimeEventExecutor();
		seekingEventExecutors[CodeEventType::NoteOn] = new NullEventExecutor();
		seekingEventExecutors[CodeEventType::NoteOff] = new NullEventExecutor();
		seekingEventExecutors[CodeEventType::ProgramChange] = new SeekingProgramChangeEventExecutor();
		
		seekingEventExecutors[CodeEventType::ControlChangeA] = new SeekingControlChangeEventExecutor();
		seekingEventExecutors[CodeEventType::ControlChangeB] = new PlayingControlChangeEventExecutor();
		seekingEventExecutors[CodeEventType::ControlChangeC] = new NullEventExecutor();
		seekingEventExecutors[CodeEventType::Volume] = new SeekingVolumeEventExecutor();
		
		seekingEventExecutors[CodeEventType::PAf] = new SeekingPAfEventExecutor();
		seekingEventExecutors[CodeEventType::CAf] = new SeekingCAfEventExecutor();
		seekingEventExecutors[CodeEventType::PitchBend] = new SeekingPitchBendEventExecutor();
		
		seekingEventExecutors[CodeEventType::Tempo] = new TempoEventExecutor();
		seekingEventExecutors[CodeEventType::Exclusive] = new PlayingExclusiveEventExecutor();
		seekingEventExecutors[CodeEventType::UseRhythmPart] = new PlayingExclusiveEventExecutor();

		seekingEventExecutors[CodeEventType::MusicEnd] = new MusicEndEventExecutor();
	}

}}}



