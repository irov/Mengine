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
		void unregTrap( MousePickerTrap* _trap );
		TVectorPickerTrap pickTrap( HotSpot * _hotspot );

		bool handleKeyEvent( HotSpot* _picker, unsigned int _key, unsigned int _char, bool _isDown );
		bool handleMouseButtonEvent( HotSpot* _picker, unsigned int _button, bool _isDown );
		bool handleMouseMove( HotSpot* _picker, float _x, float _y, int _whell );

	private:
		void updatePicked( HotSpot * _hotspot );

	private:
		TVectorPickerTrap m_picked;
		TVectorPickerTrap m_listPickerTrap;
		
		struct TRegEvent
		{
			MousePickerTrap* trap;
			bool reg;
			TRegEvent( MousePickerTrap* _trap, bool _reg )
				: trap( _trap )
				, reg( _reg )
			{
			}
		};
		typedef std::vector<TRegEvent> TVectorRegEvent;
		TVectorRegEvent m_registration;
		void execReg_();
		void addTrap_( MousePickerTrap* _trap );
		void delTrap_( MousePickerTrap* _trap );
	};
}
