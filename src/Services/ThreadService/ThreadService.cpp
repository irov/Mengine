#include "ThreadService.h"

#include "Interface/ConfigServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ThreadTask.h"
#include "Kernel/ThreadTaskPacket.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "stdex/allocator.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadService, Mengine::ThreadService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadService::ThreadService()
        : m_threadCount( 0 )
        , m_mainThreadId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadService::~ThreadService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_stdex_thread_lock( ThreadMutexInterface * _mutex )
    {
        _mutex->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_stdex_thread_unlock( ThreadMutexInterface * _mutex )
    {
        _mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::_availableService() const
    {
        bool available = CONFIG_VALUE( "Engine", "ThreadServiceAvailable", true );

        if( available == false )
        {
            return false;
        }

        if( SERVICE_EXIST( Mengine::ThreadSystemInterface ) == false )
        {
            return false;
        }

        if( THREAD_SYSTEM()
            ->isAvailableService() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::_initializeService()
    {
        m_factoryThreadMutexDummy = new FactoryPool<ThreadMutexDummy, 16>();
        m_factoryThreadQueue = new FactoryPool<ThreadQueue, 4>();
        m_factoryThreadJob = new FactoryPool<ThreadJob, 16>();

        m_threadCount = CONFIG_VALUE( "Engine", "ThreadCount", 16U );

        m_mutexAllocatorPool = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        stdex_allocator_initialize_threadsafe( m_mutexAllocatorPool.get()
            , (stdex_allocator_thread_lock_t)&s_stdex_thread_lock
            , (stdex_allocator_thread_unlock_t)&s_stdex_thread_unlock
        );

        m_mainThreadId = THREAD_SYSTEM()
            ->getCurrentThreadId();

        m_mutexMainCode = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadService::_finalizeService()
    {
        for( const ThreadTaskDesc & desc : m_tasks )
        {
            const ThreadTaskInterfacePtr & task = desc.task;

            task->cancel();

            if( desc.progress == true )
            {
                const ThreadIdentityInterfacePtr & threadIdentity = desc.identity;
                threadIdentity->removeTask();
            }

            task->finally();
        }

        m_tasks.clear();

        for( const ThreadQueueInterfacePtr & queue : m_threadQueues )
        {
            queue->finalize();
        }

        m_threadQueues.clear();

        m_mainCodes.clear();

        for( ThreadDesc & desc : m_threads )
        {
            desc.identity->join();
            desc.identity = nullptr;
        }

        m_threads.clear();

        stdex_allocator_finalize_threadsafe();

        m_mutexAllocatorPool = nullptr;
        m_mutexMainCode = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadQueue );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadMutexDummy );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadJob );

        m_factoryThreadQueue = nullptr;
        m_factoryThreadMutexDummy = nullptr;
        m_factoryThreadJob = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadJobPtr ThreadService::createJob( uint32_t _sleep, const Char * _doc )
    {
        ThreadJobPtr threadJob = m_factoryThreadJob->createObject( _doc );

        if( threadJob->initialize( _sleep, _doc ) == false )
        {
            LOGGER_ERROR( "invalid create"
            );

            return nullptr;
        }

        return threadJob;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::createThread( const ConstString & _threadName, int32_t _priority, const Char * _doc )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        if( this->hasThread( _threadName ) == true )
        {
            return false;
        }

        ThreadIdentityInterfacePtr identity = THREAD_SYSTEM()
            ->createThread( _threadName, _priority, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, false );

        ThreadDesc td;
        td.name = _threadName;
        td.identity = identity;

        m_threads.emplace_back( td );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::destroyThread( const ConstString & _threadName )
    {
        for( VectorThreadDescs::iterator
            it = m_threads.begin(),
            it_end = m_threads.end();
            it != it_end;
            ++it )
        {
            ThreadDesc & td = *it;

            if( td.name != _threadName )
            {
                continue;
            }

            td.identity->removeTask();
            td.identity->join();

            m_threads.erase( it );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::hasThread( const ConstString & _name ) const
    {
        for( const ThreadDesc & td : m_threads )
        {
            if( td.name == _name )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        if( this->hasThread( _threadName ) == false )
        {
            return false;
        }

        ThreadTaskDesc desc;

        desc.task = _task;
        desc.threadName = _threadName;
        desc.identity = nullptr;
        desc.progress = false;
        desc.complete = false;

        m_tasks.emplace_back( desc );

        ThreadTaskDesc & try_desc = m_tasks.back();
        this->tryFastProcessTask_( try_desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::joinTask( const ThreadTaskInterfacePtr & _task )
    {
        _task->cancel();

        for( VectorThreadTaskDesc::iterator
            it = m_tasks.begin(),
            it_end = m_tasks.end();
            it != it_end;
            ++it )
        {
            const ThreadTaskDesc & desc = *it;

            const ThreadTaskInterfacePtr & task = desc.task;

            if( _task != task )
            {
                continue;
            }

            if( desc.progress == true )
            {
                const ThreadIdentityInterfacePtr & threadIdentity = desc.identity;
                threadIdentity->removeTask();
            }

            task->finally();

            *it = m_tasks.back();
            m_tasks.pop_back();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadService::stopTasks()
    {
        for( const ThreadTaskDesc & desc : m_tasks )
        {
            const ThreadTaskInterfacePtr & task = desc.task;

            task->cancel();

            if( desc.progress == true )
            {
                const ThreadIdentityInterfacePtr & threadIdentity = desc.identity;
                threadIdentity->removeTask();
            }

            task->finally();
        }

        m_tasks.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadQueueInterfacePtr ThreadService::createTaskQueue( uint32_t _packetSize, const Char * _doc )
    {
        if( this->isAvailableService() == false )
        {
            return nullptr;
        }

        ThreadQueuePtr taskQueue = m_factoryThreadQueue->createObject( _doc );

        taskQueue->setPacketSize( _packetSize );

        if( taskQueue->initialize() == false )
        {
            return nullptr;
        }

        m_threadQueues.emplace_back( taskQueue );

        return taskQueue;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadService::cancelTaskQueue( const ThreadQueueInterfacePtr & _queue )
    {
        _queue->cancel();

        for( VectorThreadQueues::iterator
            it = m_threadQueues.begin(),
            it_end = m_threadQueues.end();
            it != it_end;
            ++it )
        {
            ThreadQueueInterfacePtr queue = *it;

            if( queue != _queue )
            {
                continue;
            }

            queue->finalize();

            *it = m_threadQueues.back();
            m_threadQueues.pop_back();

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadService::waitMainThreadCode( const LambdaMainThreadCode & _lambda, const Char * _doc )
    {
        ThreadConditionVariableInterfacePtr conditionVariable = THREAD_SYSTEM()
            ->createConditionVariable( _doc );

        m_mutexMainCode->lock();
        MainCodeDesc desc;
        desc.conditionVariable = conditionVariable;
        desc.lambda = _lambda;
        desc.doc = _doc;
        m_mainCodes.emplace_back( desc );
        m_mutexMainCode->unlock();

        conditionVariable->wait();
    }
    ///////////////////////////////////////////////////////////////////////////
    void ThreadService::update()
    {
        for( ThreadTaskDesc & desc_task : m_tasks )
        {
            if( desc_task.complete == true )
            {
                continue;
            }

            if( desc_task.progress == true )
            {
                if( desc_task.task->isComplete() == true )
                {
                    desc_task.complete = true;
                    desc_task.progress = false;
                }
            }
            else
            {
                ThreadTaskInterface * task = desc_task.task.get();

                for( ThreadDesc & desc_thread : m_threads )
                {
                    if( desc_thread.name != desc_task.threadName )
                    {
                        continue;
                    }

                    if( desc_thread.identity->processTask( task ) == false )
                    {
                        continue;
                    }
                     
                    desc_task.identity = desc_thread.identity;
                    desc_task.progress = true;
                    break;
                }
            }
        }

        for( VectorThreadTaskDesc::size_type
            it_task = 0,
            it_task_end = m_tasks.size();
            it_task != it_task_end;
            /*++it*/ )
        {
            ThreadTaskDesc handle = m_tasks[it_task];

            const ThreadTaskInterfacePtr & task = handle.task;

            if( task->update() == false )
            {
                ++it_task;
            }
            else
            {
                task->finally();

                m_tasks[it_task] = m_tasks.back();
                m_tasks.pop_back();
                --it_task_end;
            }
        }

        for( VectorThreadQueues::size_type
            it_task = 0,
            it_task_end = m_threadQueues.size();
            it_task != it_task_end;
            /*++it*/ )
        {
            ThreadQueuePtr pool = m_threadQueues[it_task];

            if( pool->update() == false )
            {
                ++it_task;
            }
            else
            {
                m_threadQueues[it_task] = m_threadQueues.back();
                m_threadQueues.pop_back();
                --it_task_end;
            }
        }

        if( m_mainCodes.empty() == false )
        {
            VectorMainCodeDescs mainCodes;
            m_mutexMainCode->lock();
            std::swap( m_mainCodes, mainCodes );
            m_mutexMainCode->unlock();

            for( const MainCodeDesc & desc : mainCodes )
            {
                desc.lambda();
                desc.conditionVariable->wake();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr ThreadService::createMutex( const Char * _doc )
    {
        if( this->isAvailableService() == false )
        {
            ThreadMutexDummyPtr mutex_dummy =
                m_factoryThreadMutexDummy->createObject( _doc );

            return mutex_dummy;
        }

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( _doc );

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadService::sleep( uint32_t _ms )
    {
        THREAD_SYSTEM()
            ->sleep( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    ptrdiff_t ThreadService::getCurrentThreadId()
    {
        if( this->isAvailableService() == false )
        {
            return 0U;
        }

        ptrdiff_t id = THREAD_SYSTEM()
            ->getCurrentThreadId();

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::isMainThread() const
    {
        if( this->isAvailableService() == false )
        {
            return true;
        }

        ptrdiff_t id = THREAD_SYSTEM()
            ->getCurrentThreadId();

        if( m_mainThreadId != id )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadService::tryFastProcessTask_( ThreadTaskDesc & _desc )
    {
        for( ThreadDesc & desc_thread : m_threads )
        {
            if( desc_thread.name != _desc.threadName )
            {
                continue;
            }

            ThreadTaskInterface * task_ptr = _desc.task.get();

            if( desc_thread.identity->processTask( task_ptr ) == true )
            {
                _desc.identity = desc_thread.identity;
                _desc.progress = true;
            }

            break;
        }
    }
}
