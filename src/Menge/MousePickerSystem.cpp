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
		const mt::vec2f & mp = Holder<Player>::hostage()
			->getPositionClick();

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
	void MousePickerSystem::regTrap( MousePickerTrap * _trap, const Viewport & _viewport )
	{
		PickerTrap pt;

		pt.trap = _trap;
		pt.viewport = _viewport;

		m_listPickerTrap.push_front( pt );
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrap * MousePickerSystem::pickTrap( HotSpot * _hotspot ) 
	{
		for each( const PickerTrap & picker in m_listPickerTrap )
		{
			MousePickerTrap * trap = picker.trap;

			const Viewport & vp = picker.viewport;
		
			if( trap->pick( vp.begin, _hotspot ) == true )
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

		for each( const PickerTrap & picker in m_listPickerTrap )
		{
			MousePickerTrap * trap = picker.trap;

			const Viewport & vp = picker.viewport;

			if( trap->pick( vp.begin, hotspot ) == true )
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

		for each( const PickerTrap & picker in m_listPickerTrap )
		{
			MousePickerTrap * trap = picker.trap;

			const Viewport & vp = picker.viewport;

			if( trap->pick( vp.begin, hotspot ) == true )
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

		for each( const PickerTrap & picker in m_listPickerTrap )
		{
			MousePickerTrap * trap = picker.trap;

			const Viewport & vp = picker.viewport;

			if( trap->pick( vp.begin, hotspot ) == true )
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