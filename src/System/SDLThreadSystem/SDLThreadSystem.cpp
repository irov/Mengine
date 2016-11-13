#	include "SDLThreadSystem.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Menge::SDLThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadSystem::SDLThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadSystem::~SDLThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadSystem::_initialize()
    {	
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadSystem::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadSystem::avaliable() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityPtr SDLThreadSystem::createThread( int _priority, const char * _doc)
    {
        SDLThreadIdentityPtr identity = m_poolThreadIdentity.createObject();

        ThreadMutexInterfacePtr mutex = this->createMutex(_doc);

        if( identity->initialize( m_serviceProvider, mutex, _priority, _doc ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLThreadSystem::createThread invalid initialize"
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
    ThreadMutexInterfacePtr SDLThreadSystem::createMutex(const char * _doc)
    {
        SDLThreadMutexPtr mutex = m_poolThreadMutex.createObject();
        
        if( mutex->initialize( m_serviceProvider, _doc ) == false )
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
        const SDL_threadID threadId = SDL_ThreadID();
        return static_cast<ptrdiff_t>(threadId);
    }
}
