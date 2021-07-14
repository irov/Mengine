#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadWorkerInterface.h"

#include "Kernel/ThreadTask.h"
#include "Kernel/Document.h"

#include "Config/Atomic.h"
#include "Config/Char.h"
#include "Config/UniqueId.h"

#ifndef MENGINE_THREAD_JOB_WORK_COUNT
#define MENGINE_THREAD_JOB_WORK_COUNT 32
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EThreadStatus
    {
        ETS_WORK,
        ETS_PAUSE,
        ETS_DONE,
        ETS_FREE
    };
    //////////////////////////////////////////////////////////////////////////
    struct ThreadJobWorkerDesc
    {
        ThreadMutexInterfacePtr mutex;
        ThreadMutexInterfacePtr mutex_progress;

        ThreadWorkerInterfacePtr worker;

        AtomicUInt32 id;
        Atomic<EThreadStatus> status;
        AtomicBool pause;

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    class ThreadJob
        : public ThreadTask
    {
        DECLARE_FACTORABLE( ThreadJob );

    public:
        ThreadJob();
        ~ThreadJob() override;

    public:
        bool initialize( uint32_t _sleep, const DocumentPtr & _doc );
        void finalize();

    public:
        UniqueId addWorker( const ThreadWorkerInterfacePtr & _worker, const DocumentPtr & _doc );
        bool removeWorker( uint32_t _id );
        bool pauseWorker( uint32_t _id );
        bool resumeWorker( uint32_t _id );

    protected:
        bool _onThreadTaskMain() override;
        void _onThreadTaskUpdate() override;
        void _onThreadTaskFinally() override;

    protected:
        uint32_t m_sleep;

        ThreadJobWorkerDesc m_workers[MENGINE_THREAD_JOB_WORK_COUNT];

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadJob, ThreadTaskInterface> ThreadJobPtr;
    //////////////////////////////////////////////////////////////////////////
}
