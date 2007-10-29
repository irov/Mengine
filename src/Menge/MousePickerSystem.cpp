#	include "MousePickerSystem.h"
#	include "MousePickerTrap.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "Player.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MousePickerSystem::MousePickerSystem()
		: m_lastPickerTrap(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update()
	{
		const mt::vec2f & mp = Holder<Player>::hostage()
			->getPositionClick();

		MousePickerTrap * picker = MousePickerSystem::pickTrap( mp.x, mp.y );

		if( m_lastPickerTrap != picker )
		{
			if( m_lastPickerTrap )
			{
				if( m_lastPickerTrap->isActive() )
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
	MousePickerTrap * MousePickerSystem::pickTrap( float _x, float _y )
	{
		for each( const PickerTrap & picker in m_listPickerTrap )
		{
			MousePickerTrap * trap = picker.trap;

			const Viewport & vp = picker.viewport;
		
			float picker_x = vp.begin.x + _x;
			float picker_y = vp.begin.y + _y;

			if( trap->pick( picker_x, picker_y ) == true )
			{
				return trap;
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleKeyEvent( size_t _key, bool _isDown )
	{
		const mt::vec2f & mp = Holder<Player>::hostage()
			->getPositionClick();

		for each( const PickerTrap & picker in m_listPickerTrap )
		{
			MousePickerTrap * trap = picker.trap;

			const Viewport & vp = picker.viewport;

			float picker_x = vp.begin.x + mp.x;
			float picker_y = vp.begin.y + mp.y;

			if( trap->pick( picker_x, picker_y ) == true )
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
		const mt::vec2f & mp = Holder<Player>::hostage()
			->getPositionClick();

		for each( const PickerTrap & picker in m_listPickerTrap )
		{
			MousePickerTrap * trap = picker.trap;

			const Viewport & vp = picker.viewport;

			float picker_x = vp.begin.x + mp.x;
			float picker_y = vp.begin.y + mp.y;

			if( trap->pick( picker_x, picker_y ) == true )
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
		const mt::vec2f & mp = Holder<Player>::hostage()
			->getPositionClick();

		for each( const PickerTrap & picker in m_listPickerTrap )
		{
			MousePickerTrap * trap = picker.trap;

			const Viewport & vp = picker.viewport;

			float picker_x = vp.begin.x + mp.x;
			float picker_y = vp.begin.y + mp.y;

			if( trap->pick( picker_x, picker_y ) == true )
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
	InputHandler * MousePickerSystem::pickHandler( float _x, float _y )
	{
		MousePickerTrap * picker = 
			this->pickTrap( _x, _y );

		if( picker )
		{
			return picker->handler();
		}

		return 0;
	}
}