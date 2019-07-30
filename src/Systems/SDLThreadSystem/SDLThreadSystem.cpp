#include "SDLThreadSystem.h"

#include "SDLThreadIdentity.h"
#include "SDLThreadMutex.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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
    ThreadIdentityInterfacePtr SDLThreadSystem::createThread( const ConstString & _name, int32_t _priority, const Char * _doc )
    {
        SDLThreadIdentityPtr identity = m_factoryThreadIdentity->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, nullptr, "invalid create thread '%s' (doc: %s)"
            , _name.c_str()
            , _doc
        );

        ThreadMutexInterfacePtr mutex = this->createMutex( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, nullptr, "invalid create mutex" );

        if( identity->initialize( _priority, _name, mutex, _doc ) == false )
        {
            LOGGER_ERROR( "invalid initialize thread '%s' (doc: %s)"
                , _name.c_str()
                , _doc
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
    ThreadMutexInterfacePtr SDLThreadSystem::createMutex( const Char * _doc )
    {
        SDLThreadMutexPtr mutex = m_factoryThreadMutex->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, nullptr, "invalid create (doc: '%s')"
            , _doc
        );

        if( mutex->initialize( _doc ) == false )
        {
            LOGGER_ERROR( "invalid initialize (doc: '%s')"
                , _doc
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
