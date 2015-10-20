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
		~MarmaladeThreadIdentity();

    public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const ThreadMutexInterfacePtr & _mutex, int _priority, const char * _doc );
		
	public:
		void main();

	public:
		bool processTask( ThreadTaskInterface * _task ) override;
		bool joinTask() override;

	public:
		void join() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		
		ThreadMutexInterfacePtr m_mutex;

		const char * m_doc;

		s3eThread * m_thread;

		ThreadTaskInterface * m_task;
		
		volatile bool m_complete;
		volatile bool m_exit;
	};

	typedef stdex::intrusive_ptr<MarmaladeThreadIdentity> MarmaladeThreadIdentityPtr;
}
