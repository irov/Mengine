#	include "SDLThreadSystem.h"

#	include "SDLThreadIdentity.h"
#   include "SDLThreadMutex.h"

#   include "Kernel/FactoryPool.h"

#	include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Mengine::SDLThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
    bool SDLThreadSystem::_initializeService()
    {	
        m_factoryThreadIdentity = new FactoryPool<SDLThreadIdentity, 16>();
        m_factoryThreadMutex = new FactoryPool<SDLThreadMutex, 16>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadMutex );

        m_factoryThreadIdentity = nullptr;
        m_factoryThreadMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadSystem::avaliable() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr SDLThreadSystem::createThread( int _priority, const char * _file, uint32_t _line )
    {
        SDLThreadIdentityPtr identity = m_factoryThreadIdentity->createObject();

        if( identity == nullptr )
        {
            LOGGER_ERROR("SDLThreadSystem::createThread invalid initialize (doc: '%s:%u')"
                , _file
				, _line
                );

            return nullptr;
        }

		ThreadMutexInterfacePtr mutex = this->createMutex( _file, _line );

        if( identity->initialize( mutex, _priority, _file, _line ) == false )
        {
            LOGGER_ERROR("SDLThreadSystem::createThread invalid initialize (doc: '%s:%u')"
                , _file
				, _line
                );

            return nullptr;
        }

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadSystem::sleep( uint32_t _ms )
    {
        SDL_Delay( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr SDLThreadSystem::createMutex( const char * _file, uint32_t _line )
    {
        SDLThreadMutexPtr mutex = m_factoryThreadMutex->createObject();
        
        if( mutex == nullptr )
        {
            LOGGER_ERROR("SDLThreadSystem::createMutex invalid create (doc: '%s:%u')"
                , _file
				, _line
                );

            return nullptr;
        }

        if( mutex->initialize( _file, _line ) == false )
        {
            LOGGER_ERROR("SDLThreadSystem::createMutex invalid initialize (doc: '%s:%u')"
                , _file
				, _line
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
