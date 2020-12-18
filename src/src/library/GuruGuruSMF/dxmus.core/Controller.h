
// #pragma once
#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "PerformancePlayer.h"
#include "Port.h"

namespace GuruGuruSmf{ namespace Dxmus
{

	class Controller
	{
	private:
		static Controller* instance;
		PerformancePlayer* performancePlayer;
		Port* port;
		Controller();
		Controller(const Controller&){}
		~Controller();

	public:
		static Controller* GetInstance();
		static void FreeInstance();
		GGSERROR PerformanceOpen(HWND handle, int channels)
		{
			return performancePlayer->Open(handle, channels);
		}
		void PerformanceClose()
		{
			performancePlayer->Close();
		}
		GGSERROR PerformancePlay(PerformancePlaySmfData* smf, int option, int seek)
		{
			return  performancePlayer->Play(smf, option, seek);
		}
		void PerformanceStop()
		{
			performancePlayer->Stop();
		}
		void PerformancePause()
		{
			performancePlayer->Pause();
		}
		void PerformanceRestart()
		{
			performancePlayer->Restart();
		}
		void PerformanceSetMasterVolume(long volume)
		{
			performancePlayer->SetMasterVolume(volume);
		}
		void PerformanceSetMasterTempo(float tempo)
		{
			performancePlayer->SetMasterTempo(tempo);
		}
		void PerformanceGetStatus(PlayerStatus* status)
		{
			performancePlayer->GetStatus(status);
		}
		GGSERROR PortOpen(HWND handle)
		{
			return port->Open(handle);
		}
		void PortClose()
		{
			port->Close();
		}
		GGSERROR PortGetReferenceTime(__int64* time)
		{
			return port->GetReferenceTime(time);
		}
		GGSERROR PortSendShortMessage(__int64 time, int midiMessage)
		{
			return port->SendShortMessage(time, midiMessage);
		}
		GGSERROR PortSendLongMessage(__int64 time, LPBYTE midiMessage, int length)
		{
			return port->SendLongMessage(time, midiMessage, length);
		}
		void PortSetVoiceList(DWORD* list, int count)
		{
			port->SetVoiceList(list, count);
		}
		GGSERROR PortAddDls(wchar_t* fileName)
		{
			return port->AddDls(fileName);
		}

	};


}}

#endif
