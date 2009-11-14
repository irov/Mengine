#	pragma once

#	include "Core/Viewport.h"

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
		void update( HotSpot * _picker );
		void leave( HotSpot * _picker );
		
		void regTrap( MousePickerTrap * _trap );
		void unregTrap( MousePickerTrap* _trap );

		bool handleKeyEvent( HotSpot* _picker, unsigned int _key, unsigned int _char, bool _isDown );
		bool handleMouseButtonEvent( HotSpot* _picker, unsigned int _button, bool _isDown );
		bool handleMouseMove( HotSpot* _picker, float _x, float _y, int _whell );

	private:
		void updatePicked_( HotSpot * _hotspot, bool _leaved );
		void updateDead_();

		void execReg_();

	private:
		struct PickerTrapState
		{
			MousePickerTrap * trap;
			bool picked;
			bool dead;
		};

		typedef std::vector<PickerTrapState> TVectorPickerTrap;
		TVectorPickerTrap m_listPickerTrap;
		
		typedef std::vector<MousePickerTrap*> TVectorRegEvent;
		TVectorRegEvent m_registration;

	private:
		static bool isPicked( const PickerTrapState & _state );
	};
}
