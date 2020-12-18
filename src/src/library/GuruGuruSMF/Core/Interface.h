// �����Ŏg�p����C���^�[�t�F�C�X

// #pragma once
#ifndef INTERFACE_H
#define INTERFACE_H

#include <Windows.h>
#include "Smf.h"


namespace GuruGuruSmf { namespace Core
{
	/// <summary>
	/// SMF�R���g���[���[�̃C���^�[�t�F�C�X
	/// </summary>
	struct ISmfController
	{
		// SMF�I�u�W�F�N�g���擾
		virtual Smf* GetSmf() = 0;

		// �J��
		virtual GGSERROR Open() = 0;

		// ����
		virtual void Close() = 0;
	};

	/// <summary>
	/// �v���C���[�̃C���^�[�t�F�C�X
	/// </summary>
	struct IPlayer
	{
		// �f�X�g���N�^(����������Ȃ���㩂Ƀn�}��܂���)
		virtual ~IPlayer(){}

		// �f�o�C�X���J��
		virtual GGSERROR OpenDevice(int deviceId, HWND handle) = 0;
		
		// �f�o�C�X�����
		virtual void CloseDevice() = 0;
		
		// ���t�I
		virtual GGSERROR Play(ISmfController* smfCtrl, int option, int seek, int fadeInTime, int keyShift) = 0;
		
		// ��~�I
		virtual void Stop(int fadeOutTime) = 0;
		
		// �ꎞ��~
		virtual void Pause() = 0;
		
		// �ꎞ��~����
		virtual void Restart() = 0;
		
		// ���t��Ԃ��擾
		virtual void GetStatus(PlayerStatus* status) = 0;
		
		// �}�X�^�[�e���|
		virtual void SetMasterTempo(float tempo) = 0;
		
		// �}�X�^�[�{�����[��
		virtual void SetMasterVolume(int volume) = 0;
		
		// �}�X�^�[�s�b�`
		virtual void SetMasterPitch(int pitch) = 0;
	};
	
	/// <summary>
	/// �f�o�C�X�̃C���^�[�t�F�C�X
	/// </summary>
	struct IDevice
	{
		// ����MIDI���b�Z�[�W�𑗐M
		virtual void SendLongMessage(const byte* data, int length) = 0;
		
		// �Z��MIDI���b�Z�[�W�𑗐M
		virtual void SendShortMessage(int status, int data1, int data2) = 0;
	};
	
	/// <summary>
	/// Player�����A�N�Z�X�p�̃C���^�[�t�F�C�X
	/// </summary>
	struct IPlayerCore
	{
		// MIDI���b�Z�[�W���M�f�o�C�X���擾
		virtual IDevice* GetDevice() = 0;
		
		// �}�[�J�[Tick�ɉ��Z����
		virtual void AddMarkerTick(int deltaTime) = 0;
		
		// �I�_����
		virtual void MusicEnd() = 0;
		
		// �e���|���Z�b�g
		virtual void SetTempo(int newTempo) = 0;
		
		// �{�����[�����b�Z�[�W�𑗐M
		virtual void SendVolume(int channel, int volume) = 0;
		
		// �s�b�`�x���h���b�Z�[�W�𑗐M
		virtual void SendPitchBend(int channel, int pitch) = 0;
		
		// �G�N�X�N���[�V�u�𑗐M
		virtual void SendExclusive(CodeEvent* codeEvent) = 0;
		
		// Seek�p�ɃC�x���g��o�^
		virtual void RegisterEvent(CODE_EVENTTYPE type, int channel, int data1, CodeEvent* codeEvent) = 0;

		// �V�t�g���ꂽ�m�[�g�ԍ����擾
		virtual int GetShiftedNote(int note, int channel) = 0;
	};
	
	/// <summary>
	/// Analyzer�̓����A�N�Z�X�p�̃C���^�[�t�F�C�X
	/// </summary>
	struct IAnalyzerCore
	{
		virtual void SetTempo(int newTempo) = 0;
		virtual void NoteEvent(bool isNoteOn) = 0;
		virtual void ProgramChangeEvent(int channel, int voice) = 0;
		virtual void BankSelectEvent(int channel, bool isMsb, int bank) = 0;
		virtual void UseRhythmPartEvent(int channel, bool isRhythmPart) = 0;
		virtual void VolumeEvent(int volume) = 0;
		virtual void MarkLoopPoint() = 0;
	};
	
	/// <summary>
	/// Compiler�̓����A�N�Z�X�p�̃C���^�[�t�F�C�X
	/// </summary>
	struct ICompilerCore
	{
		// Code�I�u�W�F�N�g
		virtual GgsCode* GetCode() = 0;
		
		// 2�o�C�g�̃f�[�^���������C�x���g����������
		virtual void Write2ByteEvent(CODE_EVENTTYPE type, SmfEvent* smfEvent) = 0;
		
		// �G�N�X�N���[�V�u�C�x���g����������
		virtual void WriteExclusiveEvent(SmfEvent* smfEvent) = 0;
		
		// ���Y���p�[�g�C�x���g����������
		virtual void WriteUseRhythmPartEvent(SmfEvent* smfEvent, int channel, bool rhythm) = 0;
	};
	
	
	
	/// <summary>
	/// DirectMusic�̃^�C�}�[�@�\
	/// </summary>
	struct IDirectMusicTimer
	{
		virtual __int64 GetMarkerDmTime() = 0;
		virtual void IncrementMarkerDmTime() = 0;
	};
	
}}

#endif
