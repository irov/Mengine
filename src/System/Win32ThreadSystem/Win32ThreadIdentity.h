#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	class Win32ThreadIdentity
		: public ThreadIdentity
	{
	public:
		Win32ThreadIdentity();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		ServiceProviderInterface * getServiceProvider() const;

	public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex, int _priority, const char * _doc );

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

		HANDLE m_handle;

		const char * m_doc;
			
        ThreadTaskInterface * m_task;

		volatile bool m_complete;
		volatile bool m_exit;
	};

	typedef stdex::intrusive_ptr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
}
