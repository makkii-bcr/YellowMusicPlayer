
#include "stdafx.h"

#include "Smf.h"


namespace GuruGuruSmf { namespace Core {

	//
	// VoiceList
	//

	// �R���X�g���N�^
	VoiceList::VoiceList()
	{
		count = 0;
		items = 0;
	}

	// �f�X�g���N�^
	VoiceList::~VoiceList()
	{
		delete[] items;
	}

	// set�z�񂩂�R�s�[
	void VoiceList::CopyFrom(std::set<uint>* src)
	{
		delete[] items;
		items = 0;
		count = src->size();
		TRACE1(L"VoiceList::CopyFrom(%d)\n: ", count);
		if(count == 0) return;	// ���肦�Ȃ��ł����ꉞ
		
		items = new uint[count];
		int i=0;
		std::set<uint>::iterator it = src->begin();
		while(it != src->end())
		{
			items[i] = *it;
			TRACE1(L"%x ", items[i]);
			++i;
			++it;
		}
		TRACE(L"\n");
	}


	//
	// ExclusiveData
	//

	// �R���X�g���N�^
	ExclusiveData::ExclusiveData(byte* data, int len)
	{
		length = len + 1;
		if(len < 0){
			binary = 0;
			return;
		}
		TRACE1(L"- ExclusiveData(%d): ", len+1);
		binary = new byte[len+1];
		binary[0] = 0xf0;
		TRACE(L"0xf0 ");
		memcpy(binary+1, data, len);
		TRACE4(L"%x %x %x %x\n", binary[1], binary[2], binary[3], binary[4]);
	}

	// �f�X�g���N�^
	ExclusiveData::~ExclusiveData()
	{
		delete[] binary;
	}



	//
	// GgsCode
	//

	// �R���X�g���N�^
	GgsCode::GgsCode()
	{
		binary = 0;
	}

	// �f�X�g���N�^
	GgsCode::~GgsCode()
	{
		Clear();
	}

	// �������w�肵�ă��������m�ۂ���
	void GgsCode::SetLength(int newLength)
	{
		delete[] binary;

		if(newLength < 0){
			binary = 0;
		}else{
			// ���̂Ƃ���8�o�C�g�Œ�Ȃ̂Ńn�[�h�R�[�f�B���O
			binary = new int[newLength * 2];
		}
		length = newLength;
		position = 0;
	}

	// ���݂̈ʒu�ɃC�x���g��������
	void GgsCode::WriteEvent(CodeEvent* ev)
	{
		int pos = position * 2;
		binary[pos++] = ev->Header;
		binary[pos] = ev->IntData;
		++position;
	}

	// ���݈ʒu����C�x���g�ǂݍ���
	void GgsCode::ReadEvent(CodeEvent* ev)
	{
		int pos = position * 2;
		ev->Header = binary[pos++];
		ev->IntData = binary[pos];
		++position;
	}

	// �G�N�X�N���[�V�u�f�[�^��o�^������ID���擾
	int GgsCode::ResisterExclusive(ExclusiveData* exclusive)
	{
		exclusiveList.push_back(exclusive);
		return exclusiveList.size() - 1;
	}
		
	// �G�N�X�N���[�V�u�f�[�^���擾
	ExclusiveData* GgsCode::GetExclusive(int index)
	{
		return exclusiveList[index];
	}

	// ������0�ɂ��ăG�N�X�N���[�V�u�f�[�^���J��
	void GgsCode::Clear()
	{
		for(int i=0; i<(int)exclusiveList.size(); i++){
			delete exclusiveList[i];
		}
		exclusiveList.clear();
		SetLength(0);
	}



	//
	// SmfStream
	//

	// �R���X�g���N�^
	SmfStream::SmfStream()
	{
		binary = 0;
		position = 0;
		length = 0;
	}

	// �f�X�g���N�^
	SmfStream::~SmfStream()
	{
		delete[] binary;
	}

