#pragma once

#include "Interface/ThreadSharedMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ThreadSharedMutex
        : public ThreadSharedMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32ThreadSharedMutex );

    public:
        Win32ThreadSharedMutex();
        ~Win32ThreadSharedMutex() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void lock() override;
        void unlock() override;
        bool tryLock() override;

    protected:
        void lockShared() override;
        void unlockShared() override;
        bool tryLockShared() override;

    protected:
        SRWLOCK m_srwLock;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadSharedMutex, ThreadSharedMutexInterface> Win32ThreadSharedMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}