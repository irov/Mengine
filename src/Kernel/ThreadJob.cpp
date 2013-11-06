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
		desc.done = false;

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

			if( desc.done == true )
			{
				break;
			}

			desc.done = true;
            desc.worker->onDone( desc.id );			

            m_workersAdd.erase( it );

            break;
        }
        m_mutexAdd->unlock();

		WorkerDesc desk;
		desk.worker = nullptr;
		desk.id = _id;
		desk.done = false;

		m_mutexRemove->lock();
        m_workersRemove.push_back( desk );
        m_mutexRemove->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
	bool ThreadJob::check_remove( size_t _id )
	{
		for( TWorkers::iterator
			it = m_workersRemove.begin(),
			it_end = m_workersRemove.end();
		it != it_end;
		++it )
		{
			WorkerDesc & desc = *it;

			if( desc.done == true )
			{
				continue;
			}

			if( desc.id == _id )
			{
				desc.done = true;

				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
    bool ThreadJob::_onMain()
    {
        while( this->isInterrupt() == false )
        {   
            m_mutexAdd->lock();
			for( TWorkers::iterator
				it = m_workersAdd.begin(),
				it_end = m_workersAdd.end();
			it != it_end;
			++it)
			{
				WorkerDesc & desc = *it;

				if( desc.done == true )
				{
					continue;
				}
				
				WorkerDesc desc_work;
				desc_work.worker = desc.worker;
				desc_work.id = desc.id;
				desc_work.done = false;

				m_workers.push_back( desc_work );

				desc.done = true;
			}
            m_mutexAdd->unlock();
                        
            for( TWorkers::iterator
                it = m_workers.begin(),
                it_end = m_workers.end();
            it != it_end;)
            {
                WorkerDesc & desc = *it;

                m_mutexRemove->lock();
                bool skip = this->check_remove( desc.id );
				m_mutexRemove->unlock();
                
				if( skip == true || desc.worker->onWork( desc.id ) == false )
				{
					WorkerDesc desc_complete;
					desc_complete.worker = desc.worker;
					desc_complete.id = desc.id;
					desc_complete.done = false;

					m_mutexComplete->lock();
					m_workersComplete.push_back( desc_complete );
					m_mutexComplete->unlock();

					it = m_workers.erase( it );
				}
				else
				{
					++it;
				}
			}            

			m_mutexComplete->lock();
			for( TWorkers::iterator
				it = m_workersComplete.begin(),
				it_end = m_workersComplete.end();
			it != it_end;)
			{
				WorkerDesc & desc = *it;

				if( desc.done == true )
				{
					it = m_workersComplete.erase( it );
				}
				else
				{
					++it;
				}
			}
			m_mutexComplete->unlock();

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

			if( desc.done == true )
			{
				continue;
			}
            
            desc.worker->onDone( desc.id );
			desc.done = true;
        }
        m_mutexComplete->unlock();

		m_mutexAdd->lock();
		for( TWorkers::iterator
			it = m_workersAdd.begin(),
			it_end = m_workersAdd.end();
		it != it_end;)
		{
			WorkerDesc & desc = *it;

			if( desc.done == true )
			{
				it = m_workersAdd.erase( it );
			}
			else
			{
				++it;
			}
		}
		m_mutexAdd->unlock();

		m_mutexRemove->lock();
		for( TWorkers::iterator
			it = m_workersRemove.begin(),
			it_end = m_workersRemove.end();
		it != it_end;)
		{
			WorkerDesc & desc = *it;

			if( desc.done == true )
			{
				it = m_workersRemove.erase( it );
			}
			else
			{
				++it;
			}
		}
		m_mutexRemove->unlock();
    }
}
