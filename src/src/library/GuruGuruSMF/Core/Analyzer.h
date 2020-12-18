// SMF���

// #pragma once
#ifndef _ANALYZER_H_
#define _ANALYZER_H_

#include <set>

#include "Constants.h"
#include "Interface.h"
#include "Times.h"
#include "SmfInterpreter.h"


namespace GuruGuruSmf { namespace Core { namespace Analyzer {
	
	/// <summary>
	/// SMF�C�x���g���s�p�̊��N���X(Analyzer)
	/// </summary>
	class SmfEventExecutor
	{
	public:
		// ���s�I
		virtual void Execute(SmfEvent* smfEvent, IAnalyzerCore* core) = 0;
	};
	
	/// <summary>
	/// �������s���Ȃ��p
	/// </summary>
	class NullEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, IAnalyzerCore* core){}
	};
	
	/// <summary>
	/// �m�[�g�I���p
	/// </summary>
	class NoteOnEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, IAnalyzerCore* core);
	};
	
	/// <summary>
	/// �m�[�g�I�t�p
	/// </summary>
	class NoteOffEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, IAnalyzerCore* core);
	};
	
	/// <summary>
	/// �e���|�p
	/// </summary>
	class TempoEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, IAnalyzerCore* core);
	};
	
	/// <summary>
	/// �v���O�����`�F���W�p
	/// </summary>
	class ProgramChangeEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, IAnalyzerCore* core);
	};
	
	/// <summary>
	/// �R���g���[���`�F���W�p
	/// </summary>
	class ControlChangeEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, IAnalyzerCore* core);
	};
	
	
	/// <summary>
	/// �G�N�X�N���[�V�u�p
	/// </summary>
	class ExclusiveEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, IAnalyzerCore* core);
	};
	
	
	/// <summary>
	/// SMF���
	/// </summary>
	class Analyzer : public SmfInterpreter, public IAnalyzerCore
	{
	private:
		SmfEventExecutor** eventExecutors;
		AnalyzerTimes* times;
		int eventCount;
		SmfAnalyzedData* smfData;
		
		uint* voiceBank;
		std::set<uint> voiceList;

	protected:
		// Tick��i�߂ĉ��t���Ԃ��v�Z
		void UpdateTick(int tick);

		// �C�x���g����
		void EventExecute(SmfEvent* smfEvent);

		// ����������
		void Initialize(Smf* newSmf);

		// �`���֊����߂�
		void Rewind();

	public:
		Analyzer();		// �R���X�g���N�^
		~Analyzer();	// �f�X�g���N�^

		//
		// IAnalyzerCore������
		//
		
		// �e���|��ݒ�
		void SetTempo(int tempo){ times->SetTempo(tempo); }

		// �m�[�g�I�����m�[�g�I�t�̃C�x���g
		void NoteEvent(bool isNoteOn);

		// �v���O�����`�F���W�C�x���g
		void ProgramChangeEvent(int channel, int voice);

		// �o���N�Z���N�g�C�x���g
		void BankSelectEvent(int channel, bool isMsb, int bank);

		// ���Y���p�[�g�C�x���g
		void UseRhythmPartEvent(int channel, bool isRhythmPart);

		// �{�����[���C�x���g
		void VolumeEvent(int volume);

		// ���[�v�|�C���g�𔭌�
		void MarkLoopPoint();

		//

		// ��́I
		GGSERROR Analyze(Smf* newSmf);


	};
	
	
	
	
	
	
}}}



#endif
