
#	include "PosixThreadSystem.h"
#	include <vector>

#if defined(WIN32)
	#include <Windows.h>
#endif

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::ThreadSystemInterface **_system )
{
	*_system = new Menge::PosixThreadSystem();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::ThreadSystemInterface *_system )
{
	delete static_cast<Menge::PosixThreadSystem*>(_system);
}
//////////////////////////////////////////////////////////////////////////

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadHolder
	{
	public:
		ThreadHolder( PosixThreadSystem* _system, ThreadInterface* _interface )
			: m_system( _system )
			, m_interface( _interface )
		{
		}

		void main()
		{
			m_interface->main();
			pthread_t tid;
			m_system->removeThread( m_interface, tid );
		}

	protected:
		PosixThreadSystem* m_system;
		ThreadInterface* m_interface;
	};
	//////////////////////////////////////////////////////////////////////////
	static void * s_tread_job( void * _threadHolder )
	{
		ThreadHolder* threadHolder = (ThreadHolder*)_threadHolder;
#if defined(WIN32) && defined(PTW32_STATIC_LIB)
		pthread_win32_thread_attach_np();
#endif
		threadHolder->main();
		delete threadHolder;
#if defined(WIN32) && defined(PTW32_STATIC_LIB)
		pthread_win32_thread_detach_np();
#endif
		//pthread_exit( 0 );
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	PosixThreadSystem::PosixThreadSystem()
	{
#if defined(WIN32) && defined(PTW32_STATIC_LIB)
		// init pthreads
		pthread_win32_process_attach_np();
#endif

		pthread_mutex_init( &m_tidMapMutex, NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	PosixThreadSystem::~PosixThreadSystem()
	{
		std::vector< pthread_t > vThreads;
		pthread_mutex_lock( &m_tidMapMutex );
		vThreads.reserve( m_tidMap.size() );
		for( TTIDMap::iterator it = m_tidMap.begin(), it_end = m_tidMap.end();
			it != it_end;
			++it )
		{
			vThreads.push_back( it->second );
		}
		pthread_mutex_unlock( &m_tidMapMutex );

		int ret;
		for( std::vector< pthread_t >::iterator it = vThreads.begin(), it_end = vThreads.end();
			it != it_end;
			++it )
		{
			pthread_join( (*it), (void**)&ret );
		}
		pthread_mutex_destroy( &m_tidMapMutex );
#if defined(WIN32) && defined(PTW32_STATIC_LIB)
		// deinitialize pthreads
		pthread_win32_thread_detach_np();
		pthread_win32_process_detach_np();
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::createThread( ThreadInterface * _thread )
	{
		pthread_mutex_lock( &m_tidMapMutex );
		TTIDMap::iterator it_insert = m_tidMap.insert( std::make_pair( _thread, pthread_t() ) ).first;
		pthread_mutex_unlock( &m_tidMapMutex );

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE );
		pthread_create( &it_insert->second, &attr, s_tread_job, new ThreadHolder( this, _thread ) );

		pthread_attr_destroy(&attr);
	}
// 	//////////////////////////////////////////////////////////////////////////
// 	void PosixThreadSystem::onThreadEnd( ThreadInterface* _thread )
// 	{
// 
// 	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::joinThread( ThreadInterface* _thread )
	{
		int ret;
		pthread_t tid;
		bool found = removeThread( _thread, tid );
		if( found == true )
		{
			pthread_join( tid, (void**)&ret );
		}
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

		ti.tv_nsec = (tv.tv_usec + (msecs % 1000) * 1000) * 1000;
		ti.tv_sec = tv.tv_sec + (msecs / 1000) + (ti.tv_nsec / 1000000000);
		ti.tv_nsec %= 1000000000;

		pthread_mutex_t mtx;
		pthread_cond_t cnd;

		pthread_mutex_init(&mtx, 0);
		pthread_cond_init(&cnd, 0);

		pthread_mutex_lock(&mtx);
		(void) pthread_cond_timedwait(&cnd, &mtx, ti);
		pthread_mutex_unlock(&mtx);

		pthread_cond_destroy(&cnd);
		pthread_mutex_destroy(&mtx);
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::startMutex()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::stopMutex()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixThreadSystem::removeThread( ThreadInterface* _thread, pthread_t& _tid )
	{
		bool found = false;

		pthread_mutex_lock( &m_tidMapMutex );

		TTIDMap::iterator it_find = m_tidMap.find( _thread );
		if( it_find != m_tidMap.end() )
		{
			found = true;
			_tid = it_find->second;
		}

		m_tidMap.erase( _thread );
		pthread_mutex_unlock( &m_tidMapMutex );

		return found;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
