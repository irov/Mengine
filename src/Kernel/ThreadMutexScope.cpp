#include "ThreadMutexScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexScope::ThreadMutexScope( const ThreadMutexInterfacePtr & _mutex )
        : m_mutex( _mutex )
    {
        if( m_mutex != nullptr )
        {
            m_mutex->lock();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexScope::~ThreadMutexScope()
    {
        if( m_mutex != nullptr )
        {
            m_mutex->unlock();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & ThreadMutexScope::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
}