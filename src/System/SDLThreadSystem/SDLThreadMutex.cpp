#   include "SDLThreadMutex.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	SDLThreadMutex::SDLThreadMutex()
        : m_serviceProvider(nullptr)
        , m_cs(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLThreadMutex::initialize( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

		SDL_mutex * cs = SDL_CreateMutex();

		if( cs == nullptr )
		{
			return false;
		}

		m_cs = cs;

		return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLThreadMutex::lock()
    {
		int err = SDL_LockMutex( m_cs );

		if( err != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("SDLThreadMutex::lock invalid lock"
				);
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLThreadMutex::unlock()
    {
		int err = SDL_UnlockMutex( m_cs );

		if( err != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("SDLThreadMutex::unlock invalid unlock"
				);
		}
    }
	//////////////////////////////////////////////////////////////////////////
	bool SDLThreadMutex::try_lock()
	{
		int err = SDL_TryLockMutex( m_cs );

		if( err == 0 )
		{
			return true;
		}
		else if( err == -1 )
		{
			LOGGER_ERROR( m_serviceProvider )("SDLThreadMutex::lock invalid try lock"
				);
		}

		return false;
	}
    //////////////////////////////////////////////////////////////////////////
	void SDLThreadMutex::_destroy()
    {
		SDL_DestroyMutex( m_cs );
    }
}