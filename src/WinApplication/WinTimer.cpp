#	include "WinTimer.h"

#	include <ctime>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	WinTimer::WinTimer()
		: m_timerMask(0)
        , m_startTick(0)
        , m_lastTime(0)
	{
		this->reset();
	}
    //////////////////////////////////////////////////////////////////////////
    void WinTimer::initialize()
    {
        DWORD procMask;
        DWORD sysMask;

#if _MSC_VER >= 1400 && defined (_M_X64)
        GetProcessAffinityMask(GetCurrentProcess(), (PDWORD_PTR)&procMask, (PDWORD_PTR)&sysMask);
#else
        GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);
#endif

        if( procMask == 0 )
        {
            procMask = 1;
        }

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
        
        this->reset();
    }
	//////////////////////////////////////////////////////////////////////////
	void WinTimer::reset()
	{
        m_lastTime = 0;
        ::QueryPerformanceCounter(&m_lastdt);
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
	uint64_t WinTimer::getUnixTime() const
	{
		std::time_t time = std::time(nullptr);

		return (uint64_t)time;
	}
	//////////////////////////////////////////////////////////////////////////
	uint64_t WinTimer::getMilliseconds()
	{
		LARGE_INTEGER curTime;

		HANDLE thread = GetCurrentThread();

		DWORD_PTR oldMask = SetThreadAffinityMask(thread, m_timerMask);

		QueryPerformanceCounter(&curTime);

		SetThreadAffinityMask(thread, oldMask);

		LONGLONG newTime = curTime.QuadPart - m_startTime.QuadPart;

		uint64_t newTicks = (uint64_t) (1000 * newTime / m_frequency.QuadPart);

		uint64_t check = GetTickCount() - m_startTick;
		int64_t msecOff = (int64_t)(newTicks - check);
		if (msecOff < -100 || msecOff > 100)
		{
			LONGLONG adjust = (std::min)(msecOff * m_frequency.QuadPart / 1000, newTime - m_lastTime);
			m_startTime.QuadPart += adjust;
			newTime -= adjust;

			newTicks = (uint64_t) (1000 * newTime / m_frequency.QuadPart);
		}

		m_lastTime = newTime;

		return newTicks;
	}
	//////////////////////////////////////////////////////////////////////////
	uint64_t WinTimer::getMicroseconds()
	{
		LONGLONG newTime = this->getMilliseconds();

		uint64_t newMicro = (uint64_t) (1000000 * newTime / m_frequency.QuadPart);

		return newMicro;
	}
}
