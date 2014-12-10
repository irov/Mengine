#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"

#	include "stdex/exception.h"

#	ifndef MENGINE_THREAD_GUARD_ENABLE
#	ifdef _DEBUG
#	define MENGINE_THREAD_GUARD
#	endif
#	else
#	define MENGINE_THREAD_GUARD
#	endif

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		class ThreadGuard
		{
		public:
			ThreadGuard();
			~ThreadGuard();

		public:
			void check( ServiceProviderInterface * _serviceProvider, const char * _file, uint32_t _line, const char * _doc ) const volatile;
			bool lock( bool _value ) const volatile;

		protected:
			volatile uint32_t m_id;
			volatile mutable bool m_lock;
		};
		//////////////////////////////////////////////////////////////////////////
		class ThreadGuardScope
		{
		public:
			ThreadGuardScope( ServiceProviderInterface * _serviceProvider, volatile const ThreadGuard & _guard, const char * _file, uint32_t _line, const char * _doc );
			~ThreadGuardScope();

		private:
			void operator = ( const ThreadGuardScope & ){};

		protected:
			ServiceProviderInterface * m_serviceProvider;

			volatile const ThreadGuard & m_guard;
			const char * m_file;
			uint32_t m_line;
			const char * m_doc;
		};
	}

#	ifdef MENGINE_THREAD_GUARD
	//////////////////////////////////////////////////////////////////////////
#	define THREAD_GUARD_INIT\
	public:\
	volatile Helper::ThreadGuard m_mengine_threadGuard;\
	protected:
	//////////////////////////////////////////////////////////////////////////
#	define THREAD_GUARD_CHECK(Self, ServiceProvider, Doc)\
	Self->m_mengine_threadGuard.check( ServiceProvider, __FILE__, __LINE__, Doc )
	//////////////////////////////////////////////////////////////////////////
#	define THREAD_GUARD_SCOPE(Self, ServiceProvider, Doc)\
	Helper::ThreadGuardScope mengine_threadGuardScope(ServiceProvider, Self->m_mengine_threadGuard, __FILE__, __LINE__, Doc )
	//////////////////////////////////////////////////////////////////////////
#	else
	//////////////////////////////////////////////////////////////////////////
#	define THREAD_GUARD_INIT
	//////////////////////////////////////////////////////////////////////////
#	define THREAD_GUARD_CHECK(Self, ServiceProvider, Doc)
	//////////////////////////////////////////////////////////////////////////
#	define THREAD_GUARD_SCOPE(Self, ServiceProvider, Doc)
#	endif
} 

