#include "Win32ThreadMutex.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadMutex::Win32ThreadMutex()
#ifdef _DEBUG
        : m_file( nullptr )
        , m_line( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::initialize( const Char * _file, uint32_t _line )
    {
        InitializeCriticalSection( &m_cs );

#ifdef _DEBUG
        m_file = _file;
        m_line = _line;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::lock()
    {
        EnterCriticalSection( &m_cs );
        intrusive_ptr_add_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::unlock()
    {
        if( intrusive_ptr_get_ref( this ) == 1 )
        {
            CRITICAL_SECTION cs = m_cs;
            intrusive_ptr_dec_ref( this );

            LeaveCriticalSection( &cs );
        }
        else
        {
            intrusive_ptr_dec_ref( this );
            LeaveCriticalSection( &m_cs );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadMutex::try_lock()
    {
        if( TryEnterCriticalSection( &m_cs ) == FALSE )
        {
            return false;
        }

        intrusive_ptr_add_ref( this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::_destroy()
    {
        DeleteCriticalSection( &m_cs );
    }
}