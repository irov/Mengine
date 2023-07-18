#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ThreadMutex
        : public ThreadMutexInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32ThreadMutex );

    public:
        Win32ThreadMutex();
        ~Win32ThreadMutex() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void lock() override;
        void unlock() override;
        bool tryLock() override;

    protected:
        void _destroy() override;

    protected:
        CRITICAL_SECTION m_cs;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadMutex, ThreadMutexInterface> Win32ThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}