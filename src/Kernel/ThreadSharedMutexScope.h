#pragma once

#include "Interface/ThreadSharedMutexInterface.h"

#include "Kernel/ThreadMutexScope.h"

namespace Mengine
{
    class ThreadSharedMutexScope
    {
    public:
        explicit ThreadSharedMutexScope( const ThreadSharedMutexInterfacePtr & _mutex );
        ~ThreadSharedMutexScope();

    public:
        const ThreadSharedMutexInterfacePtr & getMutex() const;

    protected:
        const ThreadSharedMutexInterfacePtr & m_mutex;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_THREAD_SHARED_MUTEX_SCOPE_II( Mutex, Index ) Mengine::ThreadSharedMutexScope thread_shared_mutex_scope_##Index ( Mutex )
#define MENGINE_THREAD_SHARED_MUTEX_SCOPE_I( Mutex, Index ) MENGINE_THREAD_SHARED_MUTEX_SCOPE_II( Mutex, Index )
#define MENGINE_THREAD_SHARED_MUTEX_SCOPE( Mutex ) MENGINE_THREAD_SHARED_MUTEX_SCOPE_I( Mutex, MENGINE_CODE_COUNTER )
//////////////////////////////////////////////////////////////////////////
