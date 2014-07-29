#	include "MarmaladeThreadSystem.h"

#	include "Logger/Logger.h"

#   include <unistd.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(ThreadSystem, Menge::ThreadSystemInterface, Menge::MarmaladeThreadSystem);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadSystem::MarmaladeThreadSystem()
		: m_serviceProvider(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadSystem::~MarmaladeThreadSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeThreadSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadSystem::initialize()		
	{
        if( s3eThreadAvailable() == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::initialize s3eThread extension not present");

            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadSystem::finalize()
	{
		m_threadIdentities.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr MarmaladeThreadSystem::createThread( int _priority )
	{
		MarmaladeThreadIdentityPtr identity = m_poolWin32ThreadIdentity.createObjectT();

		ThreadMutexInterfacePtr mutex = this->createMutex();

        if( identity->initialize( m_serviceProvider, mutex, _priority ) == false )
		{
			return nullptr;
		}
        
		m_threadIdentities.push_back( identity );
	
		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadSystem::sleep( unsigned int _ms )
	{
        ::usleep( _ms * 1000 );
	}
	//////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr MarmaladeThreadSystem::createMutex()
    {
        MarmaladeThreadMutexPtr mutex = m_poolMarmaladeThreadMutex.createObjectT();
        mutex->initialize( m_serviceProvider );

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeThreadSystem::getCurrentThreadId() const
	{
		s3eThread * threadId = s3eThreadGetCurrent();

		return (uint32_t)threadId;
	}
}
