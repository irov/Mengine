#	include "MousePickerSystem.h"
#	include "MousePickerTrap.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "Arrow.h"
#	include "Player.h"

#	include "LogEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct MousePickerSystem::PickerFinder
	{
		std::size_t m_id;

		PickerFinder( std::size_t _id )
			: m_id(_id)
		{
		}

		bool operator()( const PickerTrapState& _pickerState )
		{
			return _pickerState.id == m_id;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	MousePickerSystem::MousePickerSystem()
		: m_enumerator(0)
		, m_block(false)
		, m_arrow(0)
	{
		const unsigned int PickerTrapCapacity = 100;

		m_listPickerTrap.reserve( PickerTrapCapacity );
		
		m_trapIterator = m_listPickerTrap.begin();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::block( bool _value )
	{
		m_block = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setArrow( Arrow * _arrow )
	{
		m_arrow = _arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update()
	{		
		if( this->execReg_() == true )
		{
			const mt::vec2f & pos = InputEngine::get()
				->getCursorPosition();

			this->updatePicked_( pos );
		}

		m_trapIterator = m_listPickerTrap.begin();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps )
	{
		this->execReg_();

		this->updatePicked_( _point );

		for( TVectorPickerTrapState::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				continue;
			}

			if( it->picked == true )
			{
				MousePickerTrap * trap = it->trap;

				_traps.push_back( trap );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::clear()
	{
		this->updateDead_();
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t MousePickerSystem::regTrap( MousePickerTrap * _trap )
	{
		std::size_t id = ++m_enumerator;

		PickerTrapState state;

		state.trap = _trap;
		state.id = id;
		state.picked = false;
		state.handle = false;
		state.dead = false;

		m_registration.push_back( state );

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::unregTrap( std::size_t _id )
	{
		for( TVectorPickerTrapState::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			if( it->id == _id )
			{
				it->dead = true;
				//it->picked = false;

				if( it->picked == true )
				{
					it->picked = false;
					it->trap->onMouseLeave();
				}

				break;
			}
		}

		for( TVectorPickerTrapState::iterator
			it = m_registration.begin(),
			it_end = m_registration.end();
		it != it_end;
		++it)
		{
			if( it->id == _id )
			{
				m_registration.erase( it );

				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updateTrap( std::size_t _id )
	{
		PickerTrapState& trapState = (*m_trapIterator);

		TVectorPickerTrapState::iterator it_end = m_listPickerTrap.end();

		if( trapState.id != _id )
		{			
			TVectorPickerTrapState::iterator it_find = 
				std::find_if( m_trapIterator, it_end, PickerFinder( _id ) );

			if( it_find != it_end )
			{
				std::swap( (*m_trapIterator), (*it_find) );
			}
		}

		if( m_trapIterator != it_end )
		{
			++m_trapIterator;
		}

		if( m_trapIterator == it_end )
		{
			m_trapIterator = m_listPickerTrap.begin();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		this->execReg_();
		this->updatePicked_(_point);

		for( TVectorPickerTrapState::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				continue;
			}
			
			if( MousePickerSystem::isPicked( *it ) == true )
			{
				MousePickerTrap * trap = it->trap;

				if( trap->handleKeyEvent( _point, _key, _char, _isDown ) == true )
				{
					return true;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		this->execReg_();
		this->updatePicked_(_point);

		for( TVectorPickerTrapState::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				continue;
			}
			
			if( MousePickerSystem::isPicked( *it ) == true )
			{
				MousePickerTrap * trap = it->trap;

				if( trap->handleMouseButtonEvent( _point, _button, _isDown ) == true )
				{
					return true;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		this->execReg_();
		this->updatePicked_(_point);

		for( TVectorPickerTrapState::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				continue;
			}

			if( MousePickerSystem::isPicked( *it ) == true )
			{
				MousePickerTrap * trap = it->trap;

				trap->handleMouseButtonEventBegin( _point, _button, _isDown );
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		this->execReg_();
		this->updatePicked_( _point );

		for( TVectorPickerTrapState::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				continue;
			}

			if( MousePickerSystem::isPicked( *it ) == true )
			{
				MousePickerTrap * trap = it->trap;

				trap->handleMouseButtonEventEnd( _point, _button, _isDown );
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		this->execReg_();
		this->updatePicked_(_point);

		for( TVectorPickerTrapState::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				continue;
			}
						
			if( MousePickerSystem::isPicked( *it ) == true )
			{
				MousePickerTrap * trap = it->trap;

				if( trap->handleMouseMove( _point, _x, _y, _whell ) == true )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::execReg_()
	{
		updateDead_();

		if( m_registration.empty() == true )
		{
			return false;
		}

		m_listPickerTrap.insert( m_listPickerTrap.end(), m_registration.begin(), m_registration.end() );

		m_registration.clear();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updatePicked_( const mt::vec2f& _point )
	{
		if( m_arrow == NULL )
		{
			return;
		}

		bool handle = false;

		for( TVectorPickerTrapState::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				continue;
			}

			MousePickerTrap * trap = it->trap;

			if( handle == false && m_block == false && trap->_pickerActive() == true && trap->pick( _point, m_arrow ) == true )
			{
				if( it->picked == false )
				{
					handle = trap->onMouseEnter();

					it->picked = true;					
					it->handle = handle;
				}
				else
				{
					handle = it->handle;
				}
			}
			else
			{
				if( it->picked == true )
				{
					it->picked = false;
					trap->onMouseLeave();
				}
			}
		}

		this->updateDead_();		
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updateDead_()
	{
		for( TVectorPickerTrapState::iterator
			it = m_listPickerTrap.begin();
			it != m_listPickerTrap.end(); )
		{
			if( it->dead == true )
			{
				it = m_listPickerTrap.erase( it );
			}
			else
			{
				++it;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::isPicked( const PickerTrapState & _state )
	{
		if( _state.dead == true )
		{
			return false;
		}

		if( _state.picked == false )
		{
			return false;
		}

		if(  _state.trap->_pickerActive() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
