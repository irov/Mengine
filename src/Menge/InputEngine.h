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

		bool initialize( WINDOW_HANDLE _winHandle );

	public:
		void update();

	public:
		bool isKeyDown( int index );
		bool isModifierDown( int _modifier );

		void regHandle( InputHandler * _handle );

		int getMouseX() const;
		int getMouseY() const;
		int getMouseWhell() const;

		bool isAnyButtonDown() const;
		bool isButtonDown( int _button ) const;

		void setMousePos( int _x, int _y );

		void setMouseBounded( bool _bounded );
		bool getMouseBounded() const;

	protected:
		int m_mouseX;
		int m_mouseY;
		InputSystemInterface * m_interface;
		InputHandler * m_handle;
		bool m_mouseBounded;
	};
}