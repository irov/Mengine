#	pragma once

#   include "Interface/MousePickerSystemInterface.h"

#	include "Kernel/Scriptable.h"
#	include "Kernel/Eventable.h"

namespace Menge
{
    class MousePickerSystemInterface;
	struct PickerTrapState;

	class MousePickerAdapter
		: public MousePickerTrapInterface        
        , virtual public Scriptable
		, virtual public Eventable
	{
	public:
		MousePickerAdapter( bool _defaultHandle );

	public:
		void setEventListener( PyObject * _listener );

	public:
		void setDefaultHandle( bool _handle );
		bool getDefaultHandle() const;

	public:
		void activatePicker();
		void deactivatePicker();

	public:
		void updatePicker();

	protected:
		bool onMouseEnter() override;
		void onMouseLeave() override;

	protected:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;

		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;		
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;

    protected:
        MousePickerSystemInterface * getMousePickerSystem() const;

	protected:
		PickerTrapState * m_picker;

		bool m_defaultHandle;

	private:
		bool m_onEnterEvent;
		bool m_onLeaveEvent;
	};
}
