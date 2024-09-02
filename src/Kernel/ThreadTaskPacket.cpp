#include "ThreadTaskPacket.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPacket::ThreadTaskPacket()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPacket::~ThreadTaskPacket()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPacket::initialize( uint32_t _packetSize )
    {
        m_tasks.reserve( _packetSize );

        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_childMutex = mutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPacket::addTask( const ThreadTaskPtr & _task )
    {
        m_tasks.emplace_back( _task );
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadTaskPtr & ThreadTaskPacket::getTask( uint32_t _index ) const
    {
        return m_tasks[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ThreadTaskPacket::countTask() const
    {
        uint32_t count = (uint32_t)m_tasks.size();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPacket::_onThreadTaskProcess()
    {
        for( const ThreadTaskPtr & task : m_tasks )
        {
            m_childMutex->lock();
            task->main();
            m_childMutex->unlock();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPacket::_onThreadTaskRun()
    {
        for( const ThreadTaskPtr & task : m_tasks )
        {
            task->run( m_childMutex );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPacket::_onThreadTaskCancel()
    {
        for( const ThreadTaskPtr & task : m_tasks )
        {
            task->cancel();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPacket::_onThreadTaskUpdate()
    {
        for( const ThreadTaskPtr & task : m_tasks )
        {
            if( task->isCancel() == true ||
                task->isComplete() == true )
            {
                continue;
            }

            task->update();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPacket::_onThreadTaskJoin()
    {
        for( const ThreadTaskPtr & task : m_tasks )
        {
            task->join();
        }

        m_childMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPacket::_onThreadTaskFinish()
    {
        for( const ThreadTaskPtr & task : m_tasks )
        {
            task->finish();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPacket::_onThreadTaskComplete( bool _successful )
    {
        MENGINE_UNUSED( _successful );

        m_childMutex = nullptr;

        m_tasks.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}