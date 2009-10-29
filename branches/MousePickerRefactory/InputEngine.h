#	pragma once

#	include "Interface/InputSystemInterface.h"

#	include "Holder.h"

namespace Menge
{
	class InputEngine
		: public InputSystemHandler
		, public Holder<InputEngine>
	{
	public:
		InputEngine();
		~InputEngine();

		bool initialize( WindowHandle _winHandle );

	public:
		void update();

	public:
		bool isKeyDown( int index );
		bool isModifierDown( int _modifier );

		float getMouseX() const;
		float getMouseY() const;
		int getMouseWhell() const;

		bool isAnyButtonDown() const;
		bool isButtonDown( int _button ) const;

		void setMousePos( float _x, float _y );

		void setMouseBounded( bool _bounded );
		bool getMouseBounded() const;

		void setResolution( int _x, int _y );
	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	protected:
		float m_mouseX;
		float m_mouseY;
		InputSystemInterface * m_interface;
		bool m_mouseBounded;
		int m_boundX;
		int m_boundY;
	};
}
