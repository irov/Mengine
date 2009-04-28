
#	include "PosixThreadSystem.h"
#	include <pthread.h>

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
	PosixThreadSystem::PosixThreadSystem()
	{
#if defined(WIN32)
		// init pthreads
		pthread_win32_process_attach_np();
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	PosixThreadSystem::~PosixThreadSystem()
	{

#if defined(WIN32)
		// deinitialize pthreads
		//pthread_win32_thread_detach_np();
		pthread_win32_process_detach_np();
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	static void * s_tread_job( void * _impl )
	{
		ThreadInterface * thread = (ThreadInterface *)_impl;
		thread->main();
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixThreadSystem::createThread( ThreadInterface * _tread )
	{
		pthread_t thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE );
		pthread_create( &thread, &attr, s_tread_job, _tread );

		pthread_attr_destroy(&attr);
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