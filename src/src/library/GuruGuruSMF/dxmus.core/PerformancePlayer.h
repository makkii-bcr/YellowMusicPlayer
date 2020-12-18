
// #pragma once
#ifndef PERFORMANCEPLAYER_H
#define PERFORMANCEPLAYER_H

#include <windows.h>
#include <dmusici.h>
#include <Core/Constants.h>

namespace GuruGuruSmf { namespace Dxmus {

	// DirectMusicPerformance�ɂ��v���C���[
	class PerformancePlayer
	{
	private:
		IDirectMusicPerformance8* performance;
		IDirectMusicLoader8* loader;
		IDirectMusicSegment8* segment;
		IDirectMusicSegmentState8* segStat;
		bool isOpen, isPlaying, isPause;

		MUSIC_TIME pausedTime;

		void ReleaseSegment();
		MUSIC_TIME TickToMusicTime(int tick, int timeBase);
		MUSIC_TIME GetNowMusicTime();
	
	public:
		PerformancePlayer();
		~PerformancePlayer();
		GGSERROR Open(HWND handle, int channels);
		void Close();
		void Stop();
		GGSERROR Play(PerformancePlaySmfData* smf, int option, int seek);
		void Pause();
		void Restart();
		void SetMasterVolume(long volume);
		void SetMasterTempo(float tempo);
		void GetStatus(PlayerStatus* status);
		
	};
	

}}

#endif
