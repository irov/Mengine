#pragma once

#include "Config/Atomic.h"

#include "Kernel/ThreadTask.h"

#ifndef MENGINE_THREAD_JOB_WORK_COUNT
#	define MENGINE_THREAD_JOB_WORK_COUNT 32
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadWorkerInterface
        : public ServantInterface
    {
    public:
        virtual bool onWork( uint32_t _id ) = 0;
        virtual void onDone( uint32_t _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadWorkerInterface> ThreadWorkerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    enum EThreadStatus
    {
        ETS_WORK,
        ETS_DONE,
        ETS_FREE
    };
    //////////////////////////////////////////////////////////////////////////
    struct ThreadJobWorkerDesc
    {
        ThreadMutexInterfacePtr mutex;

        ThreadWorkerInterfacePtr worker;

        AtomicUInt32 id;

        AtomicUInt32 status;
    };
    //////////////////////////////////////////////////////////////////////////
    class ThreadJob
        : public ThreadTask
    {
    public:
        ThreadJob();
        ~ThreadJob() override;

    public:
        bool initialize( uint32_t _sleep );

    public:
        uint32_t addWorker( const ThreadWorkerInterfacePtr &_worker );
        void removeWorker( uint32_t _id );

    protected:
        bool _onMain() override;
        void _onUpdate() override;

    protected:
        bool check_remove( uint32_t _id );

    protected:
        uint32_t m_sleep;

        uint32_t m_enumerator;

        ThreadJobWorkerDesc m_workers[MENGINE_THREAD_JOB_WORK_COUNT];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadJob> ThreadJobPtr;
    //////////////////////////////////////////////////////////////////////////
}
