#	include "MousePickerSystem.h"
#	include "MousePickerTrap.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "Arrow.h"
#	include "Player.h"

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
			//	if( m_lastPickerTrap->isActivate() )
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
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::regTrap( MousePickerTrap * _trap, const Viewport & _viewport )
	{
		//m_listPickerTrap.push_front( _trap );
		m_listPickerTrap.push_back( _trap );
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrap * MousePickerSystem::pickTrap( HotSpot * _hotspot ) 
	{
		for( TListPickerTrap::iterator
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

		for( TListPickerTrap::iterator
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

		for( TListPickerTrap::iterator
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

		for( TListPickerTrap::iterator
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
