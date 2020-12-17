
#include "stdafx.h"

#include "Thread.h"

namespace GuruGuruSmf { namespace Core {

	// �R���X�g���N�^
	Thread::Thread()
	{
		handle = 0;
	}

	// �f�X�g���N�^
	Thread::~Thread()
	{
		if (handle == 0) return;

		DWORD param;
		GetExitCodeThread(handle , &param);
		if (param == STILL_ACTIVE) {
			TerminateThread(handle , FALSE);
			WaitForSingleObject(handle, INFINITE);
		}
		CloseHandle(handle);
	}


	// �X���b�h�J�n
	GGSERROR Thread::Start(void* object, LPTHREAD_START_ROUTINE procedure, int priority)
	{
		if(handle) return GgsError::Failed;	// �O�̃X���b�h�I�u�W�F�N�g���܂��c���Ă���

		DWORD id;
		handle = CreateThread(NULL, 0, procedure, object, CREATE_SUSPENDED, &id);
		if(handle == 0) return GgsError::Failed;	// ���s

		SetThreadPriority(handle, priority);
		ResumeThread(handle);
		return GgsError::NoError;
	}

	// �I���҂�
	void Thread::Join()
	{
		if(handle == 0) return;

		DWORD param;
		GetExitCodeThread(handle , &param);
		if(param == STILL_ACTIVE){
			WaitForSingleObject(handle, INFINITE);
		}

		CloseHandle(handle);
		handle = 0;
	}




}}


