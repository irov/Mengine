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
        void initialize( HANDLE _handle, ThreadTaskInterface * _thread );

    public:
		HANDLE getHandle() const;
        ThreadTaskInterface * getListener() const;

	protected:
		HANDLE m_handle;

        ThreadTaskInterface * m_thread;
	};
}
