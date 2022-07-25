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
        static void threadWorkerFree( ThreadJobWorkerDesc & _desc )
        {
            _desc.worker = nullptr;
            _desc.id = 0;
            _desc.status = ETS_FREE;
            _desc.pause = false;
            _desc.process = false;
            _desc.remove = false;

#if MENGINE_DOCUMENT_ENABLE
            _desc.doc = nullptr;
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static bool threadWorkerAdd( ThreadJobWorkerDesc & desc, const ThreadWorkerInterfacePtr & _worker, UniqueId _id, const DocumentPtr & _doc )
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

            if( desc.status == ETS_FREE && desc.pause == false )
            {
                desc.worker = _worker;

                desc.id = _id;
                desc.status = ETS_WORK;
                desc.pause = false;
                desc.process = false;
                desc.remove = false;

#if MENGINE_DOCUMENT_ENABLE
                desc.doc = _doc;
#endif

                successful = true;
            }

            desc.mutex->unlock();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool threadWorkerRemove( ThreadJobWorkerDesc & _desc, uint32_t _id )
        {
            if( _desc.status == ETS_FREE )
            {
                return false;
            }

            if( _desc.id != _id )
            {
                return false;
            }

            if( _desc.remove == true )
            {
                return false;
            }

            bool successful = false;

            ThreadWorkerInterfacePtr worker;
            uint32_t id = 0;

            _desc.mutex->lock();

            if( _desc.status != ETS_FREE && _desc.id == _id )
            {
                switch( _desc.status )
                {
                case ETS_WORK:
                    {
                        if( _desc.process == false )
                        {
                            worker = _desc.worker;
                            id = _desc.id;
                            
                            Detail::threadWorkerFree( _desc );

                            successful = true;
                        }
                        else
                        {
                            _desc.remove = true;
                        }
                    }break;
                case ETS_DONE:
                    {
                        worker = _desc.worker;
                        id = _desc.id;

                        Detail::threadWorkerFree( _desc );

                        successful = true;
                    }break;
                default:
                    break;
                }
            }

            _desc.mutex->unlock();

            if( successful == true )
            {
                worker->onThreadWorkerDone( id );
            }

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static void threadWorkerProcess( ThreadJobWorkerDesc & _desc )
        {
            if( _desc.status != ETS_WORK )
            {
                return;
            }

            if( _desc.pause == true )
            {
                return;
            }

            if( _desc.remove == true )
            {
                return;
            }

            _desc.mutex->lock();

            if( _desc.status == ETS_WORK && _desc.pause == false )
            {
                _desc.process = true;
            }

            _desc.mutex->unlock();

            if( _desc.process == false )
            {
                return;
            }

            bool work = true;

            if( _desc.remove == false && _desc.pause == false )
            {
                _desc.mutex_progress->lock();
                work = _desc.worker->onThreadWorkerWork( _desc.id );
                _desc.mutex_progress->unlock();
            }

            _desc.mutex->lock();

            _desc.process = false;

            if( work == false )
            {
                _desc.status = ETS_DONE;
                _desc.pause = false;
                _desc.remove = false;
            }

            _desc.mutex->unlock();
        }
        //////////////////////////////////////////////////////////////////////////
        static void threadWorkerUpdate( ThreadJobWorkerDesc & _desc )
        {
            ThreadWorkerInterfacePtr worker;
            uint32_t id = 0;

            EThreadStatus status = _desc.status;

            switch( status )
            {
            case ETS_WORK:
                {
                    _desc.mutex->lock();

                    worker = _desc.worker;
                    id = _desc.id;

                    _desc.mutex->unlock();
                }break;
            case ETS_DONE:
                {
                    _desc.mutex->lock();

                    worker = _desc.worker;
                    id = _desc.id;

                    Detail::threadWorkerFree( _desc );

                    _desc.mutex->unlock();
                }break;
            default:
                break;
            }

            if( worker == nullptr )
            {
                return;
            }

            switch( status )
            {
            case ETS_WORK:
                {
                    if( _desc.remove == false )
                    {
                        if( _desc.pause == false )
                        {
                            worker->onThreadWorkerUpdate( id );
                        }
                    }
                    else
                    {
                        _desc.mutex->lock();

                        Detail::threadWorkerFree( _desc );

                        _desc.mutex->unlock();

                        worker->onThreadWorkerDone( id );
                    }

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

        m_sleep = _sleep;

#if MENGINE_DOCUMENT_ENABLE
        m_doc = _doc;
#endif

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
            desc.remove = false;
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

#if MENGINE_DOCUMENT_ENABLE
        m_doc = nullptr;
#endif
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

            if( Detail::threadWorkerAdd( desc, _worker, new_id, _doc ) == false )
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

            if( Detail::threadWorkerRemove( desc, _id ) == false )
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
    bool ThreadJob::_onThreadTaskProcess()
    {
        while( this->isCancel() == false )
        {
            for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
            {
                ThreadJobWorkerDesc & desc = m_workers[i];

                Detail::threadWorkerProcess( desc );
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

            Detail::threadWorkerUpdate( desc );
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

            Detail::threadWorkerFree( desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
