// MIDI�f�o�C�X

// #pragma once
#ifndef DEVICE_H
#define DEVICE_H

#include <MMSystem.h>
#include "Constants.h"
#include "Interface.h"

namespace GuruGuruSmf { namespace Core {

	/// <summary>
	/// �f�o�C�X�̊��N���X(?)
	/// </summary>
	class DeviceBase
	{
	protected:
		// �Z��MIDI���b�Z�[�W�̐��`�p
		union ShortMessage
		{
			int Value;
			struct{
				byte Status;
				byte Data1;
				byte Data2;
				byte Reserve;
			};
		};
	};
	
namespace WindowsMultimedia {

	/// <summary>
	/// WindowsMultimedia�̃f�o�C�X������
	/// </summary>
	class Device : public DeviceBase, public IDevice
	{
	private:
		bool isOpen;
		HMIDIOUT midiOutHandle;

	public:
		Device();	// �R���X�g���N�^
		~Device();	// �f�X�g���N�^

		// �f�o�C�X���J����Ă��邩�ǂ���
		bool GetIsOpen(){ return isOpen; }

		// �f�o�C�X���J��
		GGSERROR Open(int deviceId);
		
		// �f�o�C�X�����
		void Close();
		
		//
		// IDeveice������
		//
		
		// ����MIDI���b�Z�[�W�𑗐M
		void SendLongMessage(const byte* data, int length);
		
		// �Z��MIDI���b�Z�[�W�𑗐M
		void SendShortMessage(int status, int data1, int data2);
	};


}

namespace DirectMusicPort {

	/// <summary>
	/// DirectMusic Port�̃f�o�C�X������
	/// </summary>
	class Device : public DeviceBase, public IDevice
	{
	private:
		bool isOpen;
		IDirectMusicTimer* times;

	public:
		Device(IDirectMusicTimer* timer);	// �R���X�g���N�^
		~Device();	// �f�X�g���N�^

		// �f�o�C�X���J���Ă��邩�ǂ���
		bool GetIsOpen(){ return isOpen; }
		
		// �f�o�C�X���J��
		GGSERROR Open(int deviceId, HWND handle);
		
		// �f�o�C�X�����
		void Close();
		
		//
		// IDeveice������
		//
		
		// ����MIDI���b�Z�[�W�𑗐M
		void SendLongMessage(const byte* data, int length);
		
		// �Z��MIDI���b�Z�[�W�𑗐M
		void SendShortMessage(int status, int data1, int data2);
		
	};


}

}}


#endif
