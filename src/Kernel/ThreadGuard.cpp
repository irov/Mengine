#include "ThreadGuard.h"

#include "Kernel/Win32Helper.h"
#include "Kernel/Crash.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadGuard::ThreadGuard()
        : m_id( 0 )
        , m_lock( false )
    {
        uint32_t id = Helper::Win32GetCurrentThreadId();

        m_id = id;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadGuard::~ThreadGuard()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadGuard::reset()
    {
        m_id = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadGuard::check( const Char * _doc ) const
    {
        if( m_id == 0 )
        {
            return;
        }

        uint32_t id = Helper::Win32GetCurrentThreadId();

        if( m_id == id )
        {
            return;
        }

        Helper::crash( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadGuard::lock( bool _value ) const
    {
        bool lock = m_lock;

        m_lock = _value;

        return lock;
    }
    //////////////////////////////////////////////////////////////////////////
}