#	pragma once

#	include "Viewport.h"

#	include <list>

namespace Menge
{
	class MousePickerTrap;
	class InputHandler;
	class HotSpot;

	class MousePickerSystem
	{
	public:
		MousePickerSystem();

		struct PickerTrap
		{
			MousePickerTrap * trap;
			Viewport viewport;
		};

	public:
		void update();
		void clear();

		void regTrap( MousePickerTrap * _trap, const Viewport & _viewport );
		MousePickerTrap * pickTrap( HotSpot * _hotspot );

		bool handleKeyEvent( size_t _key, bool _isDown );
		bool handleMouseButtonEvent( size_t _button, bool _isDown );
		bool handleMouseMove( int _x, int _y, int _whell );

	private:
		typedef std::list<PickerTrap> TListPickerTrap;
		TListPickerTrap m_listPickerTrap;

		MousePickerTrap * m_lastPickerTrap;
	};
}