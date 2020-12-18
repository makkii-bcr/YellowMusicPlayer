// SMF�f�[�^�֘A

// #pragma once
#ifndef SMF_H
#define SMF_H

#include <Windows.h>
#include <string>
#include <vector>
#include <set>

#include "Constants.h"

namespace GuruGuruSmf { namespace Core {

	/// <summary>
	/// ���F���X�g
	/// </summary>
	class VoiceList
	{
	private:
		int count;
		uint* items;

	public:
		VoiceList();	// �R���X�g���N�^
		~VoiceList();	// �f�X�g���N�^
		
		// set�z�񂩂�R�s�[
		void CopyFrom(std::set<uint>* src);

		// �v�f�����擾
		int GetCount(){ return count; }

		// �v�f���擾
		uint* GetItems(){ return items; }
	};



	/// <summary>
	/// SMF�̉�̓f�[�^
	/// </summary>
	struct SmfAnalyzedData
	{
		int TotalTick;
		int TotalTime;
		int FirstNoteTick;
		double FirstNoteTime;
		int LastNoteTick;
		double LastNoteTime;
		int LoopTick;
		double LoopTime;
		int MaxVolume;
		int EventCount;
		VoiceList* VoiceList;
		LOOPTYPE LoopType;
	};


	/// <summary>
	/// �G�N�X�N���[�V�u���b�Z�[�W1��(0xF0�Ŏn�܂���̐�p)
	/// </summary>
	class ExclusiveData
	{
	private:
		byte* binary;
		int length;

	public:
		ExclusiveData(byte* data, int len);	// �R���X�g���N�^
		~ExclusiveData();	// �f�X�g���N�^

		byte* GetData(){ return binary; }
		int GetLength(){ return length; }
	};


	// GgsCode�̃C�x���g1��(8�o�C�g�Œ�)
	struct CodeEvent
	{
		union
		{
			int Header;
			struct
			{
				unsigned char Type;
				unsigned char Channel;
				unsigned char Option1;
				unsigned char Option2;
			};
		};

		union
		{
			int IntData;
			struct
			{
				unsigned char ByteData1;
				unsigned char ByteData2;
				unsigned char ByteData3;
				unsigned char ByteData4;
			};
		};
	};

	/// <summary>
	/// GuruGuruSMF�����R�[�h������
	/// </summary>
	class GgsCode
	{
	private:
		std::vector<ExclusiveData*> exclusiveList;
		int length, position;
		int* binary;

	public:
		GgsCode();	// �R���X�g���N�^
		~GgsCode();	// �f�X�g���N�^

		// ����
		int GetLength(){ return length; }
		
		// ���݂̈ʒu(�m�[�`�F�b�N)
		int GetPosition(){ return position; }
		void SetPosition(int pos){ position = pos; }
		
		// ������ݒ�
		void SetLength(int newLength);
		
		// ���݂̈ʒu�ɃC�x���g��������
		void WriteEvent(CodeEvent* ev);
		
		// ���݈ʒu����C�x���g�ǂݍ���
		void ReadEvent(CodeEvent* ev);

		// ������0�ɂ��ăG�N�X�N���[�V�u�f�[�^���J��
		void Clear();
		
		// �G�N�X�N���[�V�u�f�[�^��o�^������ID���擾
		int ResisterExclusive(ExclusiveData* exclusive);
		
		// �G�N�X�N���[�V�u�f�[�^���擾
		ExclusiveData* GetExclusive(int index);
	};

	/// <summary>
	/// SMF�̃C�x���g1��������
	/// </summary>
	struct SmfEvent
	{
		SMF_EVENTTYPE Type;		// �C�x���g
		int MidiStatus;			// MIDI�X�e�[�^�X
		int DeltaTime;			// �f���^�^�C��
		int Data1;				// �f�[�^1
		int Data2;				// �f�[�^2
		int EndPosition;		// �g���b�N�̏I�[(�C�x���g�擾���Ɏg�p)
		int ExclusiveLength;	// �G�N�X�N���[�V�u�̒���
		byte* ExclusiveData;	// �G�N�X�N���[�V�u�f�[�^
	};


	/// <summary>
	/// SMF�̃o�C�i���f�[�^������
	/// </summary>
	class SmfStream
	{
	private:
		union Int32BE
		{
			int Value;
			byte B[4];
		};

		int position, length;
		byte* binary;
		const byte* baseAddress;
		bool isAllocate;

		// �ϒ��̐��l��ǂݍ���
		GGSERROR ReadVInt(int& intData, int endPosition);

		// 2�o�C�g�f�[�^�̃C�x���g���擾
		GGSERROR ReadEvent2Data(SmfEvent* smfEvent, SMF_EVENTTYPE type);

		// 1�o�C�g�f�[�^�̃C�x���g���擾
		GGSERROR ReadEvent1Data(SmfEvent* smfEvent, SMF_EVENTTYPE type);

		// �s�b�`�x���h�C�x���g���擾
		GGSERROR ReadPitchBend(SmfEvent* smfEvent);

