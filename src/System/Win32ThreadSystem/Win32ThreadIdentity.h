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
        void setHandle( HANDLE _handle );
		HANDLE getHandle() const;

	protected:
		HANDLE m_handle;
	};
}
