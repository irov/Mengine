#	include "GlobalHandleSystem.h"

#	include "InputHandler.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalKeyHandler::iterator
				it = m_setGlobalKeyHandler.begin(),
				it_end = m_setGlobalKeyHandler.end();
			it != it_end;
			/*++it*/)
			{
				if( handle = (*it++)->handleGlobalKeyEvent( _key, _char, _isDown ) )
				{
					break;
				}
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			/*++it*/)
			{
				if( handle = (*it++)->handleGlobalMouseButtonEvent( _button, _isDown ) )
				{
					break;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		for( TSetGlobalMouseHandler::iterator
			it = m_setGlobalMouseHandler.begin(),
			it_end = m_setGlobalMouseHandler.end();
		it != it_end;
		/*++it*/)
		{
			(*it++)->handleGlobalMouseButtonEvent( _button, _isDown );
		}

		return false;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			/*++it*/)
			{
				if( handle = (*it++)->handleGlobalMouseMove( _x, _y, _whell ) )
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
		TSetGlobalMouseHandler::iterator it_find = std::find( m_setGlobalMouseHandler.begin(), m_setGlobalMouseHandler.end(), _handler );
		if( it_find == m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.push_back( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::unregGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = std::find( m_setGlobalMouseHandler.begin(), m_setGlobalMouseHandler.end(), _handler );
		if( it_find != m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::regGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = std::find( m_setGlobalKeyHandler.begin(), m_setGlobalKeyHandler.end(), _handler );
		if( it_find == m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.push_back( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::unregGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = std::find( m_setGlobalKeyHandler.begin(), m_setGlobalKeyHandler.end(), _handler );
		if( it_find != m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::clear()
	{
		m_setGlobalMouseHandler.clear();
		m_setGlobalKeyHandler.clear();
	}
}
