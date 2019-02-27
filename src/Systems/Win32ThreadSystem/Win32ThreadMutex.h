#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

#include "Config/String.h"
#include "Config/Char.h"

namespace Mengine
{
    class Win32ThreadMutex
        : public ThreadMutexInterface
        , public Factorable
    {
    public:
        Win32ThreadMutex();
        ~Win32ThreadMutex() override;

    public:
        void initialize( const Char * _doc );

    protected:
        void lock() override;
        void unlock() override;

    protected:
        bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        CRITICAL_SECTION m_cs;

#ifdef MENGINE_DEBUG
        String m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadMutex> Win32ThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}