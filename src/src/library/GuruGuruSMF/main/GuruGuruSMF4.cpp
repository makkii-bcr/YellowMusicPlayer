// GuruGuruSMF4.cpp : DLL �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include <windows.h>
#include <Core/Ggs4Controller.h>

// #ifdef _DEBUG
// #pragma comment(lib, "Core_d.lib")
// #pragma comment(lib, "dxmus.core_d.lib")
// #else
// #pragma comment(lib, "Core.lib")
// #pragma comment(lib, "dxmus.core.lib")
// #endif

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "lib/dxguid.lib")

#ifdef _MANAGED
#pragma managed(push, off)
#endif


using namespace GuruGuruSmf;
using namespace GuruGuruSmf::Core;



extern "C" {
	// �f�o�C�X���J��
	GGSERROR WINAPI GGS4OpenDevice(int deviceId, HWND handle)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->OpenDevice(deviceId, handle);
	}

	// �f�o�C�X�����
	void WINAPI GGS4CloseDevice()
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->CloseDevice();
	}

	// DLS��ǉ�
	GGSERROR WINAPI GGS4AddDlsW(LPCWSTR fileName)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->AddDlsW(fileName);
	}

	GGSERROR WINAPI GGS4AddDlsA(LPCSTR fileName)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->AddDlsA(fileName);
	}

	// ���t�I
	GGSERROR WINAPI GGS4Play(int option, int musicId, int seek, int fadeInTime, int keyShift)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->Play(option, musicId, seek, fadeInTime, keyShift);
	}
	
	// ���t��~
	void WINAPI GGS4Stop(int fadeOutTime)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->Stop(fadeOutTime);
	}
	
	// �ꎞ��~
	void WINAPI GGS4Pause()
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->Pause();
	}
		
	// �ꎞ��~����
	void WINAPI GGS4Restart()
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->Restart();
	}
	
	// �}�X�^�[�e���|
	void WINAPI GGS4SetMasterTempo(float tempo)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->SetMasterTempo(tempo);
	}
		
	// �}�X�^�[�{�����[��
	void WINAPI GGS4SetMasterVolume(int volume)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->SetMasterVolume(volume);
	}
	
	// �}�X�^�[�s�b�`
	void WINAPI GGS4SetMasterPitch(int pitch)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->SetMasterPitch(pitch);
	}
	
	// ���t�����擾
	void WINAPI GGS4GetPlayerStatus(PlayerStatus* status, int size)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->GetPlayerStatus(status, size);
	}

	// ���X�g�Ƀt�@�C������SMF��ǉ�
	GGSERROR WINAPI GGS4AddListFromFileW(LPCWSTR fileName, int option, int musicId)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->AddListFromFileW(fileName, option, musicId);
	}

	GGSERROR WINAPI GGS4AddListFromFileA(LPCSTR fileName, int option, int musicId)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->AddListFromFileA(fileName, option, musicId);
	}
	
	// ���X�g�Ƀ������[����SMF��ǉ�
	GGSERROR WINAPI GGS4AddListFromMemory(const byte* smfBinary, int length, int option, int musicId)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->AddListFromMemory(smfBinary, length, option, musicId);
	}
	
	// ���X�g���̃A�C�e����1�N���A
	void WINAPI GGS4DeleteListItem(int musicId)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->DeleteListItem(musicId);
	}

	// ���X�g���N���A
	void WINAPI GGS4ClearList()
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		ggs->ClearList();
	}

	// ���X�g���̓o�^ID���
	const SmfListKeys* WINAPI GGS4EnumList()
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->EnumList();
	}

	// SMF�̏����擾
	GGSERROR WINAPI GGS4GetSmfInformation(SmfInformation* info, int size, int musicId)
	{
		Ggs4Controller* ggs = Ggs4Controller::GetInstance();
		return ggs->GetSmfInformation(info, size, musicId);
	}


}







BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Ggs4Controller::FreeInstance();
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

