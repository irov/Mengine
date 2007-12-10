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

	protected:
		int m_mouseX;
		int m_mouseY;
		InputSystemInterface * m_interface;
		InputHandler * m_handle;
	};
}