#	pragma once

#	include "MousePickerTrap.h"

#	include "Eventable.h"

namespace Menge
{
	class MousePickerAdapter
		: public MousePickerTrap
		, virtual public Eventable
	{
	public:
		MousePickerAdapter( bool _defaultHandle );

	public:
		void regEventListener( PyObject * _listener );

	public:
		void activatePicker();
		void deactivatePicker();

	public:
		void updatePicker();

	protected:
		bool onEnter() override;
		void onLeave() override;

	protected:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	protected:
		std::size_t m_pickerId;

		bool m_defaultHandle;

	private:
		bool m_onEnterEvent;
		bool m_onLeaveEvent;
	};
}