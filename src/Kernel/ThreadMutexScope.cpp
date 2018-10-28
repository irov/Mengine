#include "ThreadMutexScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexScope::ThreadMutexScope( const ThreadMutexInterfacePtr & _mutex )
        : m_mutex( _mutex )
    {
        m_mutex->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexScope::~ThreadMutexScope()
    {
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & ThreadMutexScope::getMutex() const
    {
        return m_mutex;
    }
}