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
		: m_lastPickerTrap(0)
	{
		m_listPickerTrap.reserve( VECTOR_CAPACITY );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update()
	{
		Arrow * arrow = Holder<Player>::hostage()->getArrow();

		HotSpot * hotSpot = arrow->getCurrentHotSpot();

		MousePickerTrap * picker = MousePickerSystem::pickTrap( hotSpot );

		if( m_lastPickerTrap != picker )
		{
			if( m_lastPickerTrap )
			{
				if( m_lastPickerTrap->_pickerActive() )
				{
					m_lastPickerTrap->onLeave();
				}
			}

			if( picker )
			{
				picker->onEnter();
			}
		}

		m_lastPickerTrap = picker;
		/*Arrow * arrow = Holder<Player>::hostage()->getArrow();

		HotSpot * hotspot = arrow->getCurrentHotSpot();

		TVectorPickerTrap picked;

		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;
			if( trap->_pickerActive() == true && trap->pick( hotspot ) == true )
			{
				//InputHandler * handler = trap->handler();
				
				picked.push_back( trap );
			}
		}

		TVectorPickerTrap enterPicks;
		if( !m_lastTraps.empty() )
		{
			for( TVectorPickerTrap::iterator it = picked.begin(), it_end = picked.end();
				it != it_end;
				it++ )
			{
				m_lastTraps.erase( std::remove( m_lastTraps.begin(), m_lastTraps.end(), (*it) ) );
				enterPicks.push_back( (*it) );
			}
			for( TVectorPickerTrap::iterator it = m_lastTraps.begin(), it_end = m_lastTraps.end();
				it != it_end;
				it++ )
			{
				(*it)->onLeave();
			}
		}
		m_lastTraps.assign( picked.begin(), picked.end() );

		if( !picked.empty() )
		{
			for( TVectorPickerTrap::iterator it = enterPicks.begin(), it_end = enterPicks.end();
				it != it_end;
				it++ )
			{
				picked.erase( std::remove( picked.begin(), picked.end(), (*it) ) );
			}

			for( TVectorPickerTrap::iterator it = picked.begin(), it_end = picked.end();
				it != it_end;
				it++ )
			{
				(*it)->onEnter();
			}

		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::clear()
	{
		m_listPickerTrap.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::reset()
	{
		m_lastPickerTrap = NULL;
		//m_lastTraps.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::regTrap( MousePickerTrap * _trap, const Viewport & _viewport )
	{
		m_listPickerTrap.push_back( _trap );
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrap * MousePickerSystem::pickTrap( HotSpot * _hotspot ) 
	{
		for( TVectorPickerTrap::iterator
			it = m_listPickerTrap.begin(),
			it_end = m_listPickerTrap.end();
		it != it_end;
		++it)
		{
			MousePickerTrap * trap = *it;

			if( trap->_pickerActive() == true && trap->pick( _hotspot ) == true )
			{
				return trap;
			}
		}

		return 0;
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
		int i = 0;
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
