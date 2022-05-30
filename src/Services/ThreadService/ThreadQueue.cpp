#include "ThreadQueue.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Kernel/DocumentHelper.h"

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
        m_factoryPoolTaskPacket = Helper::makeFactoryPool<ThreadTaskPacket, 4>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::finalize()
    {
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

        m_currentThreadTasks.emplace_back( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::addTask( const ThreadTaskInterfacePtr & _task )
    {
        if( m_cancel == true )
        {
            return;
        }

        _task->preparation();

        m_threadTasks.emplace_back( _task );

        for( ThreadTaskInterfacePtr & currentTask : m_currentThreadTasks )
        {
            this->updateCurrentTask_( currentTask );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::cancel()
    {
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

        while( m_threadTasks.empty() == false )
        {
            ThreadTaskInterfacePtr threadTask = m_threadTasks.front();
            m_threadTasks.pop_front();

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

        for( ThreadTaskInterfacePtr & currentTask : m_currentThreadTasks )
        {
            this->updateCurrentTask_( currentTask );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::updateCurrentTask_( ThreadTaskInterfacePtr & _currentTask )
    {
        if( _currentTask == nullptr ||
            _currentTask->isComplete() == true ||
            _currentTask->isCancel() == true )
        {
            _currentTask = nullptr;

            if( m_threadTasks.empty() == true )
            {
                return;
            }

            ThreadTaskPacketPtr packet = m_factoryPoolTaskPacket->createObject( MENGINE_DOCUMENT_FACTORABLE );

            if( packet->initialize( m_packetSize ) == false )
            {
                return;
            }

            uint32_t packetIterator = m_packetSize;

            while( m_threadTasks.empty() == false && packetIterator > 0 )
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

            if( packet->countTask() > 0 )
            {
                VectorThreads::size_type threadCount = m_threads.size();
                m_threadSampler = (m_threadSampler + 1) % threadCount;

                const ConstString & threadName = m_threads[m_threadSampler];

                if( THREAD_SERVICE()
                    ->addTask( threadName, packet, MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    uint32_t count = packet->countTask();

                    for( uint32_t i = 0; i != count; ++i )
                    {
                        const ThreadTaskPtr & task = packet->getTask( i );

                        m_threadTasks.emplace_back( task );
                    }

                    return;
                }

                _currentTask = packet;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
