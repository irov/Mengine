#	include "Win32TimerSystem.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimerSystem, Menge::Win32TimerSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32TimerSystem::Win32TimerSystem()
		: m_processAffinityMask( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32TimerSystem::~Win32TimerSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
	bool Win32TimerSystem::_initialize()
    {
		DWORD_PTR processAffinityMask;
		DWORD_PTR systemAffinityMask;

		HANDLE process = GetCurrentProcess();

		GetProcessAffinityMask( process, &processAffinityMask, &systemAffinityMask );

        HANDLE thread = GetCurrentThread();

		DWORD_PTR oldMask = SetThreadAffinityMask( thread, processAffinityMask );

        QueryPerformanceFrequency(&m_frequency);
        QueryPerformanceCounter(&m_startTime);

		SetThreadAffinityMask( thread, oldMask );

		m_processAffinityMask = processAffinityMask;

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Win32TimerSystem::_finalize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	uint64_t Win32TimerSystem::getMilliseconds()
	{
		LARGE_INTEGER curTime;

		HANDLE thread = GetCurrentThread();

		DWORD_PTR oldMask = SetThreadAffinityMask( thread, m_processAffinityMask );

		QueryPerformanceCounter(&curTime);

		SetThreadAffinityMask( thread, oldMask );

		LONGLONG newTime = curTime.QuadPart - m_startTime.QuadPart;

		uint64_t newTicks = (uint64_t) (1000 * newTime / m_frequency.QuadPart);

		return newTicks;
	}
}
