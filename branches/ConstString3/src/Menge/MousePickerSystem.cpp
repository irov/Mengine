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
	MousePickerSystem::MousePickerSystem()
		: m_enumerator(0)
		, m_block(false)
		, m_arrow(0)
	{
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
		const mt::vec2f & pos = InputEngine::get()
			->getCursorPosition();

		if( InputEngine::get()
			->validCursorPosition( pos ) == true )
		{
			this->execReg_();
			this->updatePicked_( pos );
			this->updateDead_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps )
	{
		this->updatePicked_( _point );

		for( TPickerTrapState::reverse_iterator
			it = m_pickerTrapState.rbegin(),
			it_end = m_pickerTrapState.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = &*it;

			if( state->dead == true )
			{
				continue;
			}

			if( state->picked == true )
			{
				MousePickerTrap * trap = state->trap;

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
	PickerTrapState * MousePickerSystem::regTrap( MousePickerTrap * _trap )
	{
		size_t id = ++m_enumerator;

		PickerTrapState state;

		state.trap = _trap;
		state.id = id;
		state.picked = false;
		state.handle = false;
		state.dead = false;

		m_pickerTrapState.push_back( state );
		PickerTrapState & refState = m_pickerTrapState.back();

		m_registration.push_back( &refState );

		return &refState;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::unregTrap( PickerTrapState * _ref )
	{
		for( TPickerTrapState::iterator
			it = m_pickerTrapState.begin(),
			it_end = m_pickerTrapState.end();
		it != it_end;
		++it)
		{
			if( it->id == _ref->id )
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

		for( TPickerTrapRef::iterator
			it = m_registration.begin(),
			it_end = m_registration.end();
		it != it_end;
		++it)
		{
			PickerTrapState * state = *it;

			if( state->id == _ref->id )
			{
				m_registration.erase( it );

				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::beginTrap()
	{
		m_process.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updateTrap( PickerTrapState * _ref )
	{
		m_process.push_back( _ref );
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		this->execReg_();
		this->updatePicked_(_point);
		this->updateDead_();

		for( TPickerTrapRef::reverse_iterator
			it = m_process.rbegin(),
			it_end = m_process.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = *it;

			if( state->dead == true )
			{
				continue;
			}
			
			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrap * trap = state->trap;

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
		this->updateDead_();

		for( TPickerTrapRef::reverse_iterator
			it = m_process.rbegin(),
			it_end = m_process.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = *it;

			if( state->dead == true )
			{
				continue;
			}
			
			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrap * trap = state->trap;

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
		this->updateDead_();

		for( TPickerTrapRef::reverse_iterator
			it = m_process.rbegin(),
			it_end = m_process.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = *it;

			if( state->dead == true )
			{
				continue;
			}

			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrap * trap = state->trap;

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
		this->updateDead_();

		for( TPickerTrapRef::reverse_iterator
			it = m_process.rbegin(),
			it_end = m_process.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = *it;

			if( state->dead == true )
			{
				continue;
			}

			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrap * trap = state->trap;

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
		this->updateDead_();

		for( TPickerTrapRef::reverse_iterator
			it = m_process.rbegin(),
			it_end = m_process.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = *it;

			if( state->dead == true )
			{
				continue;
			}
						
			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrap * trap = state->trap;

				if( trap->handleMouseMove( _point, _x, _y, _whell ) == true )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::handleMouseEnter( const mt::vec2f & _point )
	{
		if( InputEngine::get()
			->validCursorPosition( _point ) == false )
		{
			return;
		}

		this->execReg_();
		this->updatePicked_( _point );
		this->updateDead_();		
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::handleMouseLeave()
	{
		if( m_arrow == NULL )
		{
			return;
		}

		//bool handle = false;

		for( TPickerTrapRef::reverse_iterator
			it = m_process.rbegin(),
			it_end = m_process.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = *it;

			if( state->dead == true )
			{
				continue;
			}

			MousePickerTrap * trap = state->trap;

			//if( handle == false && m_block == false && trap->_pickerActive() == true )
			//{
			//}
			//else
			//{

			if( state->picked == true )
			{
				state->picked = false;

				trap->onMouseLeave();
			}
			//}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::execReg_()
	{
		updateDead_();

		m_process.insert( m_process.end(), m_registration.begin(), m_registration.end() );

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

		for( TPickerTrapRef::reverse_iterator
			it = m_process.rbegin(),
			it_end = m_process.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = *it;

			if( state->dead == true )
			{
				continue;
			}

			MousePickerTrap * trap = state->trap;

			if( handle == false && m_block == false && trap->_pickerActive() == true && trap->pick( _point, m_arrow ) == true )
			{
				if( state->picked == false )
				{
					handle = trap->onMouseEnter();

					state->picked = true;					
					state->handle = handle;
				}
				else
				{
					handle = state->handle;
				}
			}
			else
			{
				if( state->picked == true )
				{
					state->picked = false;
					trap->onMouseLeave();
				}
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updateDead_()
	{
		for( TPickerTrapRef::iterator
			it = m_process.begin();
			it != m_process.end(); )
		{
			PickerTrapState * state = *it;

			if( state->dead == true )
			{
				it = m_process.erase( it );
			}
			else
			{
				++it;
			}
		}

		for( TPickerTrapState::iterator
			it = m_pickerTrapState.begin();
			it != m_pickerTrapState.end(); )
		{
			if( it->dead == true )
			{
				it = m_pickerTrapState.erase( it );
			}
			else
			{
				++it;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::isPicked( const PickerTrapState * _state )
	{
		if( _state->dead == true )
		{
			return false;
		}

		if( _state->picked == false )
		{
			return false;
		}

		if(  _state->trap->_pickerActive() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MousePickerSystem::getPickerTrapCount() const
	{
		return m_pickerTrapState.size();
	}
}
