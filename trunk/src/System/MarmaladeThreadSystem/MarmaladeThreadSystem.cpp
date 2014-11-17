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
        if( s3eThreadAvailable() == 0 )
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
		for( TVectorPosixThreadIdentity::iterator
			it = m_threadIdentities.begin(),
			it_end = m_threadIdentities.end();
		it != it_end;
		++it )
		{
			const MarmaladeThreadIdentityPtr & thread = *it;

			thread->join();
		}
		
		m_threadIdentities.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadSystem::avaliable() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr MarmaladeThreadSystem::createThread( int _priority )
	{
		MarmaladeThreadIdentityPtr identity = m_poolWin32ThreadIdentity.createObjectT();

		ThreadMutexInterfacePtr mutex = this->createMutex();

        if( identity->initialize( m_serviceProvider, mutex, _priority ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createThread invalid initialize"
				);

			return nullptr;
		}

		m_threadIdentities.push_back( identity );

		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadSystem::sleep( unsigned int _ms )
	{
        s3eDeviceYield( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr MarmaladeThreadSystem::createMutex()
    {
        MarmaladeThreadMutexPtr mutex = m_poolMarmaladeThreadMutex.createObjectT();
        
		if( mutex->initialize( m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createMutex invalid initialize"
				);

			return nullptr;
		}

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeThreadSystem::getCurrentThreadId() const
	{
		s3eThread * threadId = s3eThreadGetCurrent();

		return (uint32_t)threadId;
	}
}
