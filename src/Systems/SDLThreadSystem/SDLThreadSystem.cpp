#include "SDLThreadSystem.h"

#include "SDLThreadIdentity.h"
#include "SDLThreadMutex.h"
#include "SDLThreadConditionVariable.h"

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
        m_factoryThreadConditionVariable = new FactoryPool<SDLThreadMutex, 16>();

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
    ThreadIdentityInterfacePtr SDLThreadSystem::createThread( int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line )
    {
		SDLThreadIdentityPtr identity = m_factoryThreadIdentity->createObject( MENGINE_DOCUMENT( "file %s[%d] doc '%s'", _file, _line, _doc ) );

		MENGINE_ASSERTION_MEMORY_PANIC( identity, nullptr )("invalid initialize (doc: %s) (file: '%s:%u')"
			, _doc
			, _file
			, _line
			);

        if( identity->initialize( _priority, _doc, _file, _line ) == false )
        {
            LOGGER_ERROR("invalid initialize (doc: %s) (file: '%s:%u')"
                , _doc
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
    ThreadMutexInterfacePtr SDLThreadSystem::createMutex( const Char * _file, uint32_t _line )
    {
		SDLThreadMutexPtr mutex = m_factoryThreadMutex->createObject( MENGINE_DOCUMENT( "file %s[%d]", _file, _line ) );
        
		MENGINE_ASSERTION_MEMORY_PANIC( mutex, nullptr )("invalid create (doc: '%s:%u')"
			, _file
			, _line
			);

        if( mutex->initialize( _file, _line ) == false )
        {
            LOGGER_ERROR("invalid initialize (doc: '%s:%u')"
                , _file
				, _line
                );

            return nullptr;
        }

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadConditionVariableInterfacePtr SDLThreadSystem::createConditionVariable( const Char * _file, uint32_t _line )
    {
        SDLThreadConditionVariablePtr conditionVariable = m_factoryThreadConditionVariable->createObject( MENGINE_DOCUMENT( "file %s[%d]", _file, _line ) );

        if( conditionVariable->initialize( _file, _line ) == false )
        {
            return nullptr;
        }

        return conditionVariable;
    }
    //////////////////////////////////////////////////////////////////////////
    ptrdiff_t SDLThreadSystem::getCurrentThreadId() const
    {
        const SDL_threadID threadId = SDL_ThreadID();

        return static_cast<ptrdiff_t>(threadId);
    }
}
