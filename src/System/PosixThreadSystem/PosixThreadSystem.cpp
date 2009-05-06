
#	include "PosixThreadSystem.h"
#	include <vector>
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
			m_system->onThreadEnd( m_interface );
		}

	protected:
		PosixThreadSystem* m_system;
		ThreadInterface* m_interface;
	};
	//////////////////////////////////////////////////////////////////////////
	static void * s_tread_job( void * _threadHolder )
	{
		ThreadHolder* threadHolder = (ThreadHolder*)_threadHolder;
		threadHolder->main();
		delete threadHolder;
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	PosixThreadSystem::PosixThreadSystem()
	{
#if defined(WIN32)
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
#if defined(WIN32)
		// deinitialize pthreads
		//pthread_win32_thread_detach_np();
		pthread_win32_process_detach_np();
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::createThread( ThreadInterface * _thread )
	{
		std::pair< TTIDMap::iterator, bool > mapRet;
		pthread_mutex_lock( &m_tidMapMutex );
		mapRet = m_tidMap.insert( std::make_pair( _thread, pthread_t() ) );
		pthread_mutex_unlock( &m_tidMapMutex );

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE );
		pthread_create( &mapRet.first->second, &attr, s_tread_job, new ThreadHolder( this, _thread ) );

		pthread_attr_destroy(&attr);
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::onThreadEnd( ThreadInterface* _thread )
	{
		pthread_mutex_lock( &m_tidMapMutex );
		TTIDMap::iterator it_find = m_tidMap.find( _thread );
		if( it_find != m_tidMap.end() )
		{
			m_tidMap.erase( it_find );
		}
		pthread_mutex_unlock( &m_tidMapMutex );
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::joinThread( ThreadInterface* _thread )
	{
		TTIDMap::iterator it_find;
		bool found = false;
		pthread_t tid;
		pthread_mutex_lock( &m_tidMapMutex );
		it_find = m_tidMap.find( _thread );
		if( it_find != m_tidMap.end() )
		{
			found = true;
			tid = it_find->second;
		}
		pthread_mutex_unlock( &m_tidMapMutex );

		int ret;
		if( found == true )
		{
			pthread_join( tid, (void**)&ret );
		}
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
}