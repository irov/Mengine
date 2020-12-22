#include "ThreadJob.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadJob::ThreadJob()
        : m_sleep( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadJob::~ThreadJob()
    {
    }
    //////////////////////////////////////////////////////////////////////////        
    bool ThreadJob::initialize( uint32_t _sleep, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

#if MENGINE_DOCUMENT_ENABLE
        m_doc = _doc;
#endif

        m_sleep = _sleep;

        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
                ->createMutex( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( mutex );

            desc.mutex = mutex;

            ThreadMutexInterfacePtr mutex_progress = THREAD_SERVICE()
                ->createMutex( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( mutex_progress );

            desc.mutex_progress = mutex_progress;

            desc.worker = nullptr;
            desc.id = 0;
            desc.status = ETS_FREE;
            desc.pause = false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::finalize()
    {
        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            MENGINE_ASSERTION_FATAL( desc.status == ETS_FREE );

            desc.mutex = nullptr;
            desc.mutex_progress = nullptr;

            desc.worker = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_thread_addWorker( ThreadJobWorkerDesc & desc, const ThreadWorkerInterfacePtr & _worker, UniqueId _id, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        if( desc.status != ETS_FREE )
        {
            return false;
        }

        if( desc.pause == true )
        {
            return false;
        }

        
        

        bool successful = false;

        if( desc.status == ETS_FREE )
        {
            desc.mutex->lock();
            desc.mutex_progress->lock();

            desc.worker = _worker;

            desc.id = _id;
            desc.status = ETS_WORK;
            desc.pause = false;

#if MENGINE_DOCUMENT_ENABLE
            desc.doc = _doc;
#endif

            successful = true;

            desc.mutex_progress->unlock();
            desc.mutex->unlock();
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ThreadJob::addWorker( const ThreadWorkerInterfacePtr & _worker, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( _worker );

        if( this->isCancel() == true )
        {
            return 0;
        }

        UniqueId new_id = GENERATE_UNIQUE_IDENTITY();

        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            if( s_thread_addWorker( desc, _worker, new_id, _doc ) == false )
            {
                continue;
            }

            return new_id;
        }

        LOGGER_ERROR( "overworkers more %d"
            , MENGINE_THREAD_JOB_WORK_COUNT
        );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_thread_removeWorker( ThreadJobWorkerDesc & desc, uint32_t _id )
    {
        if( desc.id != _id )
        {
            return false;
        }

        bool successful = false;

        ThreadWorkerInterfacePtr worker;

        if( desc.status != ETS_FREE )
        {
            desc.mutex->lock();
            desc.mutex_progress->lock();

            worker = desc.worker;
            desc.worker = nullptr;

            desc.id = 0;
            desc.status = ETS_FREE;
            desc.pause = false;

#if MENGINE_DOCUMENT_ENABLE
            desc.doc = nullptr;
#endif

            successful = true;

            desc.mutex_progress->unlock();
            desc.mutex->unlock();
        }

        if( successful == true )
        {
            worker->onThreadWorkerDone( _id );
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::removeWorker( uint32_t _id )
    {
        if( _id == 0 )
        {
            return false;
        }

        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            if( s_thread_removeWorker( desc, _id ) == false )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::pauseWorker( uint32_t _id )
    {
        if( _id == 0 )
        {
            return false;
        }

        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            if( desc.id != _id )
            {
                continue;
            }

            desc.mutex_progress->lock();
            desc.pause = true;
            desc.mutex_progress->unlock();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::resumeWorker( uint32_t _id )
    {
        if( _id == 0 )
        {
            return false;
        }

        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            if( desc.id != _id )
            {
                continue;
            }

            desc.mutex_progress->lock();
            desc.pause = false;
            desc.mutex_progress->unlock();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_thread_mainWorker( ThreadJobWorkerDesc & desc )
    {
        if( desc.status != ETS_WORK )
        {
            return;
        }

        if( desc.status == ETS_WORK && desc.pause == false )
        {
            desc.mutex->lock();
            ThreadWorkerInterface * worker = desc.worker.get();
            uint32_t id = desc.id;
            desc.mutex->unlock();

            desc.mutex_progress->lock();
            bool result = worker->onThreadWorkerWork( id );
            desc.mutex_progress->unlock();

            if( result == false )
            {
                desc.status = ETS_DONE;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::_onThreadTaskMain()
    {
        while( this->isCancel() == false )
        {
            for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
            {
                ThreadJobWorkerDesc & desc = m_workers[i];

                s_thread_mainWorker( desc );
            }

            if( m_sleep <= 100 )
            {
                THREAD_SERVICE()
                    ->sleep( m_sleep );
            }
            else
            {
                uint32_t sleep_partition = 100;

                for( uint32_t sleep = 0; sleep <= m_sleep; sleep += sleep_partition )
                {
                    if( this->isCancel() == true )
                    {
                        break;
                    }

                    THREAD_SERVICE()
                        ->sleep( sleep_partition );
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_thread_updateWorker( ThreadJobWorkerDesc & desc )
    {
        if( desc.pause == true )
        {
            return;
        }

        ThreadWorkerInterfacePtr worker;
        uint32_t id = 0;

        EThreadStatus status = desc.status;

        switch( status )
        {
        case ETS_DONE:
            {
                desc.mutex->lock();
                desc.mutex_progress->lock();

                worker = std::move( desc.worker );

                id = desc.id;
                desc.id = 0;

                desc.status = ETS_FREE;
                desc.pause = false;

#if MENGINE_DOCUMENT_ENABLE
                desc.doc = nullptr;
#endif

                desc.mutex_progress->unlock();
                desc.mutex->unlock();
            }break;
        case ETS_WORK:
            {
                desc.mutex->lock();
                worker = desc.worker;

                id = desc.id;
                desc.mutex->unlock();
            }break;
        default:
            break;
        }

        switch( status )
        {
        case ETS_WORK:
            {
                worker->onThreadWorkerUpdate( id );
            }break;
        case ETS_DONE:
            {
                worker->onThreadWorkerDone( id );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::_onThreadTaskUpdate()
    {
        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            s_thread_updateWorker( desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::_onThreadTaskFinally()
    {
        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            desc.mutex = nullptr;
            desc.mutex_progress = nullptr;

            desc.worker = nullptr;

            desc.status = ETS_FREE;
            desc.pause = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
