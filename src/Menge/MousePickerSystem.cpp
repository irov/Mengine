#	include "MousePickerSystem.h"
#	include "MousePickerTrap.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "Arrow.h"
#	include "Player.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MousePickerSystem::MousePickerSystem()
		: m_lastPickerTrap(0)
	{}
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
				if( m_lastPickerTrap->isActivate() )
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
		m_listPickerTrap.push_front( _trap );
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
			if( trap->isActivate() == true && trap->pick( _hotspot ) == true )
			{
				return trap;
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleKeyEvent( size_t _key, bool _isDown )
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
			if( trap->isActivate() == true &&  trap->pick( hotspot ) == true )
			{
				InputHandler * handler = trap->handler();

				if( handler->handleKeyEvent( _key, _isDown ) == true )
				{
					return true;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEvent( size_t _button, bool _isDown )
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
			if( trap->isActivate() == true &&  trap->pick( hotspot ) == true )
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
	bool MousePickerSystem::handleMouseMove( int _x, int _y, int _whell )
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
			if( trap->isActivate() == true && trap->pick( hotspot ) == true )
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
