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
		void regEventListener( PyObject * _listener );

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
		bool handleGlobalMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleGlobalMouseMove( float _x, float _y, int _whell ) override;
		bool handleGlobalKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;

	protected:
		virtual bool isEnableGlobalHandle() const = 0;

	protected:
		bool m_globalMouseEvent;
		bool m_globalKeyEvent;
	};
}
