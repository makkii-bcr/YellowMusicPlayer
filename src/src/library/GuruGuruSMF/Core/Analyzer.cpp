
#include "stdafx.h"
#include "Analyzer.h"

namespace GuruGuruSmf { namespace Core { namespace Analyzer {
	
	//
	// NoteOnEventExecutor
	//
	
	// �m�[�g�I���p
	void NoteOnEventExecutor::Execute(SmfEvent* smfEvent, IAnalyzerCore* core)
	{
		if (smfEvent->Data2 != 0){
			core->NoteEvent(true);	// �m�[�g�I��
		}else{
			core->NoteEvent(false);	// �m�[�g�I�t
		}
	}
	
	
	//
	// NoteOffEventExecutor
	//
	
	// �m�[�g�I�t�p
	void NoteOffEventExecutor::Execute(SmfEvent* smfEvent, IAnalyzerCore* core)
	{
		core->NoteEvent(false);	// �m�[�g�I�t
	}
	
	
	//
	// TempoEventExecutor
	//
	
	// �e���|�p
	void TempoEventExecutor::Execute(SmfEvent* smfEvent, IAnalyzerCore* core)
	{
		core->SetTempo(smfEvent->Data1);
	}
	
	
	//
	// ProgramChangeEventExecutor
	//
	
	// �v���O�����`�F���W�p
	void ProgramChangeEventExecutor::Execute(SmfEvent* smfEvent, IAnalyzerCore* core)
	{
		core->ProgramChangeEvent(smfEvent->MidiStatus & 0x0f ,smfEvent->Data1);
	}
	
	
	//
	// ControlChangeEventExecutor
	//
	
	// �R���g���[���`�F���W�p
	void ControlChangeEventExecutor::Execute(SmfEvent* smfEvent, IAnalyzerCore* core)
	{
		switch (smfEvent->Data1){
		case 7:	// �{�����[��
			core->VolumeEvent(smfEvent->Data2);
			break;
		case 0:	// �o���N�Z���N�gMSB
			core->BankSelectEvent(smfEvent->MidiStatus & 0x0f, true, smfEvent->Data2);
			break;
		case 32:	// �o���N�Z���N�gLSB
			core->BankSelectEvent(smfEvent->MidiStatus & 0x0f, false, smfEvent->Data2);
			break;
		case 111:	// ���[�v
			core->MarkLoopPoint();
			break;
		default:
			break;
		}
	}
	
	
	//
	// ExclusiveEventExecutor
	//
	//
	
	// �G�N�X�N���[�V�u�p
	void ExclusiveEventExecutor::Execute(SmfEvent* smfEvent, IAnalyzerCore* core)
	{
		int len = smfEvent->ExclusiveLength;
		if (len < 6) return;	// ����������Z���̂͂܂Ƃ߂ăX���[
		
		byte* exData = smfEvent->ExclusiveData;
		if (exData[len-1] != 0xf7){
			return;		// ���₵���o�C�i���͖���
		}
		
		if (len == 10){
			if (exData[0] == 0x41 && exData[2] == 0x42 && exData[3] == 0x12 &&
					exData[4] == 0x40 && (exData[5]&0xf0) == 0x10 && exData[6] == 0x15){
				// GS�̃��Y�����[�h���b�Z�[�W���E���Ă���
				int ch = Constant::GSChannel[exData[5] & 0x0f];	// �`�����l���̕��т�����
				core->UseRhythmPartEvent(ch, exData[7] != 0);
				return;
			}
		}
	}
	
	
	//
	// Analyzer
	//
	
	// �R���X�g���N�^
	Analyzer::Analyzer()
	{
		times = new AnalyzerTimes();
		
		voiceBank = new uint[16];
		eventExecutors = new SmfEventExecutor*[SmfEventType::TrackEnd];
		
		eventExecutors[SmfEventType::NoteOff] = new NoteOffEventExecutor();
		eventExecutors[SmfEventType::NoteOn] = new NoteOnEventExecutor();
		eventExecutors[SmfEventType::ProgramChange] = new ProgramChangeEventExecutor();
		eventExecutors[SmfEventType::ControlChange] = new ControlChangeEventExecutor();
		eventExecutors[SmfEventType::PitchBend] = new NullEventExecutor();
		
		eventExecutors[SmfEventType::CAf] = new NullEventExecutor();
		eventExecutors[SmfEventType::PAf] = new NullEventExecutor();
		
		eventExecutors[SmfEventType::MetaEvent] = new NullEventExecutor();
		eventExecutors[SmfEventType::Tempo] = new TempoEventExecutor();
		eventExecutors[SmfEventType::Exclusive] = new ExclusiveEventExecutor();
	}
	
