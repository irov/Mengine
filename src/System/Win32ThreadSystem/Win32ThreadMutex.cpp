#include "Win32ThreadMutex.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadMutex::Win32ThreadMutex()
#ifndef NDEBUG
        : m_file( nullptr )
        , m_line( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::initialize( const Char * _file, uint32_t _line )
    {
        (void)_file;
        (void)_line;

#ifndef NDEBUG
        m_file = _file;
        m_line = _line;
#endif

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
    bool Win32ThreadMutex::try_lock()
    {
        if( TryEnterCriticalSection( &m_cs ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::_destroy()
    {
        DeleteCriticalSection( &m_cs );
    }
}