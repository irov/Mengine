#	include "GlobalHandleSystem.h"

#   include "Logger/Logger.h"

#	include <algorithm>

namespace Menge
{
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct FFindMouseHandler
        {
            FFindMouseHandler( size_t _id )
                : m_id(_id)
            {
            }

            bool operator()( const GlobalMouseHandleDesc & _handle ) const
            {
                return _handle.id == m_id;
            }

            size_t m_id;
        };
        //////////////////////////////////////////////////////////////////////////
        struct FDeadMouseHandler
        {
            bool operator ()( const GlobalMouseHandleDesc & _handle ) const
            {
                if( _handle.dead == false )
                {
                    return false;
                }

                return true;            
            }
        };	
        //////////////////////////////////////////////////////////////////////////
        struct FFindKeyHandler
        {
            FFindKeyHandler( size_t _id )
                : m_id(_id)
            {
            }

            bool operator()( const GlobalKeyHandleDesc & _handle ) const
            {
                return _handle.id == m_id;
            }

            size_t m_id;
        };
        //////////////////////////////////////////////////////////////////////////
        struct FDeadKeyHandler
        {
            bool operator ()( const GlobalKeyHandleDesc & _handle ) const
            {
                if( _handle.dead == false )
                {
                    return false;
                }

                return true;            
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
	void GlobalHandleSystem::handleGlobalKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
        for( TVectorGlobalKeyHandler::size_type
            it = 0,
            it_end = m_globalKeyHandler.size();
        it != it_end;
        ++it)
        {
            const GlobalKeyHandleDesc & desc = m_globalKeyHandler[it];

            if( desc.dead == true )
            {
                continue;
            }

            desc.handler->handleGlobalKeyEvent( _point, _key, _char, _isDown );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::handleGlobalMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        for( TVectorGlobalMouseHandler::size_type
            it = 0,
            it_end = m_globalMouseHandler.size();
        it != it_end;
        ++it)
        {
            const GlobalMouseHandleDesc & desc = m_globalMouseHandler[it];

            if( desc.dead == true )
            {
                continue;
            }

            desc.handler->handleGlobalMouseButtonEvent( _touchId, _point, _button, _isDown );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::handleGlobalMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        for( TVectorGlobalMouseHandler::size_type
            it = 0,
            it_end = m_globalMouseHandler.size();
        it != it_end;
        ++it)
        {
            const GlobalMouseHandleDesc & desc = m_globalMouseHandler[it];

			if( desc.dead == true )
			{
				continue;
			}

			desc.handler->handleGlobalMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::handleGlobalMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        for( TVectorGlobalMouseHandler::size_type
            it = 0,
            it_end = m_globalMouseHandler.size();
        it != it_end;
        ++it)
        {
            const GlobalMouseHandleDesc & desc = m_globalMouseHandler[it];

			if( desc.dead == true )
			{
				continue;
			}

			desc.handler->handleGlobalMouseButtonEventEnd( _touchId, _point, _button, _isDown );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::handleGlobalMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
        for( TVectorGlobalMouseHandler::size_type
            it = 0,
            it_end = m_globalMouseHandler.size();
        it != it_end;
        ++it)
        {
            const GlobalMouseHandleDesc & desc = m_globalMouseHandler[it];

            if( desc.dead == true )
            {
                continue;
            }

            desc.handler->handleGlobalMouseMove( _touchId, _point, _x, _y, _whell );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	size_t GlobalHandleSystem::addGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		GlobalMouseHandleDesc desc;

        size_t new_id = ++m_handlersEnumerator;
        desc.id = new_id;

		desc.handler = _handler;
		desc.dead = false;

		m_globalMouseHandler.push_back( desc );

        return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	GlobalMouseHandler * GlobalHandleSystem::removeGlobalMouseEventable( size_t _id )
	{
        TVectorGlobalMouseHandler::iterator it_found = std::find_if( m_globalMouseHandler.begin(), m_globalMouseHandler.end(), FFindMouseHandler(_id) );

        if( it_found == m_globalMouseHandler.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("GlobalHandleSystem::removeGlobalMouseEventable not found %d"
                , _id
                );

            return NULL;
        }
        
		it_found->dead = true;

        GlobalMouseHandler * handler = it_found->handler;

        return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t GlobalHandleSystem::addGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		GlobalKeyHandleDesc desc;

        size_t new_id = ++m_handlersEnumerator;
        desc.id = new_id;

		desc.handler = _handler;
		desc.dead = false;

		m_globalKeyHandler.push_back( desc );

        return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	GlobalKeyHandler * GlobalHandleSystem::removeGlobalKeyEventable( size_t _id )
	{
        TVectorGlobalKeyHandler::iterator it_found = std::find_if( m_globalKeyHandler.begin(), m_globalKeyHandler.end(), FFindKeyHandler(_id) );

        if( it_found == m_globalKeyHandler.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("GlobalHandleSystem::removeGlobalMouseEventable not found %d"
                , _id
                );

            return NULL;
        }

        it_found->dead = true;

        GlobalKeyHandler * handler = it_found->handler;

        return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::update()
	{
        if( m_globalMouseHandler.empty() == false )
        {
            TVectorGlobalMouseHandler::iterator it_mouse_erase = std::remove_if( m_globalMouseHandler.begin(), m_globalMouseHandler.end(), FDeadMouseHandler());
            m_globalMouseHandler.erase( it_mouse_erase, m_globalMouseHandler.end() );
        }

        if( m_globalKeyHandler.empty() == false )
        {
            TVectorGlobalKeyHandler::iterator it_key_erase = std::remove_if( m_globalKeyHandler.begin(), m_globalKeyHandler.end(), FDeadKeyHandler());
            m_globalKeyHandler.erase( it_key_erase, m_globalKeyHandler.end() );
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void GlobalHandleSystem::clear()
    {
        if( m_globalMouseHandler.empty() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("GlobalHandleSystem::clear global mouse handlers is not empty"
                );
        }

        if( m_globalKeyHandler.empty() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("GlobalHandleSystem::clear global key handlers is not empty"
                );
        }
    }

}
