#include "PlayerGlobalInputHandler.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Assertion.h"

#include "Config/StdIntTypes.h"
#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PlayerGlobalInputHandler::PlayerGlobalInputHandler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PlayerGlobalInputHandler::~PlayerGlobalInputHandler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerGlobalInputHandler::finalize()
    {
#if defined(MENGINE_DEBUG)
        m_handlers.insert( m_handlers.end(), m_handlersAdd.begin(), m_handlersAdd.end() );
        m_handlersAdd.clear();

        m_handlers.erase( StdAlgorithm::remove_if( m_handlers.begin(), m_handlers.end(), []( const GlobalHandlerDesc & _handle )
        {
            return _handle.dead;
        } ), m_handlers.end() );

        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            LOGGER_ASSERTION( "was forgotten remove player global input handler [%u] (doc: %s)"
                , desc.id
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        MENGINE_ASSERTION( m_handlers.empty() == true, "finalized player global input has [%" MENGINE_PRIuPTR "] hadlers"
            , m_handlers.size()
        );

        m_handlers.clear();
        m_handlersAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleKeyEvent( const InputKeyEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleKeyEvent( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleTextEvent( const InputTextEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleTextEvent( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleAccelerometerEvent( const InputAccelerometerEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleAccelerometerEvent( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleMouseButtonEvent( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleMouseButtonEventBegin( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleMouseButtonEventEnd( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleMouseMove( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleMouseWheel( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            bool handle = desc.handler->handleMouseEnter( _event );
            MENGINE_UNUSED( handle );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerGlobalInputHandler::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            desc.handler->handleMouseLeave( _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId PlayerGlobalInputHandler::addGlobalHandler( const InputHandlerInterfacePtr & _handler, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        GlobalHandlerDesc desc;

        UniqueId new_id = Helper::generateUniqueIdentity();

        desc.id = new_id;

        desc.handler = _handler;
        desc.enable = true;
        desc.dead = false;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_handlersAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    InputHandlerInterfacePtr PlayerGlobalInputHandler::removeGlobalHandler( UniqueId _id )
    {
        VectorGlobalHandler::iterator it_found_add = StdAlgorithm::find_if( m_handlersAdd.begin(), m_handlersAdd.end(), [_id]( const GlobalHandlerDesc & _handle )
        {
            return _handle.id == _id;
        } );

        if( it_found_add != m_handlersAdd.end() )
        {
            InputHandlerInterfacePtr handler = it_found_add->handler;
            it_found_add->handler = nullptr;

            m_handlersAdd.erase( it_found_add );

            return handler;
        }

        VectorGlobalHandler::iterator it_found = StdAlgorithm::find_if( m_handlers.begin(), m_handlers.end(), [_id]( const GlobalHandlerDesc & _handle )
        {
            return _handle.id == _id;
        } );

        if( it_found == m_handlers.end() )
        {
            LOGGER_ERROR( "not found [%u]"
                , _id
            );

            return nullptr;
        }

        InputHandlerInterfacePtr handler = it_found->handler;

        it_found->dead = true;
        it_found->handler = nullptr;

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerGlobalInputHandler::enableGlobalHandler( UniqueId _id, bool _value )
    {
        VectorGlobalHandler::iterator it_found_add = StdAlgorithm::find_if( m_handlersAdd.begin(), m_handlersAdd.end(), [_id]( const GlobalHandlerDesc & _handle )
        {
            return _handle.id == _id;
        } );

        if( it_found_add != m_handlersAdd.end() )
        {
            it_found_add->enable = _value;

            return true;
        }

        VectorGlobalHandler::iterator it_found = StdAlgorithm::find_if( m_handlers.begin(), m_handlers.end(), [_id]( const GlobalHandlerDesc & _handle )
        {
            return _handle.id == _id;
        } );

        if( it_found == m_handlers.end() )
        {
            LOGGER_ERROR( "not found [%u]"
                , _id
            );

            return false;
        }

        it_found->enable = _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerGlobalInputHandler::update()
    {
        m_handlers.insert( m_handlers.end(), m_handlersAdd.begin(), m_handlersAdd.end() );
        m_handlersAdd.clear();

        VectorGlobalHandler::iterator it_erase = StdAlgorithm::remove_if( m_handlers.begin(), m_handlers.end(), []( const GlobalHandlerDesc & _handle )
        {
            return _handle.dead;
        } );

        m_handlers.erase( it_erase, m_handlers.end() );
    }
    //////////////////////////////////////////////////////////////////////////
}
