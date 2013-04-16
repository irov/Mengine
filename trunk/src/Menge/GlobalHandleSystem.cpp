#	include "GlobalHandleSystem.h"

#	include <algorithm>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    GlobalHandleSystem::GlobalHandleSystem( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalKeyHandler::iterator
				it = m_globalKeyHandler.begin(),
				it_end = m_globalKeyHandler.end();
			it != it_end;
			++it)
			{
				GlobalKeyDesc & desc = *it;

				if( desc.dead == true )
				{
					continue;
				}

                handle = desc.handler->handleGlobalKeyEvent( _point, _key, _char, _isDown );

				if( handle == true )
				{
					break;
				}
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_globalMouseHandler.begin(),
				it_end = m_globalMouseHandler.end();
			it != it_end;
			++it)
			{
				GlobalMouseDesc & desc = *it;

				if( desc.dead == true )
				{
					continue;
				}

                handle = desc.handler->handleGlobalMouseButtonEvent( _touchId, _point, _button, _isDown );

				if( handle == true )
				{
					break;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		for( TSetGlobalMouseHandler::iterator
			it = m_globalMouseHandler.begin(),
			it_end = m_globalMouseHandler.end();
		it != it_end;
		++it)
		{
			GlobalMouseDesc & desc = *it;

			if( desc.dead == true )
			{
				continue;
			}

			desc.handler->handleGlobalMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		}

		return false;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		for( TSetGlobalMouseHandler::iterator
			it = m_globalMouseHandler.begin(),
			it_end = m_globalMouseHandler.end();
		it != it_end;
		++it)
		{
			GlobalMouseDesc & desc = *it;

			if( desc.dead == true )
			{
				continue;
			}

			desc.handler->handleGlobalMouseButtonEventEnd( _touchId, _point, _button, _isDown );
		}

		return false;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_globalMouseHandler.begin(),
				it_end = m_globalMouseHandler.end();
			it != it_end;
			++it)
			{
				GlobalMouseDesc & desc = *it;

				if( desc.dead == true )
				{
					continue;
				}

                handle = desc.handler->handleGlobalMouseMove( _touchId, _point, _x, _y, _whell );

				if( handle == true )
				{
					break;
				}
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::regGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		GlobalMouseDesc desc;
		desc.handler = _handler;
		desc.dead = false;

		m_addGlobalMouseHandler.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	GlobalHandleSystem::TSetGlobalMouseHandler::iterator GlobalHandleSystem::s_findGlobalMouse( TSetGlobalMouseHandler & _set, GlobalMouseHandler * _handler )
	{
		for( TSetGlobalMouseHandler::iterator
			it = _set.begin(),
			it_end = _set.end();
		it != it_end;
		++it )
		{
			const GlobalMouseDesc & desc = *it;

			if( desc.handler == _handler )
			{
				return it;
			}
		}

		return _set.end();
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::unregGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = GlobalHandleSystem::s_findGlobalMouse( m_addGlobalMouseHandler, _handler );
		if( it_find != m_addGlobalMouseHandler.end() )
		{
			m_addGlobalMouseHandler.erase( it_find );
			return;
		}
		
		TSetGlobalMouseHandler::iterator it_dead = GlobalHandleSystem::s_findGlobalMouse( m_globalMouseHandler, _handler );

		if( it_dead == m_globalMouseHandler.end() )
		{
			return;
		}
		
		it_dead->dead = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::regGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		GlobalKeyDesc desc;
		desc.handler = _handler;
		desc.dead = false;

		m_addGlobalKeyHandler.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	GlobalHandleSystem::TSetGlobalKeyHandler::iterator GlobalHandleSystem::s_findGlobalKey( TSetGlobalKeyHandler & _set, GlobalKeyHandler * _handler )
	{
		for( TSetGlobalKeyHandler::iterator
			it = _set.begin(),
			it_end = _set.end();
		it != it_end;
		++it )
		{
			const GlobalKeyDesc & desc = *it;

			if( desc.handler == _handler )
			{
				return it;
			}
		}

		return _set.end();
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::unregGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = GlobalHandleSystem::s_findGlobalKey( m_addGlobalKeyHandler, _handler );

		if( it_find != m_addGlobalKeyHandler.end() )
		{
			m_addGlobalKeyHandler.erase( it_find );
			return;
		}

		TSetGlobalKeyHandler::iterator it_dead = GlobalHandleSystem::s_findGlobalKey( m_globalKeyHandler, _handler );

		if( it_dead == m_globalKeyHandler.end() )
		{
			return;
		}

		it_dead->dead = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::update()
	{
		for( TSetGlobalMouseHandler::iterator
			it = m_globalMouseHandler.begin(),
			it_end = m_globalMouseHandler.end();
		it != it_end; )
		{
			const GlobalMouseDesc & desc = *it;

			if( desc.dead == true )
			{
				m_globalMouseHandler.erase( it++ );
			}
			else
			{
				++it;
			}
		}

		m_globalMouseHandler.insert( m_globalMouseHandler.end(), m_addGlobalMouseHandler.begin(), m_addGlobalMouseHandler.end() );
		m_addGlobalMouseHandler.clear();

		for( TSetGlobalKeyHandler::iterator
			it = m_globalKeyHandler.begin(),
			it_end = m_globalKeyHandler.end();
		it != it_end; )
		{
			const GlobalKeyDesc & desc = *it;

			if( desc.dead == true )
			{
				m_globalKeyHandler.erase( it++ );
			}
			else
			{
				++it;
			}
		}
		
		m_globalKeyHandler.insert( m_globalKeyHandler.end(), m_addGlobalKeyHandler.begin(), m_addGlobalKeyHandler.end() );
		m_addGlobalKeyHandler.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::clear()
	{
		m_globalMouseHandler.clear();
		m_addGlobalMouseHandler.clear();

		m_globalKeyHandler.clear();
		m_addGlobalKeyHandler.clear();
	}
}
