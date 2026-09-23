#include "ThreadJob.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ThreadMutexHelper.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void threadWorkerFree( ThreadJobWorkerDesc & _desc )
        {
            _desc.worker = nullptr;
            _desc.id = INVALID_UNIQUE_ID;
            _desc.status = ETS_FREE;
            _desc.pause = false;
            _desc.process = false;
            _desc.remove = false;

#if defined(MENGINE_DOCUMENT_ENABLE)
            _desc.doc = nullptr;
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static bool threadWorkerMatch( const ThreadJobWorkerDesc & _desc, UniqueId _id )
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

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool threadWorkerAdd( ThreadJobWorkerDesc & _desc, const ThreadWorkerInterfacePtr & _worker, UniqueId _id, const DocumentInterfacePtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            if( _desc.status != ETS_FREE )
            {
                return false;
            }

            _desc.mutex->lock();

            if( _desc.status != ETS_FREE )
            {
                _desc.mutex->unlock();

                return false;
            }

            _desc.worker = _worker;
            _desc.id = _id;
            _desc.status = ETS_WORK;
            _desc.pause = false;
            _desc.process = false;
            _desc.remove = false;

#if defined(MENGINE_DOCUMENT_ENABLE)
            _desc.doc = _doc;
#endif

            _desc.mutex->unlock();

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool threadWorkerRemove( ThreadJobWorkerDesc & _desc, UniqueId _id )
        {
            if( Detail::threadWorkerMatch( _desc, _id ) == false )
            {
                return false;
            }

            ThreadWorkerInterfacePtr worker;

            _desc.mutex->lock();

            if( Detail::threadWorkerMatch( _desc, _id ) == false )
            {
                _desc.mutex->unlock();

                return false;
            }

            bool process = _desc.process;

            if( process == true )
            {
                _desc.remove = true;
            }
            else
            {
                worker = _desc.worker;

                Detail::threadWorkerFree( _desc );
            }

            _desc.mutex->unlock();

            if( process == true )
            {
                _desc.mutex_progress->lock();
                _desc.mutex_progress->unlock();
            }

            if( worker != nullptr )
            {
                worker->onThreadWorkerDone( _id );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void threadWorkerProcess( ThreadJobWorkerDesc & _desc )
        {
            if( _desc.status != ETS_WORK || _desc.pause == true || _desc.remove == true )
            {
                return;
            }

            _desc.mutex_progress->lock();
            _desc.mutex->lock();

            if( _desc.status != ETS_WORK || _desc.pause == true || _desc.remove == true )
            {
                _desc.mutex->unlock();
                _desc.mutex_progress->unlock();

                return;
            }

            ThreadWorkerInterfacePtr worker = _desc.worker;
            UniqueId id = _desc.id;

            _desc.process = true;

            _desc.mutex->unlock();

            bool work = worker->onThreadWorkerWork( id );

            _desc.mutex->lock();

            _desc.process = false;

            if( work == false )
            {
                _desc.status = ETS_DONE;
            }

            _desc.mutex->unlock();
            _desc.mutex_progress->unlock();
        }
        //////////////////////////////////////////////////////////////////////////
        static void threadWorkerUpdate( ThreadJobWorkerDesc & _desc )
        {
            if( _desc.status == ETS_FREE )
            {
                return;
            }

            _desc.mutex->lock();

            if( _desc.status == ETS_FREE )
            {
                _desc.mutex->unlock();

                return;
            }

            bool done = _desc.status == ETS_DONE || _desc.remove == true;

            if( done == true )
            {
                if( _desc.process == true )
                {
                    _desc.mutex->unlock();

                    return;
                }
            }
            else if( _desc.pause == true )
            {
                _desc.mutex->unlock();

                return;
            }

            ThreadWorkerInterfacePtr worker = _desc.worker;
            UniqueId id = _desc.id;

            if( done == true )
            {
                Detail::threadWorkerFree( _desc );
            }

            _desc.mutex->unlock();

            if( done == true )
            {
                worker->onThreadWorkerDone( id );

                return;
            }

            worker->onThreadWorkerUpdate( id );
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
    bool ThreadJob::initialize( uint64_t _sleep )
    {
        m_sleep = _sleep;

        for( ThreadJobWorkerDesc & desc : m_workers )
        {
            ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

            desc.mutex = mutex;

            ThreadMutexInterfacePtr mutex_progress = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( mutex_progress, "invalid create mutex" );

            desc.mutex_progress = mutex_progress;

            Detail::threadWorkerFree( desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::finalize()
    {
        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            MENGINE_ASSERTION_FATAL( desc.status == ETS_FREE, "job worker not free" );

            desc.mutex = nullptr;
            desc.mutex_progress = nullptr;

            desc.worker = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId ThreadJob::addWorker( const ThreadWorkerInterfacePtr & _worker, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _worker, "worker is nullptr" );

        if( this->isCancel() == true )
        {
            return INVALID_UNIQUE_ID;
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

        LOGGER_ASSERTION( "overworkers more [%u]"
            , MENGINE_THREAD_JOB_WORK_COUNT
        );

        return INVALID_UNIQUE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::removeWorker( UniqueId _id )
    {
        if( _id == INVALID_UNIQUE_ID )
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
    bool ThreadJob::pauseWorker( UniqueId _id )
    {
        if( _id == INVALID_UNIQUE_ID )
        {
            return false;
        }

        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            if( Detail::threadWorkerMatch( desc, _id ) == false )
            {
                continue;
            }

            desc.mutex->lock();

            if( Detail::threadWorkerMatch( desc, _id ) == false )
            {
                desc.mutex->unlock();

                continue;
            }

            desc.pause = true;

            desc.mutex->unlock();

            desc.mutex_progress->lock();
            desc.mutex_progress->unlock();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::resumeWorker( UniqueId _id )
    {
        if( _id == INVALID_UNIQUE_ID )
        {
            return false;
        }

        for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
        {
            ThreadJobWorkerDesc & desc = m_workers[i];

            if( Detail::threadWorkerMatch( desc, _id ) == false )
            {
                continue;
            }

            desc.mutex->lock();

            if( Detail::threadWorkerMatch( desc, _id ) == false )
            {
                desc.mutex->unlock();

                continue;
            }

            desc.pause = false;

            desc.mutex->unlock();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::_onThreadTaskProcess()
    {
        for( ;; )
        {
            bool cancel = this->isCancel();
            
            if( cancel == true )
            {
                break;
            }
            
            for( uint32_t i = 0; i != MENGINE_THREAD_JOB_WORK_COUNT; ++i )
            {
                ThreadJobWorkerDesc & desc = m_workers[i];

                Detail::threadWorkerProcess( desc );
            }

            const uint32_t sleep_partition = 100;

            if( m_sleep <= sleep_partition )
            {
                THREAD_SYSTEM()
                    ->sleep( m_sleep );
            }
            else
            {
                for( uint64_t sleep = 0; sleep <= m_sleep; sleep += sleep_partition )
                {
                    if( this->isCancel() == true )
                    {
                        break;
                    }

                    THREAD_SYSTEM()
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
