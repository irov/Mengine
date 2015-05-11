#	include "GlobalHandleSystem.h"

#   include "Logger/Logger.h"

#	include <algorithm>

namespace Menge
{
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct FFindHandler
        {
			FFindHandler( uint32_t _id )
                : m_id(_id)
            {
            }

			bool operator()( const GlobalHandlerDesc & _handle ) const
            {
                return _handle.id == m_id;
            }

            uint32_t m_id;
        };
        //////////////////////////////////////////////////////////////////////////
        struct FDeadHandler
        {
			bool operator ()( const GlobalHandlerDesc & _handle ) const
            {
				return _handle.dead;
            }
        };	
    }
    //////////////////////////////////////////////////////////////////////////
    GlobalHandleSystem::GlobalHandleSystem( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
        , m_handlersEnumerator(0)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleKeyEvent( const InputKeyEvent & _event )
	{
		for( TVectorGlobalHandler::iterator
            it = m_handlers.begin(),
			it_end = m_handlers.end();
        it != it_end;
		++it )
        {
			const GlobalHandlerDesc & desc = *it;

            if( desc.dead == true )
            {
                continue;
            }

			if( desc.enable == false )
			{
				continue;
			}

			desc.handler->handleKeyEvent( _event );
        }

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
	{
		for( TVectorGlobalHandler::iterator
			it = m_handlers.begin(),
			it_end = m_handlers.end();
		it != it_end;
		++it )
		{
			const GlobalHandlerDesc & desc = *it;

            if( desc.dead == true )
            {
                continue;
            }

			if( desc.enable == false )
			{
				continue;
			}

			desc.handler->handleMouseButtonEvent( _event );
        }

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
	{
		for( TVectorGlobalHandler::iterator
			it = m_handlers.begin(),
			it_end = m_handlers.end();
		it != it_end;
		++it )
		{
			const GlobalHandlerDesc & desc = *it;

			if( desc.dead == true )
			{
				continue;
			}

			if( desc.enable == false )
			{
				continue;
			}

			desc.handler->handleMouseButtonEventBegin( _event );
		}	

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
	{
		for( TVectorGlobalHandler::iterator
			it = m_handlers.begin(),
			it_end = m_handlers.end();
		it != it_end;
		++it )
		{
			const GlobalHandlerDesc & desc = *it;

			if( desc.dead == true )
			{
				continue;
			}

			if( desc.enable == false )
			{
				continue;
			}

			desc.handler->handleMouseButtonEventEnd( _event );
        }

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseMove( const InputMouseMoveEvent & _event )
	{
		for( TVectorGlobalHandler::iterator
			it = m_handlers.begin(),
			it_end = m_handlers.end();
		it != it_end;
		++it )
		{
			const GlobalHandlerDesc & desc = *it;

            if( desc.dead == true )
            {
                continue;
            }

			if( desc.enable == false )
			{
				continue;
			}

			desc.handler->handleMouseMove( _event );
        }

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseWheel( const InputMouseWheelEvent & _event )
	{
		for( TVectorGlobalHandler::iterator
			it = m_handlers.begin(),
			it_end = m_handlers.end();
		it != it_end;
		++it )
		{
			const GlobalHandlerDesc & desc = *it;

			if( desc.dead == true )
			{
				continue;
			}

			if( desc.enable == false )
			{
				continue;
			}

			desc.handler->handleMouseWheel( _event );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t GlobalHandleSystem::addGlobalHandler( InputSystemHandler * _handler )
	{
		GlobalHandlerDesc desc;

        uint32_t new_id = ++m_handlersEnumerator;
        desc.id = new_id;

		desc.handler = _handler;
		desc.enable = true;
		desc.dead = false;

		m_handlersAdd.push_back( desc );

        return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	InputSystemHandler * GlobalHandleSystem::removeGlobalHandler( uint32_t _id )
	{
		TVectorGlobalHandler::iterator it_found_add = std::find_if( m_handlersAdd.begin(), m_handlersAdd.end(), FFindHandler( _id ) );

		if( it_found_add != m_handlersAdd.end() )
        {
			InputSystemHandler * handler = it_found_add->handler;

			m_handlersAdd.erase( it_found_add );

            return handler;
        }

		TVectorGlobalHandler::iterator it_found = std::find_if( m_handlers.begin(), m_handlers.end(), FFindHandler( _id ) );

		if( it_found == m_handlers.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("GlobalHandleSystem::removeGlobalMouseEventable not found %d"
                , _id
                );

            return nullptr;
        }
        
		it_found->dead = true;

		InputSystemHandler * handler = it_found->handler;

        return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::enableGlobalHandler( uint32_t _id, bool _value )
	{
		TVectorGlobalHandler::iterator it_found_add = std::find_if( m_handlersAdd.begin(), m_handlersAdd.end(), FFindHandler( _id ) );

		if( it_found_add != m_handlersAdd.end() )
		{
			it_found_add->enable = _value;

			return true;
		}

		TVectorGlobalHandler::iterator it_found = std::find_if( m_handlers.begin(), m_handlers.end(), FFindHandler( _id ) );

		if( it_found == m_handlers.end() )
		{
			LOGGER_ERROR( m_serviceProvider )("GlobalHandleSystem::enableGlobalHandler not found %d"
				, _id
				);

			return false;
		}

		it_found->enable = _value;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::update()
	{
		TVectorGlobalHandler::iterator it_mouse_erase = std::remove_if( m_handlers.begin(), m_handlers.end(), FDeadHandler() );
		m_handlers.erase( it_mouse_erase, m_handlers.end() );

		m_handlers.insert( m_handlers.begin(), m_handlersAdd.begin(), m_handlersAdd.end() );
		m_handlersAdd.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    void GlobalHandleSystem::clear()
    {
		if( m_handlers.empty() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("GlobalHandleSystem::clear global handlers is not empty"
                );
        }

		if( m_handlersAdd.empty() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("GlobalHandleSystem::clear global handlers is not empty (add)"
                );
        }
    }

}
