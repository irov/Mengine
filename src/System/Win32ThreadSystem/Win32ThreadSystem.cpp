#	include "Win32ThreadSystem.h"

#	include "Logger/Logger.h"

#   include <algorithm>

#	include <process.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Menge::ThreadSystemInterface, Menge::Win32ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadSystem::Win32ThreadSystem()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadSystem::~Win32ThreadSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Win32ThreadSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::initialize()		
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::finalize()
	{
		if( m_poolWin32ThreadIdentity.emptyObject() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Win32ThreadSystem::finalize Win32ThreadIdentity not all remove %d"
				, m_poolWin32ThreadIdentity.countObject()
				);
		}

		if( m_poolWin32ThreadMutex.emptyObject() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Win32ThreadSystem::finalize Win32ThreadMutex not all remove %d"
				, m_poolWin32ThreadMutex.countObject()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::avaliable() const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr Win32ThreadSystem::createThread( int _priority )
	{
		Win32ThreadIdentityPtr identity = m_poolWin32ThreadIdentity.createObjectT();

		if( identity == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32ThreadSystem::createThread invalid create identity"
				);

			return nullptr;
		}

		identity->setServiceProvider( m_serviceProvider );

		ThreadMutexInterfacePtr mutex = this->createMutex();

		if( mutex == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32ThreadSystem::createThread invalid create mutex"
				);

			return nullptr;
		}

		if( identity->initialize( mutex, _priority ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32ThreadSystem::createThread invalid initialize"
				);
			
            return nullptr;
        }
		
		return identity;
	}
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr Win32ThreadSystem::createMutex()
    {
        Win32ThreadMutex * mutex = m_poolWin32ThreadMutex.createObjectT();
        mutex->initialize( m_serviceProvider );

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::sleep( unsigned int _ms )
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
