#include "ThreadQueue.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadQueue::ThreadQueue()
        : m_packetSize( 1 )
        , m_cancel( false )
    {
        m_factoryPoolTaskPacket = new FactoryPool<ThreadTaskPacket, 4>();
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadQueue::~ThreadQueue()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolTaskPacket );
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::setThreadName( const ConstString & _threadName )
    {
        m_threadName = _threadName;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::setThreadCount( uint32_t _count )
    {
        m_currentTasks.resize( _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::setPacketSize( uint32_t _size )
    {
        m_packetSize = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::addTask( const ThreadTaskInterfacePtr & _task )
    {
        _task->preparation();

        m_threadTasks.emplace_back( _task );
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::cancel()
    {
        for( ThreadTaskInterfacePtr & currentTask : m_currentTasks )
        {
            if( currentTask == nullptr )
            {
                continue;
            }

            THREAD_SERVICE()
                ->joinTask( currentTask );

            currentTask = nullptr;
        }

        m_currentTasks.clear();

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

        for( ThreadTaskInterfacePtr & currentTask : m_currentTasks )
        {
            this->updateCurrentTask_( currentTask );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadQueue::updateCurrentTask_( ThreadTaskInterfacePtr & _currentTask )
    {
        if( _currentTask == nullptr )
        {
            if( m_threadTasks.empty() == true )
            {
                return;
            }

            ThreadTaskPacketPtr packet = m_factoryPoolTaskPacket->createObject();

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
                if( THREAD_SERVICE()
                    ->addTask( m_threadName, packet ) == false )
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

            if( _currentTask == nullptr )
            {
                return;
            }
        }

        if( _currentTask->isComplete() == true ||
            _currentTask->isCancel() == true )
        {
            _currentTask = nullptr;

            return;
        }

        return;
    }
}
