#	pragma once

#	include "Core/Holder.h"
#	include "Core/Viewport.h"

#	include <vector>

namespace Menge
{
	class MousePickerTrap;
	class InputHandler;

	class Arrow;

	typedef std::vector<MousePickerTrap *> TVectorPickerTraps;

	class MousePickerSystem
		: public Holder<MousePickerSystem>
	{
	public:
		MousePickerSystem();

	public:
		void update( Arrow * _arrow );
		void clear();

		void pickTrap( Arrow * _arrow, TVectorPickerTraps & _traps );
		
		std::size_t regTrap( MousePickerTrap * _trap );
		void unregTrap( std::size_t _id );
		void updateTrap( std::size_t _id );

		bool handleKeyEvent( Arrow * _arrow, unsigned int _key, unsigned int _char, bool _isDown );
		bool handleMouseButtonEvent( Arrow * _arrow, unsigned int _button, bool _isDown );
		bool handleMouseButtonEventEnd( Arrow * _arrow, unsigned int _button, bool _isDown );
		bool handleMouseMove( Arrow * _arrow, float _x, float _y, int _whell );

		

	private:
		void updatePicked_( Arrow * _arrow );
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

		typedef std::vector<PickerTrapState> TVectorPickerTrapState;
		TVectorPickerTrapState m_listPickerTrap;
		TVectorPickerTrapState m_registration;
		TVectorPickerTrapState::iterator m_trapIterator;

	private:
		static bool isPicked( const PickerTrapState & _state );
	};
}
