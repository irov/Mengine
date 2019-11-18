#pragma once

#include "Interface/ThreadMutexInterface.h"

namespace Mengine
{
    class ThreadMutexScope
    {
    public:
        explicit ThreadMutexScope( const ThreadMutexInterfacePtr & _mutex );
        ~ThreadMutexScope();

    public:
        const ThreadMutexInterfacePtr & getMutex() const;

    protected:
        const ThreadMutexInterfacePtr & m_mutex;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_THREAD_MUTEX_SCOPE_II( Mutex, Line ) Mengine::ThreadMutexScope thread_mutex_scope_ ## Line ( Mutex )
#define MENGINE_THREAD_MUTEX_SCOPE_I( Mutex, Line ) MENGINE_THREAD_MUTEX_SCOPE_II( Mutex, Line )
#define MENGINE_THREAD_MUTEX_SCOPE( Mutex ) MENGINE_THREAD_MUTEX_SCOPE_I( Mutex, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////
