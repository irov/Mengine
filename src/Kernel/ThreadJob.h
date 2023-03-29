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
        ETS_FREE,
        ETS_WORK,
        ETS_DONE
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
        AtomicBool process;
        AtomicBool remove;

#if defined(MENGINE_DOCUMENT_ENABLE)
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
        bool removeWorker( UniqueId _id );
        bool pauseWorker( UniqueId _id );
        bool resumeWorker( UniqueId _id );

    protected:
        bool _onThreadTaskProcess() override;
        void _onThreadTaskUpdate() override;
        void _onThreadTaskFinally() override;

    protected:
        uint32_t m_sleep;

        ThreadJobWorkerDesc m_workers[MENGINE_THREAD_JOB_WORK_COUNT];

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadJob, ThreadTaskInterface> ThreadJobPtr;
    //////////////////////////////////////////////////////////////////////////
}
