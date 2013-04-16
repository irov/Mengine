#	pragma once

#   include "Interface/MousePickerSystemInterface.h"

#	include "Core/Viewport.h"

#	include <list>
#	include <vector>

namespace Menge
{
	class MousePickerSystem
		: public MousePickerSystemInterface
	{
	public:
		MousePickerSystem( ServiceProviderInterface * _serviceProvider );

	public:
		void block( bool _value ) override;

	public:
		void setArrow( Arrow * _arrow ) override;

	public:
		void update() override;
		void clear() override;

    public:
		void pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps ) override;
		
		PickerTrapState * regTrap( MousePickerTrapInterface * _trap ) override;
		void unregTrap( PickerTrapState * _id ) override;

	public:
		void beginTrap() override;
		void updateTrap( PickerTrapState * _id ) override;

	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;
				
	public:
		void handleMouseEnter( const mt::vec2f & _point ) override;
		void handleMouseLeave() override;

	public:
		size_t getPickerTrapCount() const override;

	private:
		void updatePicked_( const mt::vec2f& _point );		
		void updateDead_();

		bool execReg_();

	private:
        ServiceProviderInterface * m_serviceProvider;

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
