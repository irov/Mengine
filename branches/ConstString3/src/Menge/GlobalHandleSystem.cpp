#	include "GlobalHandleSystem.h"

#	include "InputHandler.h"

#	include <algorithm>

namespace Menge
{
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
				if( handle = (*it)->handleGlobalKeyEvent( _point, _key, _char, _isDown ) )
				{
					break;
				}
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown )
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
				if( handle = (*it)->handleGlobalMouseButtonEvent( _point, _button, _isDown ) )
				{
					break;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		for( TSetGlobalMouseHandler::iterator
			it = m_globalMouseHandler.begin(),
			it_end = m_globalMouseHandler.end();
		it != it_end;
		++it)
		{
			(*it)->handleGlobalMouseButtonEventBegin( _point, _button, _isDown );
		}

		return false;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		for( TSetGlobalMouseHandler::iterator
			it = m_globalMouseHandler.begin(),
			it_end = m_globalMouseHandler.end();
		it != it_end;
		++it)
		{
			(*it)->handleGlobalMouseButtonEventEnd( _point, _button, _isDown );
		}

		return false;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleSystem::handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell )
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
				if( handle = (*it)->handleGlobalMouseMove( _point, _x, _y, _whell ) )
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
		m_addGlobalMouseHandler.push_back( _handler );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::unregGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = std::find( m_addGlobalMouseHandler.begin(), m_addGlobalMouseHandler.end(), _handler );
		if( it_find != m_addGlobalMouseHandler.end() )
		{
			m_addGlobalMouseHandler.erase( it_find );
		}
		else
		{
			m_delGlobalMouseHandler.push_back( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::regGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		m_addGlobalKeyHandler.push_back( _handler );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::unregGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = std::find( m_addGlobalKeyHandler.begin(), m_addGlobalKeyHandler.end(), _handler );
		if( it_find != m_addGlobalKeyHandler.end() )
		{
			m_addGlobalKeyHandler.erase( it_find );
		}
		else
		{
			m_delGlobalKeyHandler.push_back( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		template<class C>
		class RemoveHandler
		{
		public:
			RemoveHandler( const C & _list )
				: m_list(_list)
			{
			}

		public:
			bool operator () ( typename C::value_type _handler )
			{
				typename C::const_iterator it_find = std::find( m_list.begin(), m_list.end(), _handler );
				return it_find != m_list.end();
			}

		protected:
			const C & m_list;
		};

		template<class C>
		void removeHandler( C & _left, const C & _right )
		{
			_left.remove_if( RemoveHandler<C>(_right) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::update()
	{
		m_globalMouseHandler.insert( m_globalMouseHandler.end(), m_addGlobalMouseHandler.begin(), m_addGlobalMouseHandler.end() );
		Helper::removeHandler( m_globalMouseHandler, m_delGlobalMouseHandler );

		m_addGlobalMouseHandler.clear();
		m_delGlobalMouseHandler.clear();

		m_globalKeyHandler.insert( m_globalKeyHandler.end(), m_addGlobalKeyHandler.begin(), m_addGlobalKeyHandler.end() );
		Helper::removeHandler( m_globalKeyHandler, m_delGlobalKeyHandler );
	
		m_addGlobalKeyHandler.clear();
		m_delGlobalKeyHandler.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleSystem::clear()
	{
		m_globalMouseHandler.clear();
		m_addGlobalMouseHandler.clear();
		m_delGlobalMouseHandler.clear();

		m_globalKeyHandler.clear();
		m_addGlobalKeyHandler.clear();
		m_delGlobalKeyHandler.clear();
	}
}
