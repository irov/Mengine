#	include "PosixThreadSystem.h"
#	include "PosixThreadIdentity.h"

#	include "Interface/LogSystemInterface.h"

#	include "Logger/Logger.h"

#if defined(WIN32)
#include <Windows.h>
#else
#   include <sys/time.h>
#endif

#	include <pthread.h>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::ThreadSystemInterface **_system )
{
	if( _system == 0 )
	{
		return false;
	}

	*_system = new Menge::PosixThreadSystem();

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::ThreadSystemInterface *_system )
{
	if( _system != 0 )
	{
		delete static_cast<Menge::PosixThreadSystem*>( _system );
	}
}
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
	PosixThreadSystem::PosixThreadSystem()
		: m_serviceProvider(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixThreadSystem::~PosixThreadSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixThreadSystem::initialize( ServiceProviderInterface * _serviceProvider )		
	{
		m_serviceProvider = _serviceProvider;

		m_logService = m_serviceProvider->getServiceT<LogServiceInterface>("LogService");

	#if defined(WIN32) && defined(PTW32_STATIC_LIB)
		// init pthreads
		int state = pthread_win32_process_attach_np();
		if(state == 0)
		{
			return false;
		}
	#endif
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::finalize()
	{
	#if defined(WIN32) && defined(PTW32_STATIC_LIB)
		// deinitialize pthreads
		pthread_win32_thread_detach_np();
		pthread_win32_process_detach_np();
	#endif

		for(TVectorPosixThreadIdentity::iterator
			it = m_threadIdentities.begin(),
			it_end = m_threadIdentities.end();
		it != it_end;
		++it )
		{
			PosixThreadIdentity * identity = *it;
			delete identity;
		}

		m_threadIdentities.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentity * PosixThreadSystem::createThread( ThreadListener * _thread )
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE );

		//Detail::ThreadHolder * holder = new Detail::ThreadHolder( this, _thread );
		pthread_t threadId;
		int error_code = pthread_create( &threadId, &attr, Detail::s_tread_job, _thread );
		if( error_code != 0 )
		{			
			LOGGER_ERROR(m_logService)("PosixThreadSystem::createThread: invalid create thread error code - %d"
				, error_code
				);

			return NULL;
		}
		
		PosixThreadIdentity * identity = new PosixThreadIdentity(threadId);
		m_threadIdentities.push_back( identity );
		pthread_attr_destroy(&attr);
		
		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::joinThread( ThreadIdentity * _thread )
	{
		PosixThreadIdentity * identity = static_cast<PosixThreadIdentity*>(_thread);

		pthread_t threadId = identity->getId();

		ThreadListener * listener = NULL;
		int error_code = pthread_join( threadId, (void**)&listener );
		if( error_code != 0 )
		{
			LOGGER_ERROR(m_logService)("PosixThreadSystem::joinThread: invalid join thread error code - %d"
				, error_code
				);

			return;
		}

		listener->join();
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::sleep( unsigned int _ms )
	{
#if defined(WIN32)
		Sleep( _ms );
#else
		struct timeval tv;
		gettimeofday(&tv, 0);
		struct timespec ti;

		ti.tv_nsec = (tv.tv_usec + ( _ms % 1000) * 1000) * 1000;
		ti.tv_sec = tv.tv_sec + ( _ms / 1000) + (ti.tv_nsec / 1000000000);
		ti.tv_nsec %= 1000000000;

		pthread_mutex_t mtx;
		pthread_cond_t cnd;

		pthread_mutex_init(&mtx, 0);
		pthread_cond_init(&cnd, 0);

		pthread_mutex_lock(&mtx);
		(void) pthread_cond_timedwait(&cnd, &mtx, &ti);
		pthread_mutex_unlock(&mtx);

		pthread_cond_destroy(&cnd);
		pthread_mutex_destroy(&mtx);
#endif
	}
	//////////////////////////////////////////////////////////////////////////
}
