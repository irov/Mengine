#   include "Win32ThreadMutex.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadMutex::Win32ThreadMutex()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::initialize( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
        
        InitializeCriticalSection( &m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::lock()
    {
        EnterCriticalSection( &m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::unlock()
    {
        LeaveCriticalSection( &m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadMutex::_destroy()
    {
        DeleteCriticalSection( &m_cs );

        return true;
    }
}