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

		void pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps );
		
		std::size_t regTrap( MousePickerTrap * _trap );
		void unregTrap( std::size_t _id );
		void updateTrap( std::size_t _id );

		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell ) override;

		

	private:
		void updatePicked_( const mt::vec2f& _point );
		void updateDead_();

		bool execReg_();

	private:
		struct PickerFinder;

		struct PickerTrapState
		{
			MousePickerTrap * trap;
			std::size_t id;
			bool picked;
			bool handle;
			bool dead;
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
