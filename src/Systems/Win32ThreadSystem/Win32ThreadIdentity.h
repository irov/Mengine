#pragma once

#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Document.h"

#include "Config/Char.h"
#include "Config/Atomic.h"

#ifdef MENGINE_DEBUG
#include "Kernel/DocumentHelper.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ThreadIdentity
        : public ThreadIdentityInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32ThreadIdentity );

    public:
        Win32ThreadIdentity();
        ~Win32ThreadIdentity() override;

    public:
        bool initialize( const ConstString & _name, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex, const DocumentPtr & _doc );
        void finalize();

    public:
        void main();

    public:
        uint64_t getThreadId() const override;

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        void removeTask() override;

    public:
        void join() override;

    public:
        bool isCurrentThread() const override;

    protected:
        ConstString m_name;

        ThreadMutexInterfacePtr m_mutex;

        HANDLE m_thread;

        uint64_t m_threadId;

        CRITICAL_SECTION m_processLock;
        CRITICAL_SECTION m_taskLock;

#ifdef MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA
        CRITICAL_SECTION m_conditionLock;
        CONDITION_VARIABLE m_conditionVariable;
#endif

        ThreadTaskInterface * m_task;
        AtomicBool m_exit;

#ifdef MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadIdentity, ThreadIdentityInterface> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
