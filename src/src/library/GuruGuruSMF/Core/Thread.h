// �X���b�h(���̂Ƃ���DirectMusic Port���[�h�̂ݎg�p)

// #pragma once
#ifndef THREAD_H
#define THREAD_H

#include <Windows.h>
#include "Constants.h"

namespace GuruGuruSmf { namespace Core {

	/// <summary>
	/// �X���b�h�N���X
	/// </summary>
	class Thread
	{
	private:
		HANDLE handle;


	public:
		Thread();	// �R���X�g���N�^
		~Thread();	// �f�X�g���N�^

		// �X���b�h�J�n
		GGSERROR Start(void* object, LPTHREAD_START_ROUTINE procedure, int priority);

		// �I���҂�
		void Join();
	};

}}

#endif
