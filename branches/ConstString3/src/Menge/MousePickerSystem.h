#	pragma once

#	include "Core/Holder.h"
#	include "Core/Viewport.h"

#	include "Interface/InputSystemInterface.h"

#	include <vector>

namespace Menge
{
	class MousePickerTrap;
	class InputHandler;

	class Arrow;

	typedef std::vector<MousePickerTrap *> TVectorPickerTraps;

	class MousePickerSystem
		: public Holder<MousePickerSystem>
		, public InputSystemHandler
	{
	public:
		MousePickerSystem();

	public:
		void block( bool _value );

	public:
		void setArrow( Arrow * _arrow );

	public:
		void update();
		void clear();

		void pickTrap( TVectorPickerTraps & _traps );
		
		std::size_t regTrap( MousePickerTrap * _trap );
		void unregTrap( std::size_t _id );
		void updateTrap( std::size_t _id );

		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

		

	private:
		void updatePicked_();
		void updateDead_();

		bool execReg_();

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

		bool m_block;

		Arrow * m_arrow;

		typedef std::vector<PickerTrapState> TVectorPickerTrapState;
		TVectorPickerTrapState m_listPickerTrap;
		TVectorPickerTrapState m_registration;
		TVectorPickerTrapState::iterator m_trapIterator;

	private:
		static bool isPicked( const PickerTrapState & _state );
	};
}
