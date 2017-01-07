#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

#	include "Core/ServantBase.h"

namespace Menge
{
	class Win32ThreadIdentity
		: public ServantBase<ThreadIdentity>
	{
	public:
		Win32ThreadIdentity();

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
		ThreadMutexInterfacePtr m_mutex;

		HANDLE m_handle;

		const char * m_doc;
			
        ThreadTaskInterface * m_task;

		volatile bool m_complete;
		volatile bool m_exit;
	};

	typedef stdex::intrusive_ptr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
}
