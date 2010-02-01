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
	{
		const unsigned int PickerTrapCapacity = 100;

		m_listPickerTrap.reserve( PickerTrapCapacity );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update( Arrow * _arrow )
	{
		execReg_();
		updatePicked_( _arrow );
		m_trapIterator = m_listPickerTrap.begin();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::clear()
	{
		updateDead_();
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
		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			if( it->id == _id )
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

		for( TVectorPickerTrap::iterator
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

		TVectorPickerTrap::iterator it_end = m_listPickerTrap.end();

		if( trapState.id != _id )
		{			
			TVectorPickerTrap::iterator it_find = 
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
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleKeyEvent( Arrow * _arrow, unsigned int _key, unsigned int _char, bool _isDown )
	{
		execReg_();
		updatePicked_( _arrow );

		for( TVectorPickerTrap::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;

			if( MousePickerSystem::isPicked( *it ) == true )
			{
				InputHandler * handler = trap->getInputHandler();

				if( handler->handleKeyEvent( _key, _char, _isDown ) == true )
				{
					return true;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEvent( Arrow * _arrow, unsigned int _button, bool _isDown )
	{
		execReg_();
		updatePicked_( _arrow );

		for( TVectorPickerTrap::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;

			if( MousePickerSystem::isPicked( *it ) == true )
			{
				InputHandler * handler = trap->getInputHandler();

				if( handler->handleMouseButtonEvent( _button, _isDown ) == true )
				{
					return true;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEventEnd( Arrow * _arrow, unsigned int _button, bool _isDown )
	{
		execReg_();
		updatePicked_( _arrow );

		for( TVectorPickerTrap::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;


			InputHandler * handler = trap->getInputHandler();

			handler->handleMouseButtonEventEnd( _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseMove( Arrow * _arrow, float _x, float _y, int _whell )
	{
		execReg_();
		updatePicked_( _arrow );

		for( TVectorPickerTrap::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;
			
			if( MousePickerSystem::isPicked( *it ) == true )
			{
				InputHandler * handler = trap->getInputHandler();

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
		if( m_registration.empty() == true )
		{
			return;
		}

		updateDead_();

		m_listPickerTrap.insert( m_listPickerTrap.end(), m_registration.begin(), m_registration.end() );

		m_registration.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updatePicked_( Arrow * _arrow )
	{
		bool handle = false;

		for( TVectorPickerTrap::reverse_iterator
			it = m_listPickerTrap.rbegin(),
			it_end = m_listPickerTrap.rend();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = it->trap;

			if( it->dead == true )
			{
				continue;
			}

			if( handle == false && trap->_pickerActive() == true && trap->pick( _arrow ) == true )
			{
				if( it->picked == false )
				{
					it->picked = true;
					handle = trap->onEnter();
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
					trap->onLeave();
				}
			}
		}

		updateDead_();
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
