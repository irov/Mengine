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
				m_lastPickerTrap->onLeave();
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
	void MousePickerSystem::regTrap( MousePickerTrap * _trap )
	{
		m_listPickerTrap.push_front( _trap );
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrap * MousePickerSystem::pickTrap( float _x, float _y )
	{
		for each( MousePickerTrap * trap in m_listPickerTrap )
		{
			if( trap->pick( _x, _y ) == true )
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

		for each( MousePickerTrap * trap in m_listPickerTrap )
		{
			if( trap->pick( mp.x, mp.y ) == true )
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

		for each( MousePickerTrap * trap in m_listPickerTrap )
		{
			if( trap->pick( mp.x, mp.y ) == true )
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

		for each( MousePickerTrap * trap in m_listPickerTrap )
		{
			if( trap->pick( mp.x, mp.y ) == true )
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