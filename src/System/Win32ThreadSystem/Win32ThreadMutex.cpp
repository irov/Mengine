#   include "Win32ThreadMutex.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadMutex::Win32ThreadMutex()
        : m_mutex(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::initialize( HANDLE _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadMutex::lock()
    {
        DWORD result = WaitForSingleObject( m_mutex, 0 );

        if( result != WAIT_OBJECT_0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::unlock()
    {
        ReleaseMutex( m_mutex );
    }
}