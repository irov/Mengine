#	include "MousePickerSystem.h"
#	include "MousePickerTrap.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "Arrow.h"
#	include "Player.h"

#	include <algorithm>

const unsigned int VECTOR_CAPACITY = 100;

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MousePickerSystem::MousePickerSystem()
	{
		m_listPickerTrap.reserve( VECTOR_CAPACITY );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update( HotSpot* _picker )
	{
		for( TVectorRegEvent::iterator it = m_registration.begin(), it_end = m_registration.end();
			it != it_end;
			it++ )
		{
			if( it->reg )
			{
				addTrap_( it->trap );
			}
			else
			{
				delTrap_( it->trap );
			}
		}
		m_registration.clear();


		TVectorPickerTrap pickTraps = MousePickerSystem::pickTrap( _picker );


		for( TVectorPickerTrap::iterator
			it = m_lastTraps.begin(),
			it_end = m_lastTraps.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * lastTrap = *it;

			TVectorPickerTrap::iterator it_find = std::find( pickTraps.begin(), pickTraps.end(), lastTrap );

			if( it_find == pickTraps.end() )
			{
				if( lastTrap->_pickerActive() )
				{
					lastTrap->onLeave();
				}
			}
		}

		for( TVectorPickerTrap::iterator
			it = pickTraps.begin(),
			it_end = pickTraps.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * nowTrap = *it;

			TVectorPickerTrap::iterator it_find = std::find( m_lastTraps.begin(), m_lastTraps.end(), nowTrap );

			if( it_find == m_lastTraps.end() )
			{
				nowTrap->onEnter();
			}
		}

		m_lastTraps.assign( pickTraps.begin(), pickTraps.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::clear()
	{
		m_listPickerTrap.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::reset()
	{
		m_lastTraps.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::regTrap( MousePickerTrap * _trap )
	{
		//m_listPickerTrap.push_back( _trap );
		//m_registrating.push_back( _trap );
		m_registration.push_back( TRegEvent( _trap, true ) );
	}
	//////////////////////////////////////////////////////////////////////////
	TVectorPickerTrap MousePickerSystem::pickTrap( HotSpot * _hotspot ) 
	{
		TVectorPickerTrap pickTraps;

		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;

			if( trap->_pickerActive() == true && trap->pick( _hotspot ) == true )
			{
				pickTraps.push_back( trap );
			}
		}

		return pickTraps;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleKeyEvent( HotSpot* _picker, unsigned int _key, unsigned int _char, bool _isDown )
	{
		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;
			//if( trap->_pickerActive() == true &&  trap->pick( _picker ) == true )
			if( ( trap->isPicked() && trap->_pickerActive() ) == true )
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
		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;
			//if( trap->_pickerActive() == true &&  trap->pick( _picker ) == true )
			if( ( trap->isPicked() && trap->_pickerActive() ) == true )
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
		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;
			//if( trap->_pickerActive() == true && trap->pick( _picker ) == true )
			if( ( trap->isPicked() && trap->_pickerActive() ) == true )
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
	void MousePickerSystem::unregTrap( MousePickerTrap* _trap )
	{
		m_registration.push_back( TRegEvent( _trap, false ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::addTrap_( MousePickerTrap* _trap )
	{
		m_listPickerTrap.push_back( _trap );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::delTrap_( MousePickerTrap* _trap )
	{
		TVectorPickerTrap::iterator it_find = std::find( m_listPickerTrap.begin(), m_listPickerTrap.end(), _trap );
		if( it_find != m_listPickerTrap.end() )
		{
			m_listPickerTrap.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
