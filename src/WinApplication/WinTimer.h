#	pragma once
#	include "Interface/ApplicationInterface.h"
#	include "windows.h"

namespace Menge
{
	class WinTimer 
		: public TimerInterface
	{
	public:
		WinTimer();

	public:
		void reset() override;
		float getDeltaTime() const override;
		unsigned long getMilliseconds() override;
		unsigned long getMicroseconds() override;
		unsigned long getMillisecondsCPU() override;
		unsigned long getMicrosecondsCPU() override;

	private:
		DWORD m_timerMask;
		DWORD m_startTick;
		LONGLONG m_lastTime;
		mutable LARGE_INTEGER m_lastdt;
		LARGE_INTEGER m_startTime;
		LARGE_INTEGER m_frequency;
	};
};
