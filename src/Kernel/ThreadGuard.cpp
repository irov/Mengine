#include "ThreadGuard.h"

#include "Kernel/Crash.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadGuard::ThreadGuard()
        : m_lock( false )
        , m_lockThreadId( std::this_thread::get_id() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadGuard::~ThreadGuard()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    std::thread::id ThreadGuard::getLockThreadId() const
    {
        return m_lockThreadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadGuard::check( const Char * _doc ) const
    {
        std::thread::id id = std::this_thread::get_id();

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
        m_lockThreadId = std::this_thread::get_id();;

        m_lock = _value;

        return lock;
    }
    //////////////////////////////////////////////////////////////////////////
}