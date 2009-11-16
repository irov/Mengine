#	include "MousePickerSystem.h"
#	include "MousePickerTrap.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "Arrow.h"
#	include "Player.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MousePickerSystem::MousePickerSystem()
	{
		const unsigned int PickerTrapCapacity = 100;

		m_listPickerTrap.reserve( PickerTrapCapacity );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update( HotSpot* _picker )
	{
		if( m_registration.empty() == false )
		{
			execReg_();
		}

		updateDead_();
		updatePicked_( _picker, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::leave( HotSpot * _picker )
	{
		if( m_registration.empty() == false )
		{
			execReg_();
		}

		updatePicked_( _picker, true );
		updateDead_();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::regTrap( MousePickerTrap * _trap )
	{
		m_registration.push_back( _trap );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::unregTrap( MousePickerTrap* _trap )
	{
		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			if( it->trap == _trap )
			{
				it->dead = true;

				if( it->picked == true )
				{
					it->picked = false;
					it->trap->onLeave();
				}

				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleKeyEvent( HotSpot* _picker, unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( m_registration.empty() == false )
		{
			execReg_();
		}

		updatePicked_( _picker, false );

		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;

			if( MousePickerSystem::isPicked( *it ) == true )
			{
				InputHandler * handler = trap->handler();

				if( handler->handleKeyEvent( _key, _char, _isDown ) == true )
				{
					return true;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEvent( HotSpot* _picker, unsigned int _button, bool _isDown )
	{
		if( m_registration.empty() == false )
		{
			execReg_();
		}

		updatePicked_( _picker, false );

		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;

			if( MousePickerSystem::isPicked( *it ) == true )
			{
				InputHandler * handler = trap->handler();

				if( handler->handleMouseButtonEvent( _button, _isDown ) == true )
				{
					return true;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseMove( HotSpot* _picker, float _x, float _y, int _whell )
	{
		if( m_registration.empty() == false )
		{
			execReg_();
		}

		updatePicked_( _picker, false );

		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;
			
			if( MousePickerSystem::isPicked( *it ) == true )
			{
				InputHandler * handler = trap->handler();

				if( handler->handleMouseMove( _x, _y, _whell ) == true )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::execReg_()
	{
		for( TVectorRegEvent::iterator 
			it = m_registration.begin(), 
			it_end = m_registration.end();
		it != it_end;
		++it )
		{
			PickerTrapState state;

			state.trap = *it;
			state.picked = false;
			state.dead = false;

			m_listPickerTrap.push_back( state );
		}

		m_registration.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updatePicked_( HotSpot * _hotspot, bool _leaved )
	{
		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;

			if( it->dead == true )
			{
				continue;
			}

			if( _leaved == true && it->picked == true )
			{
				it->picked = false;
				trap->onLeave();

				continue;
			}

			if( it->dead == true )
			{
				continue;
			}

			if( trap->_pickerActive() == false )
			{
				continue;
			}

			if( trap->pick( _hotspot ) == true )
			{
				if( it->picked == false )
				{
					it->picked = true;
					trap->onEnter();
				}
			}
			else
			{
				if( it->picked == true )
				{
					it->picked = false;
					trap->onLeave();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updateDead_()
	{
		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin();
			it != m_listPickerTrap.end(); )
		{
			if( it->dead == true )
			{
				it->picked = false;
				it->trap->onLeave();

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
