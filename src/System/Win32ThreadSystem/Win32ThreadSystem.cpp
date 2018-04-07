#include "Win32ThreadSystem.h"

#include "Factory/FactoryPool.h"
#include "Logger/Logger.h"

#include <algorithm>

#include <process.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Mengine::Win32ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
        m_factoryWin32ThreadIdentity = new FactoryPool<Win32ThreadIdentity, 16>();
        m_factoryWin32ThreadMutex = new FactoryPool<Win32ThreadMutex, 16>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::_finalize()
	{
		if( m_factoryWin32ThreadIdentity->isEmptyObjects() == false )
		{
			LOGGER_ERROR("Win32ThreadSystem::finalize Win32ThreadIdentity not all remove %d"
				, m_factoryWin32ThreadIdentity->getCountObject()
				);
		}

		if( m_factoryWin32ThreadMutex->isEmptyObjects() == false )
		{
			LOGGER_ERROR("Win32ThreadSystem::finalize Win32ThreadMutex not all remove %d"
				, m_factoryWin32ThreadMutex->getCountObject()
				);
		}

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadMutex );

        m_factoryWin32ThreadIdentity = nullptr;
        m_factoryWin32ThreadMutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::avaliable() const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr Win32ThreadSystem::createThread( int _priority, const char * _file, uint32_t _line )
	{
		Win32ThreadIdentityPtr identity = m_factoryWin32ThreadIdentity->createObject();

		if( identity == nullptr )
		{
			LOGGER_ERROR("Win32ThreadSystem::createThread invalid create identity"
				);

			return nullptr;
		}
        
		ThreadMutexInterfacePtr mutex = this->createMutex( _file, _line );

		if( mutex == nullptr )
		{
			LOGGER_ERROR("Win32ThreadSystem::createThread invalid create mutex"
				);

			return nullptr;
		}

		if( identity->initialize( mutex, _priority, _file, _line ) == false )
		{
			LOGGER_ERROR("Win32ThreadSystem::createThread invalid initialize"
				);
			
            return nullptr;
        }
		
		return identity;
	}
    //////////////////////////////////////////////////////////////////////////
	ThreadMutexInterfacePtr Win32ThreadSystem::createMutex( const char * _file, uint32_t _line )
    {
        Win32ThreadMutex * mutex = m_factoryWin32ThreadMutex->createObject();

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
