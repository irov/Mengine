#include "Win32TimeSystem.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimeSystem, Mengine::Win32TimeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32TimeSystem::Win32TimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32TimeSystem::~Win32TimeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32TimeSystem::_initializeService()
    {
        ::QueryPerformanceFrequency( &m_frequency );
        ::QueryPerformanceCounter( &m_initialCounter );
        ::GetSystemTimeAsFileTime( &m_initialSystemTime );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32TimeSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp Win32TimeSystem::getSystemTimestamp() const
    {
        LARGE_INTEGER currentCounter;
        ::QueryPerformanceCounter( &currentCounter );

        LONGLONG elapsedTime = (currentCounter.QuadPart - m_initialCounter.QuadPart) * 10000000 / m_frequency.QuadPart;

        ULARGE_INTEGER ui;
        ui.LowPart = m_initialSystemTime.dwLowDateTime;
        ui.HighPart = m_initialSystemTime.dwHighDateTime;

        ui.QuadPart += elapsedTime;

        Timestamp timestamp = (ui.QuadPart / 10000ULL) - 11644473600000ULL;

        return timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
}
