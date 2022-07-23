#include "ThreadJob.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EnumeratorHelper.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool threadAddWorker( ThreadJobWorkerDesc & desc, const ThreadWorkerInterfacePtr & _worker, UniqueId _id, const DocumentPtr & _doc )
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

            desc.mutex->lock();
            desc.mutex_progress->lock();

            if( desc.status == ETS_FREE && desc.pause == false )
            {
                desc.worker = _worker;

                desc.id = _id;
                desc.status = ETS_WORK;
                desc.pause = false;
                desc.process = false;

#if MENGINE_DOCUMENT_ENABLE
                desc.doc = _doc;
#endif

                successful = true;
            }

            desc.mutex_progress->unlock();
            desc.mutex->unlock();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool threadRemoveWorker( ThreadJobWorkerDesc & desc, uint32_t _id )
        {
            if( desc.status == ETS_FREE )
            {
                return false;
            }

            if( desc.id != _id )
            {
                return false;
            }

            bool successful = false;

            ThreadWorkerInterfacePtr worker;

            desc.mutex->lock();

            if( desc.status != ETS_FREE && desc.id == _id && desc.process == false )
            {
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
            }

            desc.mutex->unlock();

            if( successful == true )
            {
                worker->onThreadWorkerDone( _id );
            }

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static void threadMainWorker( ThreadJobWorkerDesc & desc )
        {
            if( desc.status != ETS_WORK )
            {
                return;
            }

            if( desc.pause == true )
            {
                return;
            }

            ThreadWorkerInterface * worker = nullptr;
            uint32_t id = 0;

            desc.mutex->lock();

            if( desc.status == ETS_WORK && desc.pause == false )
            {
                worker = desc.worker.get();
                id = desc.id;
                desc.process = true;
            }

            desc.mutex->unlock();

            if( worker == nullptr )
            {
                return;
            }
            
            desc.mutex_progress->lock();

            if( desc.worker->onThreadWorkerWork( desc.id ) == false )
            {
                desc.status = ETS_DONE;
            }

            desc.mutex_progress->unlock();

            desc.mutex->lock();
            desc.process = false;
            desc.mutex->unlock();
        }
        //////////////////////////////////////////////////////////////////////////
        static void threadUpdateWorker( ThreadJobWorkerDesc & desc )
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
            case ETS_WORK:
                {
                    desc.mutex->lock();

                    if( desc.pause == false )
                    {
                        worker = desc.worker;

                        id = desc.id;
                    }

                    desc.mutex->unlock();
                }break;
            case ETS_DONE:
                {
                    desc.mutex->lock();

                    if( desc.pause == false )
                    {
                        desc.mutex_progress->lock();

                        worker = std::move( desc.worker );

                        id = desc.id;
                        desc.id = 0;

                        desc.status = ETS_FREE;

#if MENGINE_DOCUMENT_ENABLE
                        desc.doc = nullptr;
#endif

                        desc.mutex_progress->unlock();
                    }

                    desc.mutex->unlock();
                }break;
            default:
                break;
            }

            if( worker != nullptr )
            {
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
        }
    }
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
            desc.process = false;
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
    UniqueId ThreadJob::addWorker( const ThreadWorkerInterfacePtr & _worker, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( _worker );

        if( this->isCancel() == true )
        {
            return 0;
        }

        UniqueId new_id = Helper::generateUniqueIdentity();

        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            if( Detail::threadAddWorker( desc, _worker, new_id, _doc ) == false )
            {
                continue;
            }

            return new_id;
        }

        LOGGER_ERROR( "overworkers more [%u]"
            , MENGINE_THREAD_JOB_WORK_COUNT
        );

        return INVALID_UNIQUE_ID;
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

            if( Detail::threadRemoveWorker( desc, _id ) == false )
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
    bool ThreadJob::_onThreadTaskMain()
    {
        while( this->isCancel() == false )
        {
            for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
            {
                ThreadJobWorkerDesc & desc = m_workers[i];

                Detail::threadMainWorker( desc );
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
    void ThreadJob::_onThreadTaskUpdate()
    {
        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            Detail::threadUpdateWorker( desc );
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
