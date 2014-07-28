#	include "Win32ThreadSystem.h"

#	include "Logger/Logger.h"

#   include <algorithm>

#	include <process.h>

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
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr Win32ThreadSystem::createThread( int _priority )
	{
		Win32ThreadIdentityPtr identity = m_poolWin32ThreadIdentity.createObjectT();

		ThreadMutexInterfacePtr mutex = this->createMutex();

		if( identity->initialize( m_serviceProvider, mutex, _priority ) == false )
		{
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
	uint32_t Win32ThreadSystem::getCurrentThreadId() const
	{
		DWORD id = GetCurrentThreadId();

		return (uint32_t)id;
	}
}
