#	pragma once

#	include "Interface/ThreadSystemInterface.h"
#	include <pthread.h>
#	include <map>

namespace Menge
{

	class PosixThreadSystem
		: public ThreadSystemInterface
	{
	public:
		PosixThreadSystem();
		~PosixThreadSystem();

	public:
		void createThread( ThreadInterface * _thread ) override;
		void joinThread( ThreadInterface* _thread ) override;
		void sleep( unsigned int _ms ) override;

	public:
		MutexInterface* createMutex() override;
		void releaseMutex( MutexInterface* _mutex ) override;

 	public:
 		bool removeThread( ThreadInterface* _thread, pthread_t& _tid );

	protected:
		typedef std::map< ThreadInterface*, pthread_t > TTIDMap;
		TTIDMap m_tidMap;
		pthread_mutex_t m_tidMapMutex;
	};

}	// namespace Menge