	// �t�@�C������ǂݍ���
	GGSERROR SmfStream::LoadFromFile(LPCWSTR fileName)
	{
		TRACE(L"��SmfStream::LoadFromFile\n");

		HANDLE handle = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		
		DWORD errCode;
		if(handle == INVALID_HANDLE_VALUE){	// ���ʂɎ��s�����P�[�X
			errCode = GetLastError();
			if(errCode == 2)	return GgsError::FileNotFound;
 			return GgsError::CannotOpenFile;
		}else if(handle == NULL){	// Windows9x?
			char fileNameA[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, fileName, -1,
				fileNameA, MAX_PATH, NULL, NULL);
			handle = CreateFileA(fileNameA, GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(handle == INVALID_HANDLE_VALUE){
				errCode = GetLastError();
				if(errCode == 2)	return GgsError::FileNotFound;
	 			return GgsError::CannotOpenFile;
			}
		}

		length = GetFileSize(handle, NULL);
		if(length < 1){
			CloseHandle(handle);
			return GgsError::BadFileSize;
		}

		binary = new byte[length];
		baseAddress = binary;

		unsigned long xlen;
		ReadFile(handle, binary, length, &xlen, NULL);
		CloseHandle(handle);

		TRACE2(L"  Length: %d/%d\n", xlen, length);

		position = 0;
		return GgsError::NoError;
	}

	// ����������ǂݍ���
	GGSERROR SmfStream::LoadFromMemory(const byte* smfBinary, int newLength)
	{
		delete[] binary;
		binary = 0;

		baseAddress = smfBinary;
		length = newLength;
		position = 0;

		return GgsError::NoError;
	}


	// �j��
	void SmfStream::Unload()
	{
		if(binary){
			delete[] binary;
			binary = 0;
			length = 0;	// �������[����ǂ񂾎��ɂ��ꂪ�ۑ����ꂽ�܂܂Ȃ̂��~�\
		}
		position = 0;
	}

	// �V�[�N
	GGSERROR SmfStream::Seek(int pos)
	{
		if(pos < 0 || length <= pos) return GgsError::IllegalPointer;
		position = pos;
		return GgsError::NoError;
	}

	// ���݈ʒu����2�o�C�g�̐��l���擾
	GGSERROR SmfStream::ReadUShort(int& intData)
	{
		if(position+2 >= length) return GgsError::IllegalPointer;	// �͈͊O
		
		Int32BE data;
		data.B[3] = 0;
		data.B[2] = 0;
		data.B[1] = baseAddress[position++];
		data.B[0] = baseAddress[position++];
		intData = data.Value;
		
		return GgsError::NoError;
	}

	// ���݈ʒu����4�o�C�g�̐��l���擾
	GGSERROR SmfStream::ReadInt(int& intData)
	{
		if(position+4 >= length) return GgsError::IllegalPointer;	// �͈͊O
		
		Int32BE data;
		data.B[3] = baseAddress[position++];
		data.B[2] = baseAddress[position++];
		data.B[1] = baseAddress[position++];
		data.B[0] = baseAddress[position++];
		intData = data.Value;
		
		return GgsError::NoError;
	}

	// ���݈ʒu����f���^�^�C�����擾
	GGSERROR SmfStream::ReadDeltaTime(int& deltaTime, int endPosition)
	{
		return ReadVInt(deltaTime, endPosition);
	}

	// ���݈ʒu����SMF�C�x���g��1�{�f���^�^�C�����擾
	GGSERROR SmfStream::ReadSmfEvent(SmfEvent* smfEvent)
	{
		int endPos = smfEvent->EndPosition;
		if(position >= endPos) return GgsError::TrackLimit;
		
		int status = baseAddress[position];
		if(status < 0x80){	// �����j���O�X�e�[�^�X
			status = smfEvent->MidiStatus;
		}else{
			++position;
		}
		
		GGSERROR err = GgsError::NoError;
		if (status < 0xf0){	// MIDI�C�x���g
			smfEvent->MidiStatus = status;
			switch(status & 0xf0){
			case 0x90:	// �m�[�g�I��
				err = ReadEvent2Data(smfEvent, SmfEventType::NoteOn);
				break;
			case 0x80:	// �m�[�g�I�t
				err = ReadEvent2Data(smfEvent, SmfEventType::NoteOff);
				break;
			case 0xe0:	// �s�b�`�x���h
				err = ReadPitchBend(smfEvent);
				break;
			case 0xa0: // �|���t�H�j�b�N�L�[�v���b�V���[
				err = ReadEvent2Data(smfEvent, SmfEventType::PAf);
				break;
			case 0xb0: // �R���g���[���`�F���W
				err = ReadEvent2Data(smfEvent, SmfEventType::ControlChange);
				break;
			case 0xc0: // �v���O�����`�F���W
				err = ReadEvent1Data(smfEvent, SmfEventType::ProgramChange);
				break;
			case 0xd0: // �`�F���l���v���b�V���[
				err = ReadEvent1Data(smfEvent, SmfEventType::CAf);
				break;
			}
		}else{
			switch(status){
			case 0xf0: case 0xf7:	// �G�N�X�N���[�V�u
				err = ReadExclusive(smfEvent, status);
				break;
			case 0xff:	// ���^�C�x���g
				err = ReadMetaEvent(smfEvent);
				break;
			default:
				return GgsError::BadSmf;
			}
		}
		if (err != GgsError::NoError) return err;

		// �f���^�^�C���擾
		int data;
		err = ReadVInt(data, endPos);
		smfEvent->DeltaTime = data;
		return err;
	}



	// �ϒ��̐��l��ǂݍ���
	GGSERROR SmfStream::ReadVInt(int& intData, int endPosition)
	{
		intData = 0;

		for(int i=0; i<4; i++)
		{
			intData <<= 7;
			if(position >= endPosition) return GgsError::TrackLimit;
			int a = baseAddress[position++];
			intData += a & 0x7f;
			if(a < 0x80)	break;
		}
		return GgsError::NoError;
	}

	// 2�o�C�g�f�[�^�̃C�x���g���擾
	GGSERROR SmfStream::ReadEvent2Data(SmfEvent* smfEvent, SMF_EVENTTYPE type)
	{
		if(position+1 >= smfEvent->EndPosition) return GgsError::TrackLimit;
		
		smfEvent->Data1 = baseAddress[position++];
		smfEvent->Data2 = baseAddress[position++];
		smfEvent->Type = type;
		
		return GgsError::NoError;
	}

	// 1�o�C�g�f�[�^�̃C�x���g���擾
	GGSERROR SmfStream::ReadEvent1Data(SmfEvent* smfEvent, SMF_EVENTTYPE type)
	{
		if(position >= smfEvent->EndPosition) return GgsError::TrackLimit;
		
		smfEvent->Data1 = baseAddress[position++];
		smfEvent->Type = type;
		
		return GgsError::NoError;
	}

	// �s�b�`�x���h�C�x���g���擾
	GGSERROR SmfStream::ReadPitchBend(SmfEvent* smfEvent)
	{
		if(position+1 >= smfEvent->EndPosition) return GgsError::TrackLimit;
		
		int data = baseAddress[position++];
		data += baseAddress[position++] << 7;
		smfEvent->Data1 = data - 8192;
		
		smfEvent->Type = SmfEventType::PitchBend;
		return GgsError::NoError;
	}

	// �G�N�X�N���[�V�u�C�x���g���擾(0xf0) 0xf7����n�܂���͖̂���
	GGSERROR SmfStream::ReadExclusive(SmfEvent* smfEvent, int status)
	{
		int endPos = smfEvent->EndPosition;
		int len;
		GGSERROR err = ReadVInt(len, endPos);
		if(err != GgsError::NoError) return err;
		
		int nextPos = position + len;
		if(nextPos >= endPos) return GgsError::TrackLimit;
		
		// ����99�܂ŗL��
		if(len < 100 && status == 0xf0){
			TRACE1(L"- Exculusive 0xf0: %d\n", len);
			smfEvent->ExclusiveLength = len;
			smfEvent->ExclusiveData = (byte*)&baseAddress[position];
		}else{
			smfEvent->ExclusiveLength = -1;	// ��������ꍇ��-1
		}
		
		position = nextPos;
		smfEvent->Type = SmfEventType::Exclusive;
		
		return GgsError::NoError;
	}

	// ���^�C�x���g���擾
	GGSERROR SmfStream::ReadMetaEvent(SmfEvent* smfEvent)
	{
		int endPos = smfEvent->EndPosition;
		if(position >= endPos) return GgsError::TrackLimit;
		int type = baseAddress[position++];
		
		if(type == 0x2f){	// End of Track
			smfEvent->Type = SmfEventType::TrackEnd;
			return GgsError::NoError;	// ���ꂾ���͖������ŕԂ�
		}
		
		int len;
		GGSERROR err = ReadVInt(len, endPos);
		if(err != GgsError::NoError) return err;
		
		smfEvent->Type = SmfEventType::MetaEvent;
		int nextPos = position + len;
		if(nextPos >= endPos) return GgsError::TrackLimit;
		
		switch(type)
		{
		case 0x51: // Tempo: �Ή��͌��݂���̂�
			if(len == 3){	// �f�[�^����3�o�C�g�łȂ����̂̓X���[
				Int32BE data;
				data.B[3] = 0;
				data.B[2] = baseAddress[position];
				data.B[1] = baseAddress[position+1];
				data.B[0] = baseAddress[position+2];
				
				smfEvent->Type = SmfEventType::Tempo;
				smfEvent->Data1 = data.Value;
			}
			break;
		}
		position = nextPos;
		return GgsError::NoError;
	}



	//
	// SmfTrack
	//

	// ������
	void SmfTrack::Initialize(InitParam* initParam)
	{
		smfStream = initParam->SmfStream;
		beginPosition = initParam->BeginPosition;
		length = initParam->Length;
		
		smfEvent.EndPosition = beginPosition + length;
		Rewind();
	}

	// �ŏ��Ɋ����߂�
	void SmfTrack::Rewind()
	{
		position = beginPosition;
		smfEvent.MidiStatus = 0;
	}

	// ���݈ʒu����f���^�^�C�����擾
	int SmfTrack::ReadDeltaTime(GGSERROR& error)
	{
		int dt;
		smfStream->Seek(position);
		error = smfStream->ReadDeltaTime(dt, smfEvent.EndPosition);
		position = smfStream->GetPosition();
		return dt;
	}

	// ���݈ʒu����SMF�C�x���g��1�{�f���^�^�C�����擾
	SmfEvent* SmfTrack::ReadSmfEvent(GGSERROR& error)
	{
		smfStream->Seek(position);
		error = smfStream->ReadSmfEvent(&smfEvent);
		position = smfStream->GetPosition();
		return &smfEvent;
	}


	//
	// Smf
	//

	// �R���X�g���N�^
	Smf::Smf()
	{
		code = new GgsCode();
		smfStream = new SmfStream();
		tracks = 0;
		voiceList = 0;
		trackCount = 0;
		voiceList = new VoiceList();

		data.VoiceList = voiceList;

		isReady = false;
		buffered = false;
	}

	// �f�X�g���N�^
	Smf::~Smf()
	{
		ClearData();

		delete smfStream;
		delete code;
		delete voiceList;

		for (int i=0; i<trackCount; i++){
			delete tracks[i];
		}
		delete[] tracks;
	}

	// �t�@�C������ǂݍ���
	GGSERROR Smf::LoadFromFile(LPCWSTR fileName)
	{
		TRACE(L"��Smf::LoadFromFile: ");
		TRACE(fileName);
		TRACE(L"\n");
		isReady = false;
		ClearData();
		GGSERROR err = smfStream->LoadFromFile(fileName);
		if (err != GgsError::NoError) return err;
		
		err = ReadHeader();
		TRACE(L"\n");
		if (err != GgsError::NoError) return err;
		
		smfFileName = fileName;
		storedType = StoredType::File;

		isReady = true;
		return GgsError::NoError;
	}

	// �������[����ǂݍ���
	GGSERROR Smf::LoadFromMemory(const byte* smfBinary, int length)
	{
		isReady = false;
		ClearData();
		GGSERROR err = smfStream->LoadFromMemory(smfBinary, length);
		if (err != GgsError::NoError) return err;
		
		err = ReadHeader();
		if (err != GgsError::NoError) return err;
		
		storedType = StoredType::Memory;

		isReady = true;
		return GgsError::NoError;
	}

	// �ēǂݍ���
	GGSERROR Smf::Reload()
	{
		GGSERROR err = GgsError::NoError;
		if(storedType == StoredType::File){
			err = smfStream->LoadFromFile(smfFileName.c_str());
		}
		return err;
	}

	// �ŏ��֊����߂�
	void Smf::Rewind()
	{
		for (int i=0; i<trackCount; i++){
			tracks[i]->Rewind();
		}
	}


	// �����f�[�^���N���A
	void Smf::ClearData()
	{
		for(int i=0; i<trackCount; i++){
			delete tracks[i];
		}
		delete[] tracks;
		tracks = 0;
		trackCount = 0;
	}

	// �w�b�_�[�ǂݍ���
	GGSERROR Smf::ReadHeader()
	{
		TRACE(L"��Smf::ReadHeader");
		smfStream->Seek(0);
		
		int intData;
		if (smfStream->ReadInt(intData) != GgsError::NoError)	return GgsError::NoSmf;
		TRACE1(L":%x", intData);
		if (intData != 0x4d546864) return GgsError::NoSmf;	//'MThd'���`�F�b�N
		
		if (smfStream->ReadInt(intData) != GgsError::NoError)	return GgsError::BadSmf;
		TRACE1(L":%d", intData);
		if (intData != 6) return GgsError::UnmatchedSmf;	//�w�b�_����6�o�C�g�łȂ��ꍇ�͈����Ȃ��ł�
		
		if (smfStream->ReadUShort(intData) != GgsError::NoError)	return GgsError::BadSmf;
		TRACE1(L":%d", intData);
		if (intData >= 2) return GgsError::UnmatchedSmf;	//�t�H�[�}�b�g0��1�̂ݑΉ��ł�
			
		if (smfStream->ReadUShort(intData) != GgsError::NoError)	return GgsError::BadSmf;
		TRACE1(L":%d", intData);
		if (intData < 1) return GgsError::BadSmf;
		trackCount = intData;
		
		if (smfStream->ReadUShort(intData) != GgsError::NoError)	return GgsError::BadSmf;
		TRACE1(L":%d", intData);
		if ((intData & 0x8000) != 0) return GgsError::UnmatchedSmf;	//��Ύ��Ԍn�f�[�^�͖��Ή�
		timeBase = intData;
		
		tracks = new SmfTrack*[trackCount];
		for (int i=0; i<trackCount; i++){
			tracks[i] = 0;	// �G���[���o������delete�~�X�΍�
		}

		SmfTrack::InitParam initParam;
		initParam.SmfStream = smfStream;
		

		for (int i=0; i<trackCount; i++){
			SmfTrack* track = new SmfTrack();
			tracks[i] = track;
			if (smfStream->ReadInt(intData) != GgsError::NoError)	return GgsError::BadSmf;
			if (intData != 0x4d54726b) return GgsError::BadSmf;	//'MTrk'���`�F�b�N
			
			if (smfStream->ReadInt(intData) != GgsError::NoError)	return GgsError::BadSmf;
			
			int pos = smfStream->GetPosition();
			int nextPos = pos + intData;
			initParam.BeginPosition = pos;
			initParam.Length = intData;
			track->Initialize(&initParam);
		
			if (i == (trackCount-1)){
				// �Ō�̃g���b�N
				if (nextPos > smfStream->GetLength()) return GgsError::BadSmf;
			}else{
				smfStream->Seek(nextPos);
			}
		}
			
		return GgsError::NoError;
	}


}}


