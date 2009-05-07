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
		void startMutex() override;
		void stopMutex() override;

	public:
		void onThreadEnd( ThreadInterface* _thread );

	protected:
		typedef std::map< ThreadInterface*, pthread_t > TTIDMap;
		TTIDMap m_tidMap;
		pthread_mutex_t m_tidMapMutex;
	};

}	// namespace Menge
