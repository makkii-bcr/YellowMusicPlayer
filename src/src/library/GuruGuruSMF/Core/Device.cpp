
#include "stdafx.h"

#include <dxmus.core/Controller.h>
#include "Device.h"


namespace GuruGuruSmf { namespace Core {
namespace WindowsMultimedia {

	// �R���X�g���N�^
	Device::Device()
	{
		isOpen = false;
	}

	// �f�X�g���N�^
	Device::~Device()
	{
		Close();
	}

	// �f�o�C�X���J��
	GGSERROR Device::Open(int deviceId)
	{
		Close();
		
		if (midiOutOpen(&midiOutHandle, deviceId, 0, 0, 0) != 0)
			return GgsError::CannotOpenDevice;
		isOpen = true;
		return GgsError::NoError;
	}
		
	// �f�o�C�X�����
	void Device::Close()
	{
		if (isOpen){
			midiOutClose(midiOutHandle);
			isOpen = false;
		}
	}

	// ����MIDI���b�Z�[�W�𑗐M
	void Device::SendLongMessage(const byte* data, int length)
	{
		if (!isOpen) return;
		
		MIDIHDR midiHeader;
		int headerSize = sizeof(MIDIHDR);
		ZeroMemory(&midiHeader, headerSize);

		midiHeader.lpData = (LPSTR)data;
		midiHeader.dwBufferLength = length;
		midiHeader.dwFlags = 0;
		midiOutPrepareHeader(midiOutHandle, &midiHeader, headerSize);

		while ((midiHeader.dwFlags & MHDR_PREPARED) != MHDR_PREPARED){
			Sleep(0);
		}

		// MHDR_DONE�������Ȃ����Ƃ����邽��3�񂾂��҂�
		if(midiOutLongMsg(midiOutHandle, &midiHeader, headerSize) == MMSYSERR_NOERROR){
			if ((midiHeader.dwFlags & MHDR_DONE) != MHDR_DONE){
				Sleep(1);
				if ((midiHeader.dwFlags & MHDR_DONE) != MHDR_DONE){
					Sleep(5);
					if ((midiHeader.dwFlags & MHDR_DONE) != MHDR_DONE){
						Sleep(5);
					}
				}
			}
		}
		
		midiOutUnprepareHeader(midiOutHandle, &midiHeader, headerSize);
		GGS_LOGGING_LONGMSG(GGS_LOGGING_GETTIME(), length);
	}
		
		
	// �Z��MIDI���b�Z�[�W�𑗐M
	void Device::SendShortMessage(int status, int data1, int data2)
	{
		ShortMessage msg;
		msg.Status = (byte)status;
		msg.Data1 = (byte)data1;
		msg.Data2 = (byte)data2;
		msg.Reserve = 0;
		midiOutShortMsg(midiOutHandle, msg.Value);
		GGS_LOGGING_SHORTMSG(GGS_LOGGING_GETTIME(), status, data1, data2);
	}

}


namespace DirectMusicPort {


	// �R���X�g���N�^
	Device::Device(IDirectMusicTimer* timer)
	{
		times = timer;
		isOpen = false;
	}

	// �f�X�g���N�^
	Device::~Device()
	{
		Close();
	}

	// �f�o�C�X���J��
	GGSERROR Device::Open(int deviceId, HWND handle)
	{
		Close();
		
		GGSERROR err = GuruGuruSmf::Dxmus::Controller::GetInstance()->PortOpen(handle);
		if (err != 0) return err;
		isOpen = true;
		return GgsError::NoError;
	}
		
	// �f�o�C�X�����
	void Device::Close()
	{
		if (isOpen){
			GuruGuruSmf::Dxmus::Controller::GetInstance()->PortClose();
			isOpen = false;
		}
	}
	

	// ����MIDI���b�Z�[�W�𑗐M
	void Device::SendLongMessage(const byte* data, int length)
	{
		TRACE4(L"/SendLongMessage(%d): %x %x %x ", length, data[0], data[1], data[2]);
		TRACE3(L"%x %x %x\n", data[3], data[4], data[5]);

		__int64 t = times->GetMarkerDmTime();
		GGS_LOGGING_LONGMSG(t, length);
		GuruGuruSmf::Dxmus::Controller::GetInstance()->PortSendLongMessage(t, (LPBYTE)data, length);
		times->IncrementMarkerDmTime();
	}
	
	
	// �Z��MIDI���b�Z�[�W�𑗐M
	void Device::SendShortMessage(int status, int data1, int data2)
	{
		ShortMessage msg;
		msg.Status = (byte)status;
		msg.Data1 = (byte)data1;
		msg.Data2 = (byte)data2;
		msg.Reserve = 0;

		__int64 t = times->GetMarkerDmTime();
		GGS_LOGGING_SHORTMSG(t, status, data1, data2);
		GuruGuruSmf::Dxmus::Controller::GetInstance()->PortSendShortMessage(t, msg.Value);
		times->IncrementMarkerDmTime();
	}
	

}

}}
