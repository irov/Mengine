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
        void initialize( HANDLE _handle, const ThreadTaskInterfacePtr & _thread );

    public:
		HANDLE getHandle() const;
        const ThreadTaskInterfacePtr & getListener() const;
		
	protected:
		HANDLE m_handle;

        ThreadTaskInterfacePtr m_thread;
	};

	typedef stdex::intrusive_ptr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
}
