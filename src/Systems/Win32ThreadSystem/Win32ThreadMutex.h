#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Document.h"

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
        bool initialize( const DocumentPtr & _doc );

    protected:
        void lock() override;
        void unlock() override;

    protected:
        bool try_lock() override;

    protected:
        void _destroy() override;

    protected:
        CRITICAL_SECTION m_cs;

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadMutex, ThreadMutexInterface> Win32ThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}