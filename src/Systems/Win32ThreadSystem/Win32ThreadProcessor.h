#pragma once

#include "Interface/ThreadProcessorInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

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
        bool initialize( const ThreadDescription & _description, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex );
        void finalize();

    public:
        void main();

    public:
        EThreadPriority getPriority() const override;
        const ThreadDescription & getDescription() const override;

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
        ThreadDescription m_description;
        EThreadPriority m_priority;

        ThreadMutexInterfacePtr m_mutex;

        HANDLE m_thread;

        DWORD m_threadId;

        CRITICAL_SECTION m_processLock;
        CRITICAL_SECTION m_taskLock;

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        CRITICAL_SECTION m_conditionLock;
        CONDITION_VARIABLE m_conditionVariable;
#endif

        ThreadTaskInterface * m_task;
        AtomicBool m_exit;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadProcessor, ThreadProcessorInterface> Win32ThreadProcessorPtr;
    //////////////////////////////////////////////////////////////////////////
}
