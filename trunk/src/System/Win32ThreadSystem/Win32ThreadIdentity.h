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

		HANDLE m_handle;

		HANDLE m_hTaskSignalEvent;

		ThreadMutexInterfacePtr m_mutex;
			
        ThreadTaskInterface * m_task;

		volatile bool m_complete;
		volatile bool m_exit;
	};

	typedef stdex::intrusive_ptr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
}
