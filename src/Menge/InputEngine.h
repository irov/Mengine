#	pragma once

#	include "Interface/InputSystemInterface.h"

#	include "Holder.h"

namespace Menge
{
	class InputHandler;

	class InputEngine
	{
	public:
		InputEngine( InputSystemInterface * _interface );
		~InputEngine();

		bool initialize( WindowHandle _winHandle );

	public:
		void update();

	public:
		bool isKeyDown( int index );
		bool isModifierDown( int _modifier );

		void regHandle( InputHandler * _handle );

		float getMouseX() const;
		float getMouseY() const;
		int getMouseWhell() const;

		bool isAnyButtonDown() const;
		bool isButtonDown( int _button ) const;

		void setMousePos( float _x, float _y );

		void setMouseBounded( bool _bounded );
		bool getMouseBounded() const;

	protected:
		float m_mouseX;
		float m_mouseY;
		InputSystemInterface * m_interface;
		InputHandler * m_handle;
		bool m_mouseBounded;
	};
}
