#include "stdafx.h"
#include <dmksctrl.h>
#include "Port.h"


namespace GuruGuruSmf {	namespace Dxmus
{

	// �R���X�g���N�^
	Dls::Dls(IDirectMusicLoader8* ldr)
	{
		loader = ldr;
		collection = 0;
	}

	// �f�X�g���N�^
	Dls::~Dls()
	{
		if (collection){
			collection->Release();
			collection = 0;
		}
	}

	// �t�@�C������ǂݍ���
	GGSERROR Dls::LoadFromFile(wchar_t *fileName)
	{
		HRESULT hr;
		hr = loader->LoadObjectFromFile(CLSID_DirectMusicCollection, IID_IDirectMusicCollection8,
								fileName, (LPVOID*)&collection );
		if(FAILED(hr))	return GgsError::FileLoadFailed;	// �ǂݍ��ݎ��s

		return GgsError::NoError;	// ����
	}

	// ���F���o��
	int Dls::GetInstrument(int voice, IDirectMusicInstrument8** instrument)
	{
		if(collection == 0)	return -1;

		return collection->GetInstrument(voice, instrument);
	}




	// �R���X�g���N�^
	Port::Port()
	{
		music = 0;
		port = 0;
		buffer = 0;
		loader = 0;
		defaultDls = 0;
		clock = 0;

		downloadedList = 0;
		downloadedCount = 0;

		isOpen = false;
	}

	// �f�X�g���N�^
	Port::~Port()
	{
		Close();
	}

	// ������
	GGSERROR Port::Open(HWND handle)
	{
		Close();

		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		isOpen = true;

		HRESULT hr;
		hr = CoCreateInstance(CLSID_DirectMusic ,NULL ,
					CLSCTX_INPROC_SERVER ,IID_IDirectMusic8,
					(LPVOID*)&music);
		if(FAILED(hr))	return GgsError::CannotOpenDevice;
		hr = music->SetDirectSound(NULL ,handle);
		if(FAILED(hr))	return GgsError::CannotOpenDevice;

		//�|�[�g�쐬
		DMUS_PORTPARAMS param;
		ZeroMemory(&param, sizeof(DMUS_PORTPARAMS));
		param.dwSize = sizeof(DMUS_PORTPARAMS);
		param.dwValidParams = DMUS_PORTPARAMS_VOICES
			| DMUS_PORTPARAMS_CHANNELGROUPS
			| DMUS_PORTPARAMS_SAMPLERATE
			| DMUS_PORTPARAMS_EFFECTS;
		param.dwVoices = 128;
		param.dwChannelGroups = 1;
		param.dwSampleRate = 44100;
		param.dwEffectFlags = DMUS_EFFECT_REVERB | DMUS_EFFECT_CHORUS;

		GUID guidPort;
		hr = music->GetDefaultPort(&guidPort);
		if(FAILED(hr))	return GgsError::CannotOpenDevice;
		hr = music->CreatePort(guidPort, &param, &port, NULL);
		if(FAILED(hr))	return GgsError::CannotOpenDevice;
		port->Activate(TRUE);

		// �o�b�t�@�쐬
		DMUS_BUFFERDESC bufferDesc;

		bufferDesc.dwSize = sizeof(DMUS_BUFFERDESC);
		bufferDesc.dwFlags = 0;
		bufferDesc.guidBufferFormat = GUID_NULL;
		bufferDesc.cbBuffer = 128;
		hr = music->CreateMusicBuffer(&bufferDesc, &buffer, NULL);
		if(FAILED(hr))	return GgsError::CannotOpenDevice;

		// �^�C�}�擾
		hr = port->GetLatencyClock(&clock);
		if(FAILED(hr))	return GgsError::CannotOpenDevice;

		// ���[�_�[�쐬
		hr = CoCreateInstance(CLSID_DirectMusicLoader, NULL,
					CLSCTX_INPROC_SERVER, IID_IDirectMusicLoader8,
					(LPVOID*)&loader);
		if(FAILED(hr))	return GgsError::CannotOpenDevice;

		wchar_t path[MAX_PATH];
		if(GetCurrentDirectoryW(MAX_PATH, path) == 0){
			DWORD errCode = GetLastError();
			if(errCode == ERROR_CALL_NOT_IMPLEMENTED){	// Windows9x �n�[�h�R�[�f�B���O�ł�
				char pathA[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, pathA);
				MultiByteToWideChar(CP_ACP, 0, pathA, -1, path, MAX_PATH);
			}
		}

		loader->SetSearchDirectory(GUID_DirectMusicAllTypes, path, FALSE);


		// �f�t�H���g��DLS���_�E�����[�h
		DMUS_OBJECTDESC objectDesc;
		ZeroMemory(&objectDesc, sizeof(DMUS_OBJECTDESC));
		objectDesc.dwSize = sizeof(DMUS_OBJECTDESC);
		objectDesc.guidClass = CLSID_DirectMusicCollection;
		objectDesc.guidObject = GUID_DefaultGMCollection;
		objectDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_OBJECT;
		hr = loader->GetObject(&objectDesc, IID_IDirectMusicCollection8,
					(LPVOID*)&defaultDls);
		if(FAILED(hr))	return GgsError::CannotOpenDevice;


		return GgsError::NoError;
	}

