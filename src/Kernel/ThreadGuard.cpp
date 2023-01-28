#include "ThreadGuard.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Crash.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadGuard::ThreadGuard()
        : m_lock( false )
        , m_lockThreadId( 0 )
    {
        ThreadId id = THREAD_SYSTEM()
            ->getCurrentThreadId();

        m_initThreadId = id;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadGuard::~ThreadGuard()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId ThreadGuard::getLockThreadId() const
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

        ThreadId id = THREAD_SYSTEM()
            ->getCurrentThreadId();

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
        m_lockThreadId = THREAD_SYSTEM()
            ->getCurrentThreadId();

        m_lock = _value;

        return lock;
    }
    //////////////////////////////////////////////////////////////////////////
}