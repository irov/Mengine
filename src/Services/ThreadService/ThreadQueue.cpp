#include "ThreadQueue.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/ThreadMutexHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadQueue::ThreadQueue()
        : m_packetSize( 1 )
        , m_threadSampler( 0 )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadQueue::~ThreadQueue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadQueue::initialize()
    {
        m_threadTasksMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_currentThreadTasksMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_factoryPoolTaskPacket = Helper::makeFactoryPool<ThreadTaskPacket, 4>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::finalize()
    {
        m_threadTasksMutex = nullptr;
        m_currentThreadTasksMutex = nullptr;

        m_threads.clear();
        m_threadTasks.clear();
        m_currentThreadTasks.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolTaskPacket );

        m_factoryPoolTaskPacket = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::setPacketSize( uint32_t _size )
    {
        m_packetSize = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::addThread( const ConstString & _threadName )
    {
        m_threads.emplace_back( _threadName );

        m_currentThreadTasksMutex->lock();
        m_currentThreadTasks.emplace_back( nullptr );
        m_currentThreadTasksMutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::addTask( const ThreadTaskInterfacePtr & _task )
    {
        if( m_cancel == true )
        {
            return;
        }

        _task->preparation();

        m_threadTasksMutex->lock();
        m_threadTasks.emplace_back( _task );
        m_threadTasksMutex->unlock();

        m_currentThreadTasksMutex->lock();
        for( ThreadTaskInterfacePtr & currentTask : m_currentThreadTasks )
        {
            this->updateCurrentTask_( currentTask );
        }
        m_currentThreadTasksMutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::cancel()
    {
        m_currentThreadTasksMutex->lock();
        for( ThreadTaskInterfacePtr & currentTask : m_currentThreadTasks )
        {
            if( currentTask == nullptr )
            {
                continue;
            }

            THREAD_SERVICE()
                ->joinTask( currentTask );

            currentTask = nullptr;
        }

        m_currentThreadTasks.clear();
        m_currentThreadTasksMutex->unlock();

        m_threadTasksMutex->lock();
        ListThreadTasks threadTasks = std::move( m_threadTasks );
        m_threadTasksMutex->unlock();

        while( threadTasks.empty() == false )
        {
            ThreadTaskInterfacePtr threadTask = threadTasks.front();
            threadTasks.pop_front();

            threadTask->cancel();
        }

        m_cancel = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadQueue::update()
    {
        if( m_cancel == true )
        {
            return true;
        }

        m_currentThreadTasksMutex->lock();
        for( ThreadTaskInterfacePtr & currentTask : m_currentThreadTasks )
        {
            this->updateCurrentTask_( currentTask );
        }
        m_currentThreadTasksMutex->unlock();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::updateCurrentTask_( ThreadTaskInterfacePtr & _currentTask )
    {
        ThreadTaskInterfacePtr testTask = _currentTask;

        if( testTask != nullptr &&
            testTask->isComplete() == false &&
            testTask->isCancel() == false )
        {
            return;
        }

        _currentTask = nullptr;

        m_threadTasksMutex->lock();
        if( m_threadTasks.empty() == true )
        {
            m_threadTasksMutex->unlock();

            return;
        }
        m_threadTasksMutex->unlock();

        ThreadTaskPacketPtr packet = m_factoryPoolTaskPacket->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( packet->initialize( m_packetSize ) == false )
        {
            return;
        }

        uint32_t packetIterator = m_packetSize;

        m_threadTasksMutex->lock();
        while( m_threadTasks.empty() == false && packetIterator != 0 )
        {
            ThreadTaskInterfacePtr threadTask = m_threadTasks.front();
            m_threadTasks.pop_front();

            if( threadTask->isComplete() == true ||
                threadTask->isCancel() == true )
            {
                continue;
            }

            packet->addTask( threadTask );

            --packetIterator;
        }
        m_threadTasksMutex->unlock();

        if( packet->countTask() == 0 )
        {
            return;
        }

        VectorThreads::size_type threadCount = m_threads.size();
        m_threadSampler = (m_threadSampler + 1) % threadCount;

        const ConstString & threadName = m_threads[m_threadSampler];

        if( THREAD_SERVICE()
            ->addTask( threadName, packet, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            uint32_t count = packet->countTask();

            m_threadTasksMutex->lock();
            for( uint32_t i = 0; i != count; ++i )
            {
                const ThreadTaskPtr & task = packet->getTask( i );

                m_threadTasks.emplace_back( task );
            }
            m_threadTasksMutex->unlock();

            return;
        }

        _currentTask = packet;
    }
    //////////////////////////////////////////////////////////////////////////
}
