#pragma once

#include "Interface/ThreadIdentityInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

#include "Config/String.h"
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
        bool initialize( const ConstString & _name, int32_t _priority, const Char * _doc );

    public:
        void main();

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        void removeTask() override;

    public:
        void join() override;

    protected:
        ConstString m_name;

        HANDLE m_thread;

        CRITICAL_SECTION m_processLock;

        CRITICAL_SECTION m_conditionLock;
        CONDITION_VARIABLE m_conditionVariable;

        ThreadTaskInterface * m_task;
        AtomicBool m_exit;

#ifdef MENGINE_DEBUG
        String m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
