#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "s3eThread.h"

namespace Menge
{
	class MarmaladeThreadIdentity
		: public ThreadIdentity
	{
	public:
		MarmaladeThreadIdentity();

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, const ThreadMutexInterfacePtr & _mutex, int _priority );
		
	public:
		void main();

	public:
		bool addTask( ThreadTaskInterface * _task ) override;
		bool joinTask( ThreadTaskInterface * _task ) override;

	public:
		void join() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		
		ThreadMutexInterfacePtr m_mutex;

		s3eThread * m_thread;

		ThreadTaskInterface * m_task;
		
		volatile bool m_complete;
		volatile bool m_exit;

		bool m_join;
	};

	typedef stdex::intrusive_ptr<MarmaladeThreadIdentity> MarmaladeThreadIdentityPtr;
}
