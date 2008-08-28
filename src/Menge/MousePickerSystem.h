#	pragma once

#	include "Viewport.h"

#	include <vector>

namespace Menge
{
	class MousePickerTrap;
	class InputHandler;
	class HotSpot;

	typedef std::vector<MousePickerTrap *> TVectorPickerTrap;

	class MousePickerSystem
	{
	public:
		MousePickerSystem();

	public:
		void update( HotSpot* _picker );
		void clear();
		void reset();

		void regTrap( MousePickerTrap * _trap );
		TVectorPickerTrap pickTrap( HotSpot * _hotspot );

		bool handleKeyEvent( HotSpot* _picker, unsigned int _key, unsigned int _char, bool _isDown );
		bool handleMouseButtonEvent( HotSpot* _picker, unsigned int _button, bool _isDown );
		bool handleMouseMove( HotSpot* _picker, float _x, float _y, int _whell );

	private:
		TVectorPickerTrap m_listPickerTrap;

		//TVectorPickerTrap m_lastPickerTrap;
		TVectorPickerTrap m_lastTraps;
	};
}