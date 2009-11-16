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
		void clear();
		
		std::size_t regTrap( MousePickerTrap * _trap );
		void unregTrap( std::size_t _id );

		bool handleKeyEvent( HotSpot* _picker, unsigned int _key, unsigned int _char, bool _isDown );
		bool handleMouseButtonEvent( HotSpot* _picker, unsigned int _button, bool _isDown );
		bool handleMouseMove( HotSpot* _picker, float _x, float _y, int _whell );

	private:
		void updatePicked_( HotSpot * _hotspot );
		void updateDead_();

		void execReg_();

	private:
		struct PickerTrapState
		{
			MousePickerTrap * trap;
			std::size_t id;
			bool picked;
			bool dead;
		};

		std::size_t m_enumerator;

		typedef std::vector<PickerTrapState> TVectorPickerTrap;
		TVectorPickerTrap m_listPickerTrap;
		TVectorPickerTrap m_registration;

	private:
		static bool isPicked( const PickerTrapState & _state );
	};
}