		// �G�N�X�N���[�V�u�C�x���g���擾(0xf0) 0xf7����n�܂���͖̂���
		GGSERROR ReadExclusive(SmfEvent* smfEvent, int status);

		// ���^�C�x���g���擾
		GGSERROR ReadMetaEvent(SmfEvent* smfEvent);

	public:
		SmfStream();	// �R���X�g���N�^
		~SmfStream();	// �f�X�g���N�^

		// ���݂̈ʒu
		int GetPosition(){ return position; }

		// �f�[�^�̃|�C���^���擾
		const byte* GetBaseAddress(){ return baseAddress; }

		// ����
		int GetLength(){ return length; }

		// �t�@�C������ǂݍ���
		GGSERROR LoadFromFile(LPCWSTR fileName);

		// ����������ǂݍ���
		GGSERROR LoadFromMemory(const byte* smfBinary, int newLength);

		// �j��
		void Unload();

		// �V�[�N
		GGSERROR Seek(int pos);

		// ���݈ʒu����2�o�C�g�̐��l���擾
		GGSERROR ReadUShort(int& intData);

		// ���݈ʒu����4�o�C�g�̐��l���擾
		GGSERROR ReadInt(int& intData);

		// ���݈ʒu����f���^�^�C�����擾
		GGSERROR ReadDeltaTime(int& deltaTime, int endPosition);

		// ���݈ʒu����SMF�C�x���g��1�{�f���^�^�C�����擾
		GGSERROR ReadSmfEvent(SmfEvent* smfEvent);

	};

	/// <summary>
	/// SMF�̃g���b�N1�����Ǘ�����
	/// </summary>
	class SmfTrack
	{
	private:
		SmfEvent smfEvent;
		SmfStream* smfStream;
		int beginPosition, position, length;

	public:
		// �������p�\����
		struct InitParam
		{
			SmfStream* SmfStream;
			int BeginPosition;
			int Length;
		};

		// ���݂̈ʒu
		int GetPosition(){ return position; }
		void SetPosition(int pos){ position = pos; }

		// ������
		void Initialize(InitParam* initParam);

		// �ŏ��Ɋ����߂�
		void Rewind();

		// ���݈ʒu����f���^�^�C�����擾
		int ReadDeltaTime(GGSERROR& error);

		// ���݈ʒu����SMF�C�x���g��1�{�f���^�^�C�����擾
		SmfEvent* ReadSmfEvent(GGSERROR& error);

	};



	/// <summary>
	/// SMF1�����Ǘ�����N���X
	/// </summary>
	class Smf
	{
	private:
		bool isReady, buffered;
		SmfStream* smfStream;
		std::wstring smfFileName;
		int trackCount, timeBase;
		SmfTrack** tracks;
		GgsCode* code;
		VoiceList* voiceList;
		SmfAnalyzedData data;
		STOREDTYPE storedType;

		// �w�b�_�[�ǂݍ���
		GGSERROR ReadHeader();

		// �����f�[�^���N���A
		void ClearData();

	public:
		Smf();	// �R���X�g���N�^
		~Smf();	// �f�X�g���N�^

		// ��̓f�[�^�I�u�W�F�N�g
		SmfAnalyzedData* GetData(){ return &data; }

		// GGSCode�R�[�h�I�u�W�F�N�g
		GgsCode* GetCode(){ return code; }

		// �������[���m�ۂ��ď�ɕۑ����Ă������ǂ���
		bool GetBuffered(){ return buffered; }
		void SetBuffered(bool b){ buffered = b; }

		// �t�@�C���ۑ����������ۑ���
		STOREDTYPE GetStoredType(){ return storedType; }

		// �t�@�C����
		const wchar_t* GetFileName(){ return smfFileName.c_str(); }

		// �f�[�^�̃|�C���^���擾
		const byte* GetBaseAddres(){ return smfStream->GetBaseAddress(); }

		// ����
		int GetLength(){ return smfStream->GetLength(); }

		// SMF�Ƃ��ėL�����ǂ���
		bool GetIsReady(){ return isReady; }

		// �g���b�N��
		int GetTrackCount(){ return trackCount; }

		// �^�C���x�[�X
		int GetTimeBase(){ return timeBase; }

		// �t�@�C������ǂݍ���
		GGSERROR LoadFromFile(LPCWSTR fileName);

		// ����������ǂݍ���
		GGSERROR LoadFromMemory(const byte* smfBinary, int length);

		// SMF�o�C�i����j��
		void ReleaseSmfBinary(){ smfStream->Unload(); }

		// �R�[�h�o�C�i����j��
		void ReleaseCodeBinary(){ code->Clear(); }

		// �ēǂݍ���
		GGSERROR Reload();

		// �g���b�N�I�u�W�F�N�g���擾
		SmfTrack* GetTrack(int index){ return tracks[index]; }

		// �ŏ��֊����߂�
		void Rewind();

	};



}}

#endif
