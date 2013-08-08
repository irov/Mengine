#	include "ThreadJob.h"

#   include <algorithm>

namespace Menge 
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct FWorkerFind
        {
            FWorkerFind( ThreadWorkerInterface * _worker )
                : m_worker(_worker)
            {
            }

            bool operator()( const ThreadJob::WorkerDesc & _desc ) const
            {
                return _desc.worker == m_worker;
            }

            ThreadWorkerInterface * m_worker;
        };
        //////////////////////////////////////////////////////////////////////////
        struct FWorkerDead
        {
            bool operator ()( const ThreadJob::WorkerDesc & _desc ) const
            {
                if( _desc.dead == false )
                {
                    return false;
                }

                return true;
            }
        };	
    }	    
    //////////////////////////////////////////////////////////////////////////
    ThreadJob::ThreadJob()
        : m_serviceProvider(nullptr)
        , m_sleep(0)
        , m_mutex(nullptr)
        , m_mutexAdd(nullptr)
        , m_enumerator(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadJob::~ThreadJob()
    {
        if( m_mutex != nullptr )
        {
            m_mutex->destroy();
            m_mutex = nullptr;
        }

        if( m_mutexAdd != nullptr )
        {
            m_mutexAdd->destroy();
            m_mutexAdd = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////        
    void ThreadJob::initialize( ServiceProviderInterface * _serviceProvider, size_t _sleep )
    {
        m_serviceProvider = _serviceProvider;
        m_sleep = _sleep;

        m_mutex = THREAD_SERVICE(m_serviceProvider)
            ->createMutex();

        m_mutexAdd = THREAD_SERVICE(m_serviceProvider)
            ->createMutex();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ThreadJob::addWorker( ThreadWorkerInterface * _worker )
    {
        WorkerDesc desc;

        desc.worker = _worker;

        desc.id = ++m_enumerator;

        desc.stop = false;
        desc.dead = false;

        m_mutexAdd->lock();
        m_workersAdd.push_back( desc );
        m_mutexAdd->unlock();

        return desc.id;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::removeWorker( size_t _id )
    {
        m_mutex->lock();        
        for( TVectorWorkers::iterator
            it = m_workers.begin(),
            it_end = m_workers.end();
        it != it_end;
        ++it )
        {
            WorkerDesc & desc = *it;

            if( desc.id != _id )
            {
                continue;
            }

            desc.stop = true;
            break;
        }
        m_mutex->unlock();
        
        m_mutexAdd->lock();
        for( TVectorWorkers::iterator
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

            desc.stop = true;
            break;
        }
        m_mutexAdd->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::_onMain()
    {
        while( this->isInterrupt() == false )
        {            
            m_mutexAdd->lock();

            m_mutex->lock();
            m_workers.insert( m_workers.end(), m_workersAdd.begin(), m_workersAdd.end() );
            m_mutex->unlock();

            m_workersAdd.clear();
            m_mutexAdd->unlock();            

            for( TVectorWorkers::size_type
                it = 0,
                it_end = m_workers.size();
            it != it_end;
            ++it )
            {
                m_mutex->lock();
                WorkerDesc & desc = m_workers[it];

                if( desc.stop == true )
                {
                    continue;
                }
                
                if( desc.worker->onWork() == false )
                {
                    desc.stop = true;
                }
                m_mutex->unlock();
            }

            m_mutex->lock();
            TVectorWorkers::iterator it_erase = std::remove_if( m_workers.begin(), m_workers.end(), FWorkerDead());
            m_workers.erase( it_erase, m_workers.end() );
            m_mutex->unlock();

            THREAD_SERVICE(m_serviceProvider)
                ->sleep( m_sleep );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::_onUpdate()
    {
        m_mutex->lock();

        for( TVectorWorkers::iterator
            it = m_workers.begin(),
            it_end = m_workers.end();
        it != it_end;
        ++it )
        {
            WorkerDesc & desc = *it;

            if( desc.stop == false || desc.dead == true )
            {
                continue;
            }

            desc.dead = true;

            desc.worker->onDone();                
        }

        m_mutex->unlock();
    }
}
