#	include "Win32ThreadSystem.h"

#   include "Factory/FactoryPool.h"
#	include "Logger/Logger.h"

#   include <algorithm>

#	include <process.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Menge::Win32ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadSystem::Win32ThreadSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadSystem::~Win32ThreadSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::_initialize()		
	{
        m_poolWin32ThreadIdentity = new FactoryPool<Win32ThreadIdentity, 16>( m_serviceProvider );
        m_poolWin32ThreadMutex = new FactoryPool<Win32ThreadMutex, 16>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::_finalize()
	{
		if( m_poolWin32ThreadIdentity->emptyObject() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Win32ThreadSystem::finalize Win32ThreadIdentity not all remove %d"
				, m_poolWin32ThreadIdentity->countObject()
				);
		}

		if( m_poolWin32ThreadMutex->emptyObject() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Win32ThreadSystem::finalize Win32ThreadMutex not all remove %d"
				, m_poolWin32ThreadMutex->countObject()
				);
		}

        m_poolWin32ThreadIdentity = nullptr;
        m_poolWin32ThreadMutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::avaliable() const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr Win32ThreadSystem::createThread( int _priority, const char * _file, uint32_t _line )
	{
		Win32ThreadIdentityPtr identity = m_poolWin32ThreadIdentity->createObject();

		if( identity == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32ThreadSystem::createThread invalid create identity"
				);

			return nullptr;
		}

		identity->setServiceProvider( m_serviceProvider );

		ThreadMutexInterfacePtr mutex = this->createMutex( _file, _line );

		if( mutex == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32ThreadSystem::createThread invalid create mutex"
				);

			return nullptr;
		}

		if( identity->initialize( mutex, _priority, _file, _line ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32ThreadSystem::createThread invalid initialize"
				);
			
            return nullptr;
        }
		
		return identity;
	}
    //////////////////////////////////////////////////////////////////////////
	ThreadMutexInterfacePtr Win32ThreadSystem::createMutex( const char * _file, uint32_t _line )
    {
        Win32ThreadMutex * mutex = m_poolWin32ThreadMutex->createObject();

        mutex->setServiceProvider( m_serviceProvider );

		mutex->initialize( _file, _line );

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::sleep( uint32_t _ms )
	{
		::Sleep( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
	ptrdiff_t Win32ThreadSystem::getCurrentThreadId() const
	{
		DWORD id = GetCurrentThreadId();

		return (ptrdiff_t)id;
	}
}
