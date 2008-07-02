#	pragma once

#	include "Viewport.h"

#	include <vector>

namespace Menge
{
	class MousePickerTrap;
	class InputHandler;
	class HotSpot;

	class MousePickerSystem
	{
	public:
		MousePickerSystem();

	public:
		void update();
		void clear();
		void reset();

		void regTrap( MousePickerTrap * _trap, const Viewport & _viewport );
		MousePickerTrap * pickTrap( HotSpot * _hotspot );

		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown );
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown );
		bool handleMouseMove( float _x, float _y, int _whell );

	private:
		typedef std::vector<MousePickerTrap *> TVectorPickerTrap;
		TVectorPickerTrap m_listPickerTrap;

		MousePickerTrap * m_lastPickerTrap;
	};
}