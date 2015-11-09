#	include "MarmaladeThreadSystem.h"

#	include "Logger/Logger.h"

#   include "s3eDevice.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(ThreadSystem, Menge::MarmaladeThreadSystem);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadSystem::MarmaladeThreadSystem()
		: m_available(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadSystem::~MarmaladeThreadSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadSystem::_initialize()		
	{
        if( s3eThreadAvailable() == S3E_FALSE )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::initialize s3eThread extension not present"
				);

			m_available = false;
        }
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeThreadSystem::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadSystem::avaliable() const
	{
		return m_available;
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
