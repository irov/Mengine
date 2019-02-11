#pragma once

#include "Config/Atomic.h"

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ThreadTask.h"

#include "Config/String.h"
#include "Config/Char.h"

#ifndef MENGINE_THREAD_JOB_WORK_COUNT
#define MENGINE_THREAD_JOB_WORK_COUNT 32
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadWorkerInterface
        : public Mixin
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
        bool initialize( uint32_t _sleep, const Char * _doc );
        
    public:
        uint32_t addWorker( const ThreadWorkerInterfacePtr &_worker );
        bool removeWorker( uint32_t _id );
        bool pauseWorker( uint32_t _id );
        bool resumeWorker( uint32_t _id );

    protected:
        bool _onMain() override;
        void _onUpdate() override;
        void _onCancel() override;
        void _onComplete( bool _successful ) override;

    protected:
        bool check_remove( uint32_t _id );

    protected:
        uint32_t m_sleep;        

        ThreadJobWorkerDesc m_workers[MENGINE_THREAD_JOB_WORK_COUNT];

#ifndef NDEBUG
        String m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadJob> ThreadJobPtr;
    //////////////////////////////////////////////////////////////////////////
}
