#pragma once

#include "Interface/ThreadProcessorInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Document.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Char.h"
#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ThreadProcessor
        : public ThreadProcessorInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32ThreadProcessor );

    public:
        Win32ThreadProcessor();
        ~Win32ThreadProcessor() override;

    public:
        bool initialize( const ConstString & _name, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex, const DocumentPtr & _doc );
        void finalize();

    public:
        void main();

    public:
        ThreadId getThreadId() const override;

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

        DWORD m_threadId;

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
    typedef IntrusivePtr<Win32ThreadProcessor, ThreadProcessorInterface> Win32ThreadProcessorPtr;
    //////////////////////////////////////////////////////////////////////////
}
