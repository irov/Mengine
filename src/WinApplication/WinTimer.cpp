#	include "WinTimer.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	WinTimer::WinTimer()
		: m_timerMask()
	{
		this->reset();
	}
	//////////////////////////////////////////////////////////////////////////
	void WinTimer::reset()
	{
		DWORD procMask;
		DWORD sysMask;

	#if _MSC_VER >= 1400 && defined (_M_X64)
		GetProcessAffinityMask(GetCurrentProcess(), (PDWORD_PTR)&procMask, (PDWORD_PTR)&sysMask);
	#else
		GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);
	#endif

		if (procMask == 0)
			procMask = 1;

		if( m_timerMask == 0 )
		{
			m_timerMask = 1;
			while( ( m_timerMask & procMask ) == 0 )
			{
				m_timerMask <<= 1;
			}
		}

		HANDLE thread = GetCurrentThread();

		DWORD oldMask = SetThreadAffinityMask(thread, m_timerMask);

		QueryPerformanceFrequency(&m_frequency);

		QueryPerformanceCounter(&m_startTime);

		m_startTick = GetTickCount();

		SetThreadAffinityMask(thread, oldMask);

		::QueryPerformanceCounter(&m_lastdt);

		m_lastTime = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	float WinTimer::getDeltaTime() const
	{
		LARGE_INTEGER time;
		::QueryPerformanceCounter( &time );
		float deltaTime = static_cast<float>( time.QuadPart - m_lastdt.QuadPart )  / m_frequency.QuadPart * 1000.0f;
		m_lastdt = time;

		return deltaTime;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned long WinTimer::getMilliseconds()
	{
		LARGE_INTEGER curTime;

		HANDLE thread = GetCurrentThread();

		DWORD_PTR oldMask = SetThreadAffinityMask(thread, m_timerMask);

		QueryPerformanceCounter(&curTime);

		SetThreadAffinityMask(thread, oldMask);

		LONGLONG newTime = curTime.QuadPart - m_startTime.QuadPart;

		unsigned long newTicks = (unsigned long) (1000 * newTime / m_frequency.QuadPart);

		unsigned long check = GetTickCount() - m_startTick;
		signed long msecOff = (signed long)(newTicks - check);
		if (msecOff < -100 || msecOff > 100)
		{
			LONGLONG adjust = (std::min)(msecOff * m_frequency.QuadPart / 1000, newTime - m_lastTime);
			m_startTime.QuadPart += adjust;
			newTime -= adjust;

			newTicks = (unsigned long) (1000 * newTime / m_frequency.QuadPart);
		}

		m_lastTime = newTime;

		return newTicks;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned long WinTimer::getMicroseconds()
	{
		LONGLONG newTime = this->getMilliseconds();

		unsigned long newMicro = (unsigned long) (1000000 * newTime / m_frequency.QuadPart);

		return newMicro;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned long WinTimer::getMillisecondsCPU()
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned long WinTimer::getMicrosecondsCPU()
	{
		return 0;
	}
}
