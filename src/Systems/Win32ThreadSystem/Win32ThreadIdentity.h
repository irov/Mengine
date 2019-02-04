#pragma once

#include "Interface/ThreadIdentityInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

#include "Config/Char.h"
#include "Config/Atomic.h"

namespace Mengine
{
    class Win32ThreadIdentity
        : public ThreadIdentityInterface
        , public Factorable
    {
    public:
        Win32ThreadIdentity();
        ~Win32ThreadIdentity() override;

    public:
        bool initialize( int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line );

    public:
        void main();

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        void removeTask() override;

    public:
        void join() override;

    protected:
        HANDLE m_thread;

        CRITICAL_SECTION m_conditionLock;
        CONDITION_VARIABLE m_conditionVariable;

		Atomic<ThreadTaskInterface *> m_task;
        AtomicBool m_exit;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
