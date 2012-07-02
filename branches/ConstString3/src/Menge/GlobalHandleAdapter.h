#	pragma once

#	include "InputHandler.h"

#	include "Eventable.h"
#	include "Scriptable.h"

namespace Menge
{
	class Eventable;
	class Scriptable; 

	class GlobalHandleAdapter
		: public GlobalMouseHandler
		, public GlobalKeyHandler
		, virtual public Eventable
		, virtual public Scriptable
	{
	public:
		GlobalHandleAdapter();

	public:
		void setEventListener( PyObject * _listener );

	public:
		void activateGlobalHandle();
		void deactivateGlobalHandle();

	public:
		void enableGlobalMouseEvent( bool _value );
		void enableGlobalKeyEvent( bool _value );

	protected:
		void applyGlobalMouseEvent_( bool _value );
		void applyGlobalKeyEvent_( bool _value );

	public:
		bool handleGlobalKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;

	public:
		bool handleGlobalMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleGlobalMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleGlobalMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleGlobalMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;		

	protected:
		virtual bool isEnableGlobalHandle() const = 0;

	protected:
		bool m_globalMouseEvent;
		bool m_globalKeyEvent;
	};
}
