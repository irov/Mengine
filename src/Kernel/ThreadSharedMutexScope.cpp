#include "ThreadSharedMutexScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadSharedMutexScope::ThreadSharedMutexScope( const ThreadSharedMutexInterfacePtr & _mutex )
        : m_mutex( _mutex )
    {
        if( m_mutex != nullptr )
        {
            m_mutex->lockShared();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadSharedMutexScope::~ThreadSharedMutexScope()
    {
        if( m_mutex != nullptr )
        {
            m_mutex->unlockShared();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadSharedMutexInterfacePtr & ThreadSharedMutexScope::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
}