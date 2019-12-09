#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadWorkerInterface.h"

#include "Kernel/ThreadTask.h"

#include "Config/Atomic.h"
#include "Config/Char.h"

#ifndef MENGINE_THREAD_JOB_WORK_COUNT
#define MENGINE_THREAD_JOB_WORK_COUNT 32
#endif

#ifdef MENGINE_DEBUG
#include "Kernel/DocumentHelper.h"
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
        ThreadWorkerInterfacePtr worker;

        AtomicUInt32 id;
        Atomic<EThreadStatus> status;
    };
    //////////////////////////////////////////////////////////////////////////
    class ThreadJob
        : public ThreadTask
    {
    public:
        ThreadJob();
        ~ThreadJob() override;

    public:
        bool initialize( uint32_t _sleep, const DocumentPtr & _doc );

    public:
        uint32_t addWorker( const ThreadWorkerInterfacePtr & _worker );
        bool removeWorker( uint32_t _id );
        bool pauseWorker( uint32_t _id );
        bool resumeWorker( uint32_t _id );

    protected:
        bool _onMain() override;
        void _onUpdate() override;
        void _onFinally() override;

    protected:
        uint32_t m_sleep;

        ThreadJobWorkerDesc m_workers[MENGINE_THREAD_JOB_WORK_COUNT];

#ifdef MENGINE_DEBUG
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadJob> ThreadJobPtr;
    //////////////////////////////////////////////////////////////////////////
}
