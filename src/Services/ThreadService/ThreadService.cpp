#include "ThreadService.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ThreadTask.h"
#include "Kernel/ThreadTaskPacket.h"
#include "Kernel/ThreadMutexDummy.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadService, Mengine::ThreadService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadService::ThreadService()
        : m_threadCount( 0 )
        , m_mainThreadId( ~0U )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadService::~ThreadService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::_initializeService()
    {
        m_factoryThreadQueue = Helper::makeFactoryPool<ThreadQueue, 4>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadJob = Helper::makeFactoryPool<ThreadJob, 16>( MENGINE_DOCUMENT_FACTORABLE );

        m_threadCount = CONFIG_VALUE( "Engine", "ThreadCount", 16U );

        m_mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_mainThreadId = THREAD_SYSTEM()
            ->getCurrentThreadId();

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
                const ThreadProcessorInterfacePtr & threadProcessor = desc.processor;
                threadProcessor->removeTask();
            }

            task->finally();
        }

        m_tasks.clear();

        for( const ThreadQueuePtr & queue : m_threadQueues )
        {
            queue->finalize();
        }

        m_threadQueues.clear();

        for( ThreadProcessorDesc & desc : m_threadProcessors )
        {
            desc.processor->join();
            desc.processor = nullptr;
        }

        m_threadProcessors.clear();

        for( const ThreadIdentityInterfacePtr & threadIdentity : m_threadIdentities )
        {
            threadIdentity->join();
        }

        m_threadIdentities.clear();

        m_mutex = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadQueue );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadJob );

        m_factoryThreadQueue = nullptr;
        m_factoryThreadJob = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadJobPtr ThreadService::createJob( uint32_t _sleep, const DocumentPtr & _doc )
    {
        ThreadJobPtr threadJob = m_factoryThreadJob->createObject( _doc );

        if( threadJob->initialize( _sleep, _doc ) == false )
        {
            LOGGER_ERROR( "invalid create" );

            return nullptr;
        }

        return threadJob;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::createThreadProcessor( const ConstString & _threadName, EThreadPriority _priority, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( this->hasThreadProcessor( _threadName ) == false, "thread '%s' already exist"
            , _threadName.c_str()
        );

        ThreadProcessorInterfacePtr threadProcessor = THREAD_SYSTEM()
            ->createThreadProcessor( _threadName, _priority, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( threadProcessor );

        ThreadProcessorDesc td;
        td.name = _threadName;
        td.processor = threadProcessor;

        m_threadProcessors.emplace_back( td );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::destroyThreadProcessor( const ConstString & _threadName )
    {
        for( VectorThreadProcessorDescs::iterator
            it = m_threadProcessors.begin(),
            it_end = m_threadProcessors.end();
            it != it_end;
            ++it )
        {
            ThreadProcessorDesc & td = *it;

            if( td.name != _threadName )
            {
                continue;
            }

            td.processor->removeTask();
            td.processor->join();

            m_threadProcessors.erase( it );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr ThreadService::createThreadIdentity( const ConstString & _threadName, EThreadPriority _priority, const DocumentPtr & _doc )
    {
        ThreadIdentityInterfacePtr threadIdentity = THREAD_SYSTEM()
            ->createThreadIdentity( _threadName, _priority, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( threadIdentity );

        m_threadIdentities.emplace_back( threadIdentity );

        return threadIdentity;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::destroyThreadIdentity( const ThreadIdentityInterfacePtr & _threadIdentity )
    {
        VectorThreadIdentityDescs::iterator it_found = Algorithm::find( m_threadIdentities.begin(), m_threadIdentities.end(), _threadIdentity );

        if( it_found == m_threadIdentities.end() )
        {
            return false;
        }

        m_threadIdentities.erase( it_found );

        ThreadIdentityInterfacePtr threadIdentity = *it_found;
        threadIdentity->cancel();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::hasThreadProcessor( const ConstString & _threadName ) const
    {
        for( const ThreadProcessorDesc & td : m_threadProcessors )
        {
            if( td.name == _threadName )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( _task, "thread [%s] add null task (doc: %s)"
            , _threadName.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( this->hasThreadProcessor( _threadName ) == false )
        {
            return false;
        }

        ThreadTaskDesc desc;

        desc.task = _task;
        desc.threadName = _threadName;
        desc.processor = nullptr;
        desc.progress = false;
        desc.complete = false;

#ifdef MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_tasks.emplace_back( desc );
        m_mutex->unlock();

        m_mutex->lock();
        ThreadTaskDesc & try_desc = m_tasks.back();
        this->tryFastProcessTask_( try_desc );
        m_mutex->unlock();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::joinTask( const ThreadTaskInterfacePtr & _task )
    {
        _task->cancel();

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

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
                const ThreadProcessorInterfacePtr & threadProcessor = desc.processor;
                threadProcessor->removeTask();
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
        m_mutex->lock();

        for( const ThreadTaskDesc & desc : m_tasks )
        {
            const ThreadTaskInterfacePtr & task = desc.task;

            task->cancel();

            if( desc.progress == true )
            {
                const ThreadProcessorInterfacePtr & threadIdentity = desc.processor;
                threadIdentity->removeTask();
            }

            task->finally();
        }

        m_tasks.clear();

        m_mutex->unlock();

        for( const ThreadQueuePtr & queue : m_threadQueues )
        {
            queue->finalize();
        }

        m_threadQueues.clear();

        for( ThreadProcessorDesc & desc : m_threadProcessors )
        {
            desc.processor->join();
            desc.processor = nullptr;
        }

        m_threadProcessors.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadQueueInterfacePtr ThreadService::createTaskQueue( uint32_t _packetSize, const DocumentPtr & _doc )
    {
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
    ///////////////////////////////////////////////////////////////////////////
    void ThreadService::update()
    {
        m_mutex->lock();

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

                for( ThreadProcessorDesc & desc_thread : m_threadProcessors )
                {
                    if( desc_thread.name != desc_task.threadName )
                    {
                        continue;
                    }

                    if( desc_thread.processor->processTask( task ) == false )
                    {
                        continue;
                    }

                    desc_task.processor = desc_thread.processor;
                    desc_task.progress = true;
                    break;
                }
            }
        }

        m_mutex->unlock();

        m_tasksAux.clear();

        m_mutex->lock();

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
                m_tasksAux.push_back( task );

                m_tasks[it_task] = m_tasks.back();
                m_tasks.pop_back();
                --it_task_end;
            }
        }

        m_mutex->unlock();

        for( const ThreadTaskInterfacePtr & task : m_tasksAux )
        {
            task->finish();
            task->finally();
        }

        m_tasksAux.clear();

        for( VectorThreadQueues::size_type
            it_task = 0,
            it_task_end = m_threadQueues.size();
            it_task != it_task_end;
            /*++it*/ )
        {
            ThreadQueuePtr queue = m_threadQueues[it_task];

            if( queue->update() == false )
            {
                ++it_task;
            }
            else
            {
                queue->finalize();

                m_threadQueues[it_task] = m_threadQueues.back();
                m_threadQueues.pop_back();
                --it_task_end;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadService::isMainThread() const
    {
        uint64_t id = THREAD_SYSTEM()
            ->getCurrentThreadId();

        if( m_mainThreadId != id )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t ThreadService::getMainThreadId() const
    {
        return m_mainThreadId;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ThreadService::getCurrentThreadName() const
    {
        if( this->isMainThread() == true )
        {
            return STRINGIZE_STRING_LOCAL( "MengineMainThread" );
        }

        for( const ThreadProcessorDesc & desc : m_threadProcessors )
        {
            if( desc.processor->isCurrentThread() == false )
            {
                continue;
            }

            return desc.name;
        }

        return STRINGIZE_STRING_LOCAL( "MengineUnknownThread" );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ThreadService::findThreadNameById( uint64_t _id ) const
    {
        if( m_mainThreadId == _id )
        {
            return STRINGIZE_STRING_LOCAL( "MengineMainThread" );
        }

        for( const ThreadProcessorDesc & desc : m_threadProcessors )
        {
            if( desc.processor->isCurrentThread() == false )
            {
                continue;
            }

            return desc.name;
        }

        return STRINGIZE_STRING_LOCAL( "MengineUnknownThread" );
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadService::tryFastProcessTask_( ThreadTaskDesc & _desc )
    {
        for( ThreadProcessorDesc & desc_thread : m_threadProcessors )
        {
            if( desc_thread.name != _desc.threadName )
            {
                continue;
            }

            ThreadTaskInterface * task_ptr = _desc.task.get();

            if( desc_thread.processor->processTask( task_ptr ) == true )
            {
                _desc.processor = desc_thread.processor;
                _desc.progress = true;
            }

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
