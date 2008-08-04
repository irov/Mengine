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
	void MousePickerSystem::update()
	{
		Arrow * arrow = Holder<Player>::hostage()->getArrow();

		HotSpot * hotSpot = arrow->getCurrentHotSpot();

		TVectorPickerTrap pickTraps = MousePickerSystem::pickTrap( hotSpot );


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
	void MousePickerSystem::regTrap( MousePickerTrap * _trap, const Viewport & _viewport )
	{
		m_listPickerTrap.push_back( _trap );
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
	bool MousePickerSystem::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		Arrow * arrow = Holder<Player>::hostage()->getArrow();

		HotSpot * hotspot = arrow->getCurrentHotSpot();

		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;
			if( trap->_pickerActive() == true &&  trap->pick( hotspot ) == true )
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
	bool MousePickerSystem::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		Arrow * arrow = Holder<Player>::hostage()->getArrow();

		HotSpot * hotspot = arrow->getCurrentHotSpot();

		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;
			if( trap->_pickerActive() == true &&  trap->pick( hotspot ) == true )
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
	bool MousePickerSystem::handleMouseMove( float _x, float _y, int _whell )
	{
		Arrow * arrow = Holder<Player>::hostage()->getArrow();

		HotSpot * hotspot = arrow->getCurrentHotSpot();

		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;
			if( trap->_pickerActive() == true && trap->pick( hotspot ) == true )
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
}
