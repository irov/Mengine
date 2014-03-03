#	include "MarmaladeThreadSystem.h"

#	include "Logger/Logger.h"

#   include <unistd.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(ThreadSystem, Menge::ThreadSystemInterface, Menge::MarmaladeThreadSystem);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Detail
	{
		//////////////////////////////////////////////////////////////////////////
		static void * s_tread_job( void * _task )
		{
			//ThreadHolder * threadHolder = (ThreadHolder *)_threadHolder;
			ThreadTaskInterface * task = static_cast<ThreadTaskInterface *>(_task);

			task->main();

			return task;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadSystem::MarmaladeThreadSystem()
		: m_serviceProvider(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadSystem::~MarmaladeThreadSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeThreadSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadSystem::initialize()		
	{
        if( s3eThreadAvailable() == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::initialize s3eThread extension not present");

            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadSystem::finalize()
	{
		m_threadIdentities.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr MarmaladeThreadSystem::createThread( const ThreadTaskInterfacePtr & _task, int _priority )
	{
		ThreadTaskInterface * task_ptr = _task.get();
        s3eThread * s3e_thread = s3eThreadCreate( &Detail::s_tread_job, (void *)task_ptr, NULL );

		if( s3e_thread == nullptr )
		{			
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createThread: invalid create thread error code - %s"
                , s3eThreadGetErrorString()
				);

			return nullptr;
		}
		
		MarmaladeThreadIdentityPtr identity = m_poolWin32ThreadIdentity.createObjectT(); 
        identity->initialize( s3e_thread, _task );
        
		m_threadIdentities.push_back( identity );
	
		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadSystem::joinThread( const ThreadIdentityPtr & _thread )
	{
		MarmaladeThreadIdentityPtr identity = stdex::intrusive_static_cast<MarmaladeThreadIdentityPtr>(_thread);

		const ThreadTaskInterfacePtr & task = identity->getTask();
		task->cancel();

		s3eThread * s3e_thread = identity->getThread();

		ThreadTaskInterface * join_task = nullptr;
		s3eResult result = s3eThreadJoin( s3e_thread, (void**)&join_task );

		if( result == S3E_RESULT_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::joinThread: invalid join thread error code - %s"
				, s3eThreadGetErrorString()
				);

			return false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadSystem::sleep( unsigned int _ms )
	{
        ::usleep( _ms * 1000 );
	}
	//////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr MarmaladeThreadSystem::createMutex()
    {
        MarmaladeThreadMutexPtr mutex = m_poolMarmaladeThreadMutex.createObjectT();
        mutex->initialize( m_serviceProvider );

        return mutex;
    }
}
