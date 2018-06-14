#include "C11ThreadSystem.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Mengine::C11ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	C11ThreadSystem::C11ThreadSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	C11ThreadSystem::~C11ThreadSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool C11ThreadSystem::_initializeService()
	{
        m_factoryC11ThreadIdentity = new FactoryPool<C11ThreadIdentity, 16>();
        m_factoryC11ThreadMutex = new FactoryPool<C11ThreadMutex, 16>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void C11ThreadSystem::_finalizeService()
	{
		if( m_factoryC11ThreadIdentity->isEmptyObjects() == false )
		{
			LOGGER_ERROR("C11ThreadSystem::finalize Win32ThreadIdentity not all remove %d"
				, m_factoryC11ThreadIdentity->getCountObject()
				);
		}

		if( m_factoryC11ThreadMutex->isEmptyObjects() == false )
		{
			LOGGER_ERROR("C11ThreadSystem::finalize Win32ThreadMutex not all remove %d"
				, m_factoryC11ThreadMutex->getCountObject()
				);
		}

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryC11ThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryC11ThreadMutex );

        m_factoryC11ThreadIdentity = nullptr;
        m_factoryC11ThreadMutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool C11ThreadSystem::avaliable() const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr C11ThreadSystem::createThread( int _priority, const char * _file, uint32_t _line )
	{
		C11ThreadIdentityPtr identity = m_factoryC11ThreadIdentity->createObject();

		if( identity == nullptr )
		{
			LOGGER_ERROR("C11ThreadSystem::createThread invalid create identity"
				);

			return nullptr;
		}
        
		if( identity->initialize( _priority, _file, _line ) == false )
		{
			LOGGER_ERROR("C11ThreadSystem::createThread invalid initialize"
				);
			
            return nullptr;
        }
		
		return identity;
	}
    //////////////////////////////////////////////////////////////////////////
	ThreadMutexInterfacePtr C11ThreadSystem::createMutex( const char * _file, uint32_t _line )
    {
        C11ThreadMutexPtr mutex = m_factoryC11ThreadMutex->createObject();

        if( mutex == nullptr )
        {
            return nullptr;
        }

		mutex->initialize( _file, _line );

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	void C11ThreadSystem::sleep( uint32_t _ms )
	{
		::Sleep( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
	ptrdiff_t C11ThreadSystem::getCurrentThreadId() const
	{
		DWORD id = GetCurrentThreadId();

		return (ptrdiff_t)id;
	}
}
