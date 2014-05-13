#	pragma once

#	include "Config/Typedef.h"

#	if defined(WIN32) || defined(_DEBUG)
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	class ThreadGuardException
	{
	public:
		ThreadGuardException( const char * _file, uint32_t _line )
			: file(_file)
			, line(_line)
		{
		}

	public:
		const char * file;
		uint32_t line;
	};

	class ThreadGuard
	{
	public:
		ThreadGuard();
		~ThreadGuard();

	public:
		void check( const char * _file, uint32_t _line );

	protected:
		uint32_t m_id;
	};

#	define THREAD_GUARD_INIT\
	public:\
	ThreadGuard m_threadGuard;\
	protected:


#	define THREAD_GUARD_CHECK(Self)\
	Self->m_threadGuard.check(__FILE__, __LINE__)
}
//////////////////////////////////////////////////////////////////////////
#	else
//////////////////////////////////////////////////////////////////////////
#	define THREAD_GUARD_INIT

#	define THREAD_GUARD_CHECK(Self)\
	(void)Self
#	endif


