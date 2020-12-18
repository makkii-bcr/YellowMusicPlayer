// �v���C���[�̊��N���X

// #pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Interface.h"
#include "SmfController.h"

namespace GuruGuruSmf { namespace Core { namespace Player {

	/// <summary>
	/// GGSCode�C�x���g���s�p�̊��N���X(Player���g�p)
	/// </summary>
	class CodeEventExecutor
	{
	public:
		// ���s�I
		virtual void Execute(CodeEvent* codeEvent, IPlayerCore* core) = 0;
	};
	
	/// <summary>
	/// �������s���Ȃ��p
	/// </summary>
	class NullEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core){}
	};

	/// <summary>
	/// �I�_�p
	/// </summary>
	class MusicEndEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// �f���^�^�C���p
	/// </summary>
	class DeltaTimeEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// �e���|�p
	/// </summary>
	class TempoEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �m�[�g�I���p
	/// </summary>
	class PlayingNoteOnEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �m�[�g�I�t�p
	/// </summary>
	class PlayingNoteOffEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �v���O�����`�F���W�p
	/// </summary>
	class PlayingProgramChangeEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �R���g���[���`�F���W�p
	/// </summary>
	class PlayingControlChangeEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �{�����[���p
	/// </summary>
	class PlayingVolumeEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �s�b�`�x���h�p
	/// </summary>
	class PlayingPitchBendEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �`�����l���v���b�V���[�p
	/// </summary>
	class PlayingCAfEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �|���t�H�j�b�N�L�[�v���b�V���[�p
	/// </summary>
	class PlayingPAfEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Play: �G�N�X�N���[�V�u�p
	/// </summary>
	class PlayingExclusiveEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	
	
	/// <summary>
	/// Seek: �v���O�����`�F���W�p
	/// </summary>
	class SeekingProgramChangeEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Seek: �R���g���[���`�F���W�p
	/// </summary>
	class SeekingControlChangeEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Seek: �{�����[���p
	/// </summary>
	class SeekingVolumeEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Seek: �s�b�`�x���h�p
	/// </summary>
	class SeekingPitchBendEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Seek: �`�����l���v���b�V���[�p
	/// </summary>
	class SeekingCAfEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};
	
	/// <summary>
	/// Seek: �|���t�H�j�b�N�L�[�v���b�V���[�p
	/// </summary>
	class SeekingPAfEventExecutor : public CodeEventExecutor
	{
	public:
		void Execute(CodeEvent* codeEvent, IPlayerCore* core);
	};


	/// <summary>
	/// �v���C���[�̊��N���X
	/// </summary>
	class PlayerBase
	{
	private:
		CodeEventExecutor** playingEventExecutors;
		CodeEventExecutor** seekingEventExecutors;
		
		Smf* smf;
		GgsCode* code;
		
		int keyShift;
		int* pitches;
		bool* rhythmChannels;
		
		int masterPitch;
		
		bool doLoop, doSkipBeginning, doSkipEnding;
		bool doNormalize, canDoMasterPitch, doSendExclusive;

	protected:
		// SMF�I�u�W�F�N�g
		Smf* GetSmf(){ return smf; }
		
		// Code�I�u�W�F�N�g
		GgsCode* GetCode(){ return code; }
		
		// ���[�v���邩�ǂ���
		bool GetDoLoop(){ return doLoop; }
		
		// �`���̋󔒂��X�L�b�v�����邩�ǂ���
		bool GetDoSkipBeginning(){ return doSkipBeginning; }
		
		// �����̋󔒂��X�L�b�v���邩�ǂ���
		bool GetDoSkipEnding(){ return doSkipEnding; }
		
		// �}�X�^�[�s�b�`��L���ɂ��邩�ǂ���
		bool GetCanDoMasterPitch(){ return canDoMasterPitch; }
		
		// �m�[�}���C�Y���邩�ǂ���
		bool GetDoNormalize(){ return doNormalize; }
		
		// �}�X�^�[�s�b�`
		void SetMasterPitchBase(int pitch){ masterPitch = pitch; }
		
		// �L�[�V�t�g
		void SetKeyShift(int key){ keyShift = key; }
		
		// �V�t�g���ꂽ�m�[�g�ԍ����擾
		int GetShiftedNoteBase(int note, int channel);
		
		// �}�X�^�[�s�b�`�𑗐M
		void SendMasterPitchBase(IDevice* device);
		
		// �s�b�`�x���h���b�Z�[�W�𑗐M
		void SendPitchBendBase(IDevice* device, int channel, int pitch);
		
		// �G�N�X�N���[�V�u�𑗐M
		void SendExclusiveBase(IDevice* device, CodeEvent* codeEvent);

		// �s�b�`�x���h�Z���V�r���e�B��������
		void InitializePitchBendSensitivity(IDevice* device);
		
		// SMF�ŏ�����
		virtual void Initialize(Smf* newSmf);
		
		// �`���֊����߂�
		virtual void Rewind();
		
		// PlayOption������
		void ParsePlayOption(int option);
		
		// �C�x���g�����s(Play)
		void PlayingEventExecute(CodeEvent* ev, IPlayerCore* core);
		
		// �C�x���g�����s(Seek)
		void SeekingEventExecute(CodeEvent* ev, IPlayerCore* core);
		
		// �C�x���g���s�I�u�W�F�N�g�̔z����쐬(Play)
		void MakePlayCodeEventExcutors();
		
		// �C�x���g���s�I�u�W�F�N�g�̔z����쐬(Seek)
		void MakeSeekCodeEventExcutors();
		
	public:
		PlayerBase();	// �R���X�g���N�^
		virtual ~PlayerBase();	// �f�X�g���N�^
	};

}}}

#endif
