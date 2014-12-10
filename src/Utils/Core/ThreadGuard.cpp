#	include "ThreadGuard.h"

//////////////////////////////////////////////////////////////////////////
#	include "Logger/Logger.h"
//////////////////////////////////////////////////////////////////////////
#	ifdef WIN32
//////////////////////////////////////////////////////////////////////////
#	include "WindowsLayer/WindowsIncluder.h"

#	define MENGINE_THREAD_GUARD_GET_CURRENT_THREAD_ID() ((uint32_t)GetCurrentThreadId())
#	else
#	define MENGINE_THREAD_GUARD_GET_CURRENT_THREAD_ID() (0U)
#	endif
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		ThreadGuard::ThreadGuard()
			: m_lock(false)
		{
			m_id = MENGINE_THREAD_GUARD_GET_CURRENT_THREAD_ID();
		}
		//////////////////////////////////////////////////////////////////////////
		ThreadGuard::~ThreadGuard()
		{
		}
		//////////////////////////////////////////////////////////////////////////
		void ThreadGuard::check( ServiceProviderInterface * _serviceProvider, const char * _file, uint32_t _line, const char * _doc ) const volatile
		{
			(void)_serviceProvider;

			uint32_t id = MENGINE_THREAD_GUARD_GET_CURRENT_THREAD_ID();

			if( m_id == id )
			{
				return;
			}

			if( _serviceProvider != nullptr )
			{
				LOGGER_CRITICAL(_serviceProvider)("ThreadGuard %d:%d error %s file %d:%s"
					, m_id
					, id
					, _doc
					, _line
					, _file
					);
			}
			else
			{
				MENGINE_THROW_EXCEPTION_FL(_file, _line)(_doc);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		bool ThreadGuard::lock( bool _value ) const volatile
		{
			bool lock = m_lock;

			m_lock = _value;

			return lock;
		}
		//////////////////////////////////////////////////////////////////////////
		ThreadGuardScope::ThreadGuardScope( ServiceProviderInterface * _serviceProvider, volatile const ThreadGuard & _guard, const char * _file, uint32_t _line, const char * _doc )
			: m_serviceProvider(_serviceProvider)
			, m_guard(_guard)
			, m_file(_file)
			, m_line(_line)
			, m_doc(_doc)
		{
			if( m_guard.lock( true ) == true )
			{
				if( m_serviceProvider != nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("ThreadGuardScope + error %s file %d:%s"
						, m_doc
						, m_line
						, m_file
						);
				}

				MENGINE_THROW_EXCEPTION_FL(m_file, m_line)(m_doc);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		ThreadGuardScope::~ThreadGuardScope()
		{
			if( m_guard.lock( false ) == false )
			{
				if( m_serviceProvider != nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("ThreadGuardScope - error %s file %d:%s"
						, m_doc
						, m_line
						, m_file
						);
				}
				
				MENGINE_THROW_EXCEPTION_FL(m_file, m_line)(m_doc);
			}
		}
	}
}