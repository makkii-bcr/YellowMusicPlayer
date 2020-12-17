// SMF�������R�[�h�ϊ�

#pragma once

#include "Constants.h"
#include "Interface.h"
#include "SmfInterpreter.h"


namespace GuruGuruSmf { namespace Core { namespace Compiler {

	/// <summary>
	/// SMF�C�x���g���s�p�̊��N���X(Compiler)
	/// </summary>
	class SmfEventExecutor
	{
	public:
		// ���s�I
		virtual void Execute(SmfEvent* smfEvent, ICompilerCore* core) = 0;
	};

	/// <summary>
	/// �������s���Ȃ��p
	/// </summary>
	class NullEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core){}
	};
	
	/// <summary>
	/// �e���|�p
	/// </summary>
	class TempoEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
	};
	
	/// <summary>
	/// �m�[�g�I���p
	/// </summary>
	class NoteOnEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
	};
	
	/// <summary>
	/// �m�[�g�I�t�p
	/// </summary>
	class NoteOffEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
	};
	
	/// <summary>
	/// �v���O�����`�F���W�p
	/// </summary>
	class ProgramChangeEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
	};
	
	/// <summary>
	/// �R���g���[���`�F���W�p
	/// </summary>
	class ControlChangeEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
	};

	/// <summary>
	/// �s�b�`�x���h�p
	/// </summary>
	class PitchBendEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
	};

	/// <summary>
	/// �`�����l���v���b�V���[�p
	/// </summary>
	class CAfEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
	};
	
	/// <summary>
	/// �|���t�H�j�b�N�L�[�v���b�V���[�p
	/// </summary>
	class PAfEventExecutor : public SmfEventExecutor
	{
	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
	};
	
	/// <summary>
	/// �G�N�X�N���[�V�u�p
	/// </summary>
	class ExclusiveEventExecutor : public SmfEventExecutor
	{
	private:
		// �f�[�^���r����
		bool CompareExclusive(const byte* data, const byte* compare, int position, int length);

	public:
		void Execute(SmfEvent* smfEvent, ICompilerCore* core);
		
	};

	/// <summary>
	/// SMF��ϊ�
	/// </summary>
	class Compiler : public SmfInterpreter, public ICompilerCore
	{
	private:
		GgsCode* code;
		SmfEventExecutor** eventExecutors;
		int markerTick;

	protected:
		// Tick��i�߂�
		void UpdateTick(int newTick);
		
		// �C�x���g����
		void EventExecute(SmfEvent* smfEvent);
		
		// ����������
		void Initialize(Smf* newSmf);
	
	public:
		Compiler();		// �R���X�g���N�^
		~Compiler();	// �f�X�g���N�^

		//
		// ICompilerCore������
		//
		
		// GGSCode�I�u�W�F�N�g
		GgsCode* GetCode(){ return code; }
		
		// 2�o�C�g�̃C�x���g����������
		void Write2ByteEvent(CODE_EVENTTYPE type, SmfEvent* smfEvent);

		// �G�N�X�N���[�V�u�C�x���g����������
		void WriteExclusiveEvent(SmfEvent* smfEvent);
		
		// ���Y���p�[�g�C�x���g����������
		void WriteUseRhythmPartEvent(SmfEvent* smfEvent, int channel, bool rhythm);

		//

		// �ϊ��I
		GGSERROR Compile(Smf* newSmf);
		
		
	};





}}}



