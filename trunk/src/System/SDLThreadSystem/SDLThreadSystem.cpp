#	include "SDLThreadSystem.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Menge::ThreadSystemInterface, Menge::SDLThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SDLThreadSystem::SDLThreadSystem()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SDLThreadSystem::~SDLThreadSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
	void SDLThreadSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * SDLThreadSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SDLThreadSystem::initialize()
	{	
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLThreadSystem::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLThreadSystem::avaliable() const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr SDLThreadSystem::createThread( int _priority )
	{
		SDLThreadIdentityPtr identity = m_poolThreadIdentity.createObjectT();

		ThreadMutexInterfacePtr mutex = this->createMutex();

        if( identity->initialize( m_serviceProvider, mutex, _priority ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createThread invalid initialize"
				);

			return nullptr;
		}

		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLThreadSystem::sleep( unsigned int _ms )
	{
		SDL_Delay( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadMutexInterfacePtr SDLThreadSystem::createMutex()
    {
        SDLThreadMutexPtr mutex = m_poolThreadMutex.createObjectT();
        
		if( mutex->initialize( m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadSystem::createMutex invalid initialize"
				);

			return nullptr;
		}

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	ptrdiff_t SDLThreadSystem::getCurrentThreadId() const
	{
		SDL_threadID threadId = SDL_ThreadID();
		
		return (ptrdiff_t)threadId;
	}
}
