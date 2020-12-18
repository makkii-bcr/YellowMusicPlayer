// GuruGuruSMF4�̒萔

// #pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef unsigned char byte;
typedef unsigned int uint;

namespace GuruGuruSmf {

	namespace PlayOption
	{
		const int Loop = 1;			// ���[�v�Đ�����
		const int SkipBeginning = 2;	// �`���̋󔒂��X�L�b�v
		const int SkipEnding = 4;		// �����̋󔒂��X�L�b�v
		const int MasterPitch = 8;		// �}�X�^�[�s�b�`��L���ɂ���
		const int Normalize = 16;		// �ő剹�ʂ�127�Ɉ����グ��
		const int Exclusive = 32;		// �G�N�X�N���[�V�u�𑗐M����
	}
	
	namespace LoadOption
	{
		const int Buffered = 1;		// �f�[�^������ɕێ����邩
	}
	
	namespace Device
	{
		const int DirectMusicPerformance = -1000;	// DirectMusic Performance���[�h(�p�~)
		const int DirectMusic = -1100;				// DirectMusic Port���[�h
	}
	
	/// <summary>
	/// �G���[�R�[�h
	/// </summary>
	namespace GgsError {
		enum Code {
			NoError, 			// �G���[�Ȃ�
			Failed,				// �Ȃ񂾂��悭�킩��Ȃ����ǎ��s
			FileNotFound,		// �t�@�C�������݂��Ȃ�
			CannotOpenFile,		// �t�@�C�����J���Ȃ�
			FileLoadFailed,		// �t�@�C���ǂݍ��݂Ɏ��s
			BadFileSize,		// �t�@�C���T�C�Y���s��
			NoSmf,				// SMF�ł͂���܂���
			BadSmf,				// �s����SMF
			UnmatchedSmf,		// ���Ή���SMF
			CannotOpenDevice,	// �f�o�C�X���J���Ȃ�
			CannotStartTimer,	// �^�C�}�[���J�n�ł��Ȃ�
			IllegalPointer,		// �͈͊O���Q�Ƃ��悤�Ƃ���
			TrackLimit,			// ���b�Z�[�W�̓r���Ńg���b�N���I�[�ɒB����
			NotReady,			// ���t�\�ł͂Ȃ�
			UndefinedId,		// ����`��ID
			ApiNull				// API���L���łȂ�
		};
	}
	typedef GgsError::Code GGSERROR;
	

	/// <summary>
	/// �v���C���[�̏��
	/// </summary>
	namespace PlayerState {
		enum Code {
			Stop,		// ��~��
			Play,		// ���t��
			Pause		// �ꎞ��~��
		};
	}
	typedef PlayerState::Code PLAYERSTATE;

	/// <summary>
	/// �v���C���[�̏��
	/// </summary>
	struct PlayerStatus
	{
		PLAYERSTATE State;
		int Tempo;
		int Tick;
		int Time;
		int LoopCount;
	};

	/// <summary>
	/// PerformancePlay�֓n��SMF�f�[�^
	/// </summary>
	struct PerformancePlaySmfData
	{
		int StoredType;
		const wchar_t* FileName;
		void* SmfBinary;
		int SmfBinaryLength;
		int TimeBase;
		int TotalTick;
		int FirstNoteTick;
		int LastNoteTick;
		int LoopTick;
	};

	/// <summary>
	/// SMF�̃f�[�^
	/// </summary>
	struct SmfInformation
	{
		int TimeBase;		// �x�[�X�J�E���g
		int TotalTick;		// ��Tick
		int TotalTime;		// �����t����
		int FirstNoteTick;	// �ŏ��̔���tick
		int FirstNoteTime;	// �ŏ��̔����̎���
		int LastNoteTick;	// �Ō�̔���tick
		int LastNoteTime;	// �Ō�̔����̎���
		int LoopTick;		// CC#111�̈ʒu
		int LoopTime;		// CC#111�̈ʒu�̎���
	};

	/// <summary>
	/// SMF���X�g�̃L�[�ꗗ
	/// </summary>
	struct SmfListKeys
	{
		int Count;		// �L�[�̐�
		int* Keys;		// �L�[�̃��X�g
	};

}


namespace GuruGuruSmf { namespace Core
{
	
	/// <summary>
	/// �g���b�N�̏��
	/// </summary>
	namespace TrackState {
		enum Code {
			Play,		// ���t(���)��
			End			// �I��
		};
	}
	typedef TrackState::Code TRACKSTATE;

	/// <summary>
	/// �f�[�^�̕ۑ��^�C�v
	/// </summary>
	namespace StoredType {
		enum Code {
			File, Memory
		};
	}
	typedef StoredType::Code STOREDTYPE;
	
	/// <summary>
	/// SMF�C�x���g�萔
	/// </summary>
	namespace SmfEventType {
		enum Code {
			NoteOff, NoteOn, ProgramChange, ControlChange,
			PitchBend, CAf, PAf,
			MetaEvent, Tempo, Exclusive,
			TrackEnd
		};
	}

	typedef SmfEventType::Code SMF_EVENTTYPE;
	
	// �G�N�X�N���[�V�u���b�Z�[�W
	namespace Exclusive
	{
		// GM�V�X�e���I��
		const int GMSystemOnLength = 6;
		const byte GMSystemOn[6] = {0xf0,0x7e,0x7f,0x09,0x01,0xf7};
		
		// GS���Z�b�g
		const int GSResetLength = 11;
		const byte GSReset[11] = {0xf0,0x41,0x10,0x42,0x12,0x40,0x00,0x7f,0x00,0x41,0xf7};
	}
	
	// ���[�v�^�C�v
	namespace LoopType {
		enum Code {
			Beginning,				// �ŏ���(���[�v�|�C���g�Ȃ�)
			LoopPointAtBeginning,	// ���[�v�|�C���g�ōŏ���
			LoopPoint,				// ���[�v�|�C���g��
			End						// �I������
		};
	}

	typedef LoopType::Code LOOPTYPE;
	
	// GGSCode�̃C�x���g�^�C�v
	namespace CodeEventType {
		enum Code {
			DeltaTime, NoteOn, NoteOff,
			ProgramChange, ControlChangeA, ControlChangeB, ControlChangeC, Volume,
			PAf, CAf, PitchBend,
			Tempo, Exclusive, UseRhythmPart,
			MusicEnd
		};
	}

	typedef CodeEventType::Code CODE_EVENTTYPE;
	
	// �{�����[���R���g���[���̏��
	namespace VolumeState {
		enum Code {
			Normal, FadeIn, FadeOut
		};
	}

	typedef VolumeState::Code VOLUMESTATE;
	
	
	// ���̑��萔
	namespace Constant {
		const int MaxTick = 0x70000000;		// ������Tick�̍ő�l(�Ƃ肠�������̐���)
		const __int64 PrepareTime = 600000;		// Port���[�h�Ő揈�����鎞��(60ms)
		const int TimeInterval = 5;			// �񓯊���������(5ms)
		const double DefaultTempo = 500000;			// �e���|�̏����l(500000 = bpm120)
		const int GSChannel[] = {9,0,1,2,3,4,5,6,7,8,10,11,12,13,14,15};	// GS������ch�z��
	}


}}




#endif