	// ���F���X�g�����Z�b�g
	void Port::ResetVoiceList()
	{
		if(downloadedCount == 0) return;

		for(int i=0; i<downloadedCount; i++){
			if(downloadedList[i]){
				port->UnloadInstrument(downloadedList[i]);
				downloadedList[i]->Release();
			}
		}
		delete[] downloadedList;
		downloadedList = 0;
		downloadedCount = 0;
	}

	// ���F�����o��
	int Port::GetInstrument(int voice, IDirectMusicInstrument8** instrument)
	{
		// �܂��̓f�t�H���g����T��
		defaultDls->GetInstrument(voice, instrument);
		if(*instrument != 0) return 0;

		// ������Ȃ���΃��X�g����T��
		for(int i=0; i<(int)dlsList.size(); i++){
			if(dlsList[i]->GetInstrument(voice, instrument) == 0) return 0;
		}
		return -1;
	}

	// ���F���X�g���Z�b�g
	void Port::SetVoiceList(DWORD* list, int count)
	{
		ResetVoiceList();

		IDirectMusicInstrument8* instrument = 0 ;
		IDirectMusicDownloadedInstrument8* downloaded = 0;

		downloadedList = new IDirectMusicDownloadedInstrument8*[count];
		downloadedCount = count;

		// �L���m�[�g�͑S�͈�
		DMUS_NOTERANGE noteRange;
		noteRange.dwLowNote = 0;
		noteRange.dwHighNote = 127;

		// ���F��ǂݏo����port�ɐݒ�
		for(int i=0; i<count; i++){
			downloadedList[i] = 0;

			int voice = list[i];

			if(GetInstrument(voice, &instrument)){	// ���F���Ȃ�������o���N0
				defaultDls->GetInstrument(voice&0xff0000ff, &instrument);
				if(instrument == 0) continue;	// ����͂���グ
				instrument->SetPatch(voice);
			}

			port->DownloadInstrument(instrument, &downloaded, &noteRange, 1);
			downloadedList[i] = downloaded;
			downloaded = 0;
			instrument->Release();
			instrument = 0;
		}
		port->Compact();
	}



	// ��n��
	void Port::Close()
	{
		ResetVoiceList();

		for(int i=0; i<(int)dlsList.size(); i++){
			delete dlsList[i];
		}
		dlsList.clear();

		if(clock){
			clock->Release();
			clock = 0;
		}

		if(buffer){
			buffer->Release();
			buffer = 0;
		}

		if(defaultDls){
			defaultDls->Release();
			defaultDls = 0;
		}

		if(port){
			port->Activate(FALSE);
			port->Release();
			port = 0;
		}

		if(loader){
			loader->Release();
			loader = 0;
		}

		if(music){
			music->Release();
			music = 0;
		}
			
		if(isOpen){
			CoUninitialize();
			isOpen = false;
		}
	}

	// �x���t�����Ԃ��擾
	GGSERROR Port::GetReferenceTime(long long* time)
	{
		HRESULT hr = clock->GetTime(time);
		if(FAILED(hr)) return GgsError::Failed;
		return GgsError::NoError;
	}

	// �Z�����b�Z�[�W�𑗂�
	GGSERROR Port::SendShortMessage(long long time, int midiMessage)
	{
		if(!isOpen)	return GgsError::NotReady;

		HRESULT hr;
		hr = buffer->PackStructured(time, 0, midiMessage);
		if(FAILED(hr)) return GgsError::Failed;
		hr = port->PlayBuffer(buffer);
		buffer->Flush();
			
		return GgsError::NoError;
	}

	// �������b�Z�[�W�𑗂�
	GGSERROR Port::SendLongMessage(long long time, LPBYTE midiMessage, int length)
	{
		if(!isOpen) return GgsError::NotReady;

		HRESULT hr;
		hr = buffer->PackUnstructured(time, 0, length, midiMessage);
		if(FAILED(hr)) return GgsError::Failed;
		hr = port->PlayBuffer(buffer);
		buffer->Flush();
		if(FAILED(hr)) return GgsError::Failed;

		return GgsError::NoError;
	}

	// DLS��ǉ�
	GGSERROR Port::AddDls(wchar_t* fileName)
	{
		Dls* dls = new Dls(loader);
		GGSERROR err = dls->LoadFromFile(fileName);
		if(err == GgsError::NoError){
			dlsList.push_back(dls);
			return err;
		}else{
			delete dls;
			return err;
		}
	}

}}

