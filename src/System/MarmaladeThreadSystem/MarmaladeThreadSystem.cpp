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
		static void * s_tread_job( void * _listener )
		{
			//ThreadHolder * threadHolder = (ThreadHolder *)_threadHolder;
			Menge::ThreadListener * threadListener = static_cast<Menge::ThreadListener*>(_listener);

#if defined(WIN32) && defined(PTW32_STATIC_LIB)
			pthread_win32_thread_attach_np();
#endif

			threadListener->main();

#if defined(WIN32) && defined(PTW32_STATIC_LIB)
			pthread_win32_thread_detach_np();
#endif		
			return threadListener;
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
		for(TVectorPosixThreadIdentity::iterator
			it = m_threadIdentities.begin(),
			it_end = m_threadIdentities.end();
		it != it_end;
		++it )
		{
			MarmaladeThreadIdentity * identity = *it;
			delete identity;
		}

		m_threadIdentities.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr MarmaladeThreadSystem::createThread( const ThreadTaskInterfacePtr & _listener, int _priority )
	{
        s3eThread * thread = s3eThreadCreate( &Detail::s_tread_job, _thread, NULL );

		if( thread == nullptr )
		{			
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createThread: invalid create thread error code - %s"
                , s3eThreadGetErrorString()
				);

			return nullptr;
		}
		
		MarmaladeThreadIdentity * identity = m_poolWin32ThreadIdentity.createT(); 
        identity->initialize( thread );
        
		m_threadIdentities.push_back( identity );
	
		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadSystem::joinThread( const ThreadIdentityPtr & _thread )
	{
		MarmaladeThreadIdentity * identity = static_cast<MarmaladeThreadIdentity*>(_thread);

		s3eThread * thread = identity->getThread();

		ThreadListener * listener = nullptr;
		s3eResult result = s3eThreadJoin( thread, (void**)&listener );

		if( result == S3E_RESULT_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::joinThread: invalid join thread error code - %s"
				, s3eThreadGetErrorString()
				);

			return false;
		}

		listener->join();

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
        MarmaladeThreadMutex * mutex = m_poolMarmaladeThreadMutex.createObjectT();
        mutex->initialize( m_serviceProvider );

        return mutex;
    }
}
