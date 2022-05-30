#include "ThreadGuard.h"

#include "Kernel/Win32Helper.h"
#include "Kernel/Crash.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadGuard::ThreadGuard()
        : m_lock( false )
        , m_lockThreadId( 0 )
    {
        uint64_t id = Helper::Win32GetCurrentThreadId();

        m_initThreadId = id;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadGuard::~ThreadGuard()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t ThreadGuard::getLockThreadId() const
    {
        return m_lockThreadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadGuard::reset()
    {
        m_initThreadId = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadGuard::check( const Char * _doc ) const
    {
        if( m_initThreadId == 0 )
        {
            return;
        }

        uint64_t id = Helper::Win32GetCurrentThreadId();

        if( m_initThreadId == id )
        {
            return;
        }

        Helper::crash( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadGuard::lock( bool _value ) const
    {
        bool lock = m_lock;

        //ToDo
        m_lockThreadId = Helper::Win32GetCurrentThreadId();

        m_lock = _value;

        return lock;
    }
    //////////////////////////////////////////////////////////////////////////
}