	// �f�X�g���N�^
	Analyzer::~Analyzer()
	{
		for(int i=0; i<SmfEventType::TrackEnd; i++){
			delete eventExecutors[i];
		}
		delete[] eventExecutors;
		
		delete times;
		delete[] voiceBank;
	}
	
	// Tick��i�߂ĉ��t���Ԃ��v�Z
	void Analyzer::UpdateTick(int tick)
	{
		times->Update(tick);
		++eventCount;
	}

	// �C�x���g����
	void Analyzer::EventExecute(SmfEvent* smfEvent)
	{
		eventExecutors[smfEvent->Type]->Execute(smfEvent, this);
		++eventCount;
	}

	// ����������
	void Analyzer::Initialize(Smf* newSmf)
	{
		smfData = newSmf->GetData();
		times->SetTimeBase(newSmf->GetTimeBase());
		
		SmfInterpreter::Initialize(newSmf);
		
	}

	// �`���֊����߂�
	void Analyzer::Rewind()
	{
		eventCount = 0;
		
		SmfInterpreter::Rewind();
		times->Rewind();
		
		smfData->LastNoteTick = -1;
		smfData->LastNoteTime = 0;
		smfData->FirstNoteTick = -1;
		smfData->FirstNoteTime = 0;
		smfData->LoopTick = -1;
		smfData->LoopTime = 0;
		
		
		for (int i=0; i<16; i++){
			voiceBank[i] = 0;
		}
		voiceBank[9] = 0x80000000;
		
		voiceList.clear();
		voiceList.insert(0);	// @0�����͋����I�ɒǉ�
		voiceList.insert(0x80000000);	// ���Y��@0
	
		smfData->MaxVolume = 100;	// �����ƌ����ɂ��������܂��������̓A�o�E�g��
	}

	// �m�[�g�I�����m�[�g�I�t�̃C�x���g
	void Analyzer::NoteEvent(bool isNoteOn)
	{
		if (isNoteOn && smfData->FirstNoteTick == -1){
			smfData->FirstNoteTick = times->GetMarkerTick();
			smfData->FirstNoteTime = times->GetMarkerTime();
		}
		smfData->LastNoteTick = times->GetMarkerTick();
		smfData->LastNoteTime = times->GetMarkerTime();
	}
	
	// �v���O�����`�F���W�C�x���g
	void Analyzer::ProgramChangeEvent(int channel, int voice)
	{
		uint newVoice = (voiceBank[channel] & 0xffffff00) | (uint)voice;
		voiceBank[channel] = newVoice;
		voiceList.insert(newVoice);
	}

	// �o���N�Z���N�g�C�x���g
	void Analyzer::BankSelectEvent(int channel, bool isMsb, int bank)
	{
		uint newBank = voiceBank[channel];
		if (isMsb){
			newBank = (newBank & 0xff00ffff) | (uint)bank << 16;
		}else{
			newBank = (newBank & 0xffff00ff) | (uint)bank << 8;
		}
		voiceBank[channel] = newBank;
	}

	// ���Y���p�[�g�C�x���g
	void Analyzer::UseRhythmPartEvent(int channel, bool isRhythmPart)
	{
		uint newVoice = voiceBank[channel];
		if (isRhythmPart){
			newVoice |= (uint)0x80000000;
		}else{
			newVoice &= (uint)0x7fffffff;
		}
		voiceBank[channel] = newVoice;
		voiceList.insert(newVoice);
	}

	// �{�����[���C�x���g
	void Analyzer::VolumeEvent(int volume)
	{
		if (smfData->MaxVolume < volume)
			smfData->MaxVolume = volume;
	}

	// ���[�v�|�C���g�𔭌�
	void Analyzer::MarkLoopPoint()
	{
		if (smfData->LoopTick == -1){
			smfData->LoopTick = times->GetMarkerTick();
			smfData->LoopTime = times->GetMarkerTime();
		}
	}

	// ��́I
	GGSERROR Analyzer::Analyze(Smf* newSmf)
	{
		if (!newSmf->GetIsReady()) return GgsError::NotReady;
		
		Initialize(newSmf);
		
		GGSERROR err = InterpretSmf();
		if (err != GgsError::NoError) return err;
		
		smfData->EventCount = eventCount+1;
		smfData->TotalTick = times->GetMarkerTick();
		smfData->TotalTime = (int)times->GetMarkerTime();
		
		smfData->VoiceList->CopyFrom(&voiceList);
		

		// ���[�v�̎�ނ𒲂ׂ�
		if (smfData->LoopTick >= smfData->LastNoteTick){
			smfData->LoopType = LoopType::End;
		}else if (smfData->LoopTick == -1){
			smfData->LoopType = LoopType::Beginning;
		}else if (smfData->LoopTick == 0){
			smfData->LoopType = LoopType::LoopPointAtBeginning;
		}else{
			smfData->LoopType = LoopType::LoopPoint;
		}
		
		return GgsError::NoError;
	}




}}}


