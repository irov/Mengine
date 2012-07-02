#	pragma once

#	include "Core/Holder.h"
#	include "Core/Viewport.h"

#	include "Interface/InputSystemInterface.h"

#	include <list>
#	include <vector>

namespace Menge
{
	class MousePickerTrap;
	class InputHandler;

	class Arrow;

	typedef std::vector<MousePickerTrap *> TVectorPickerTraps;

	struct PickerTrapState
	{
		MousePickerTrap * trap;
		size_t id;
		bool picked;
		bool handle;
		bool dead;
	};

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
		
		PickerTrapState * regTrap( MousePickerTrap * _trap );
		void unregTrap( PickerTrapState * _id );

	public:
		void beginTrap();
		void updateTrap( PickerTrapState * _id );

	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;
				
	public:
		void handleMouseEnter( const mt::vec2f & _point );
		void handleMouseLeave();

	public:
		size_t getPickerTrapCount() const;

	private:
		void updatePicked_( const mt::vec2f& _point );		
		void updateDead_();

		bool execReg_();

	private:	
		size_t m_enumerator;

		bool m_block;

		Arrow * m_arrow;

		typedef std::list<PickerTrapState> TPickerTrapState;
		TPickerTrapState m_pickerTrapState;

		typedef std::vector<PickerTrapState *> TPickerTrapRef;
		TPickerTrapRef m_registration;
		TPickerTrapRef m_process;
		//TVectorPickerTrapState::iterator m_trapIterator;

	private:
		static bool isPicked( const PickerTrapState * _state );
	};
}
