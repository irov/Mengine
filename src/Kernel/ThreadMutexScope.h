#pragma once

#include "Interface/ThreadMutexInterface.h"

namespace Mengine
{
    class ThreadMutexScope
    {
    public:
        ThreadMutexScope( const ThreadMutexInterfacePtr & _mutex );
        ~ThreadMutexScope();

    public:
        const ThreadMutexInterfacePtr & getMutex() const;

    protected:
        const ThreadMutexInterfacePtr & m_mutex;
    };
}