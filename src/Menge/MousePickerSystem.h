#	pragma once

#	include "core/Holder.h"
#	include "Core/Viewport.h"

#	include <vector>

namespace Menge
{
	class MousePickerTrap;
	class InputHandler;
	class HotSpot;

	class MousePickerSystem
		: public Holder<MousePickerSystem>
	{
	public:
		MousePickerSystem();

	public:
		void update( HotSpot * _picker );
		void clear();
		
		std::size_t regTrap( MousePickerTrap * _trap );
		void unregTrap( std::size_t _id );
		void updateTrap( std::size_t _id );

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
			bool handle;
			bool dead;
		};

		struct PickerFinder
		{
			std::size_t m_id;

			PickerFinder( std::size_t _id )
				: m_id( _id )
			{
			}

			bool operator()( const PickerTrapState& _pickerState )
			{
				return _pickerState.id == m_id;
			}

		};

		std::size_t m_enumerator;

		typedef std::vector<PickerTrapState> TVectorPickerTrap;
		TVectorPickerTrap m_listPickerTrap;
		TVectorPickerTrap m_registration;
		TVectorPickerTrap::iterator m_trapIterator;

	private:
		static bool isPicked( const PickerTrapState & _state );
	};
}
