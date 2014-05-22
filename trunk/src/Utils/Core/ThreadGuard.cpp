#	include "ThreadGuard.h"

#	if defined(WIN32) && defined(_DEBUG)
#	include "Windows.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadGuard::ThreadGuard()
	{
		m_id = GetCurrentThreadId();
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadGuard::~ThreadGuard()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadGuard::check( const char * _file, uint32_t _line )
	{
		DWORD id = GetCurrentThreadId();

		if( m_id == id )
		{
			return;
		}

		throw ThreadGuardException(_file, _line);
	}
}
#	endif