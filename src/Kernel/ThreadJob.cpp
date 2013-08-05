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
        struct FWorkerDone
        {
            bool operator ()( const ThreadJob::WorkerDesc & _desc ) const
            {
                if( _desc.stop == false )
                {
                    return false;
                }

                _desc.worker->onDone();

                return true;
            }
        };	
    }	    
    //////////////////////////////////////////////////////////////////////////
    ThreadJob::ThreadJob()
        : m_serviceProvider(nullptr)
        , m_sleep(0)
        , m_mutex(nullptr)
        , m_updating(false)
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
    }
    //////////////////////////////////////////////////////////////////////////        
    void ThreadJob::initialize( ServiceProviderInterface * _serviceProvider, size_t _sleep )
    {
        m_serviceProvider = _serviceProvider;
        m_sleep = _sleep;

        m_mutex = THREAD_SERVICE(m_serviceProvider)
            ->createMutex();
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::addWorker( ThreadWorkerInterface * _worker )
    {
        WorkerDesc desc;

        desc.worker = _worker;
        desc.stop = false;

        m_mutex->lock();
        m_workersAdd.push_back( desc );
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::removeWorker( ThreadWorkerInterface * _worker )
    {
        m_mutex->lock();
        
        for( TVectorWorkers::iterator
            it = m_workers.begin(),
            it_end = m_workers.end();
        it != it_end;
        ++it )
        {
            WorkerDesc & desc = *it;

            if( desc.worker != _worker )
            {
                continue;
            }

            desc.stop = true;
        }
        
        TVectorWorkers::iterator it_erase = std::remove_if( m_workersAdd.begin(), m_workersAdd.end(), FWorkerFind(_worker));
        m_workersAdd.erase( it_erase, m_workersAdd.end() );

        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadJob::_onMain()
    {
        while( this->isInterrupt() == false )
        {
            m_mutex->lock();
            m_workers.insert( m_workers.end(), m_workersAdd.begin(), m_workersAdd.end() );
            m_workersAdd.clear();
            m_mutex->unlock();

            m_updating = true;

            for( TVectorWorkers::size_type
                it = 0,
                it_end = m_workers.size();
            it != it_end;
            ++it )
            {
                WorkerDesc & desc = m_workers[it];

                if( desc.stop == true )
                {
                    continue;
                }

                m_mutex->lock();
                if( desc.worker->onWork() == false )
                {
                    desc.stop = true;
                }
                m_mutex->unlock();
            }

            m_updating = false;

            THREAD_SERVICE(m_serviceProvider)
                ->sleep( m_sleep );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadJob::_onUpdate()
    {
        if( m_updating == false )
        {
            m_mutex->lock();
            TVectorWorkers::iterator it_erase = std::remove_if( m_workers.begin(), m_workers.end(), FWorkerDone());
            m_workers.erase( it_erase, m_workers.end() );
            m_mutex->unlock();
        }
    }
}
