#	include "ThreadJob.h"

#   include <algorithm>

namespace Menge 
{
    //////////////////////////////////////////////////////////////////////////
    ThreadJob::ThreadJob()
        : m_serviceProvider(nullptr)
        , m_sleep(0)
        , m_mutexAdd(nullptr)
        , m_mutexRemove(nullptr)
        , m_mutexComplete(nullptr)
        , m_enumerator(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadJob::~ThreadJob()
    {
        if( m_mutexAdd != nullptr )
        {
            m_mutexAdd->destroy();
            m_mutexAdd = nullptr;
        }
        
        if( m_mutexRemove != nullptr )
        {
            m_mutexRemove->destroy();
            m_mutexRemove = nullptr;
        }

        if( m_mutexComplete != nullptr )
        {
            m_mutexComplete->destroy();
            m_mutexComplete = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////        
    void ThreadJob::initialize( ServiceProviderInterface * _serviceProvider, size_t _sleep )
    {
        m_serviceProvider = _serviceProvider;
        m_sleep = _sleep;

        m_mutexAdd = THREAD_SERVICE(m_serviceProvider)
            ->createMutex();

        m_mutexRemove = THREAD_SERVICE(m_serviceProvider)
            ->createMutex();

        m_mutexComplete = THREAD_SERVICE(m_serviceProvider)
            ->createMutex();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ThreadJob::addWorker( ThreadWorkerInterface * _worker )
    {
        WorkerDesc desc;

        desc.worker = _worker;

        desc.id = ++m_enumerator;

        m_mutexAdd->lock();
        m_workersAdd.push_back( desc );
        m_mutexAdd->unlock();

        return desc.id;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::removeWorker( size_t _id )
    {
        m_mutexAdd->lock();
        for( TWorkers::iterator
            it = m_workersAdd.begin(),
            it_end = m_workersAdd.end();
        it != it_end;
        ++it )
        {
            WorkerDesc & desc = *it;

            if( desc.id != _id )
            {
                continue;
            }

            desc.worker->onDone( desc.id );

            m_workersAdd.erase( it );

            break;
        }
        m_mutexAdd->unlock();

        m_mutexRemove->lock();
        m_workersRemove.push_back( _id );
        m_mutexRemove->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::_onMain()
    {
        while( this->isInterrupt() == false )
        {   
            m_mutexRemove->lock();
            for( TWorkers::iterator
                it = m_workers.begin(),
                it_end = m_workers.end();
            it != it_end;)
            {
                const WorkerDesc & desc = *it;

                if( std::find( m_workersRemove.begin(), m_workersRemove.end(), desc.id ) != m_workersRemove.end() )
                {
                    m_mutexComplete->lock();
                    m_workersComplete.push_back( desc );
                    m_mutexComplete->unlock();

                    it = m_workers.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            m_workersRemove.clear();
            m_mutexRemove->unlock();

            m_mutexAdd->lock();
            m_workers.insert( m_workers.end(), m_workersAdd.begin(), m_workersAdd.end() );
            m_workersAdd.clear();
            m_mutexAdd->unlock();
            
            
            for( TWorkers::iterator
                it = m_workers.begin(),
                it_end = m_workers.end();
            it != it_end;)
            {
                WorkerDesc & desc = *it;

                m_mutexRemove->lock();
                if( std::find( m_workersRemove.begin(), m_workersRemove.end(), desc.id ) == m_workersRemove.end() )
                {
                    if( desc.worker->onWork( desc.id ) == false )
                    {
                        m_mutexComplete->lock();
                        m_workersComplete.push_back( desc );
                        m_mutexComplete->unlock();

                        it = m_workers.erase( it );
                    }
                    else
                    {
                        ++it;
                    }
                }
                else
                {
                    ++it;
                }
                m_mutexRemove->unlock();
            }

            THREAD_SERVICE(m_serviceProvider)
                ->sleep( m_sleep );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::_onUpdate()
    {
        m_mutexComplete->lock();

        for( TWorkers::iterator
            it = m_workersComplete.begin(),
            it_end = m_workersComplete.end();
        it != it_end;
        ++it )
        {
            WorkerDesc & desc = *it;
            
            desc.worker->onDone( desc.id );                
        }

        m_workersComplete.clear();

        m_mutexComplete->unlock();
    }
}
