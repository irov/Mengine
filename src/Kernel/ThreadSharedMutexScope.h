#pragma once

#include "Interface/ThreadSharedMutexInterface.h"

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
#define MENGINE_THREAD_SHARED_MUTEX_SCOPE_II( Mutex, Line ) Mengine::ThreadSharedMutexScope thread_shared_mutex_scope_ ## Line ( Mutex )
#define MENGINE_THREAD_SHARED_MUTEX_SCOPE_I( Mutex, Line ) MENGINE_THREAD_SHARED_MUTEX_SCOPE_II( Mutex, Line )
#define MENGINE_THREAD_SHARED_MUTEX_SCOPE( Mutex ) MENGINE_THREAD_SHARED_MUTEX_SCOPE_I( Mutex, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////
