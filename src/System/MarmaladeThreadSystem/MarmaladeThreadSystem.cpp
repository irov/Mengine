#	include "MarmaladeThreadSystem.h"

#	include "Logger/Logger.h"

#   include "s3eDevice.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(ThreadSystem, Menge::ThreadSystemInterface, Menge::MarmaladeThreadSystem);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadSystem::MarmaladeThreadSystem()
		: m_serviceProvider(nullptr)
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
        if( s3eThreadAvailable() == S3E_FALSE )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::initialize s3eThread extension not present"
				);

            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadSystem::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadSystem::avaliable() const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr MarmaladeThreadSystem::createThread( int _priority, const char * _doc )
	{
		MarmaladeThreadIdentityPtr identity = m_poolWin32ThreadIdentity.createObject();

		ThreadMutexInterfacePtr mutex = this->createMutex( _doc );

		if( identity->initialize( m_serviceProvider, mutex, _priority, _doc ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createThread invalid initialize"
				);

			return nullptr;
		}

		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadSystem::sleep( unsigned int _ms )
	{
        s3eDeviceYield( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadMutexInterfacePtr MarmaladeThreadSystem::createMutex( const char * _doc )
    {
        MarmaladeThreadMutexPtr mutex = m_poolMarmaladeThreadMutex.createObject();
        
		if( mutex->initialize( m_serviceProvider, _doc ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createMutex invalid initialize"
				);

			return nullptr;
		}

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	ptrdiff_t MarmaladeThreadSystem::getCurrentThreadId() const
	{
		s3eThread * threadId = s3eThreadGetCurrent();
		
		return (ptrdiff_t)threadId;
	}
}
