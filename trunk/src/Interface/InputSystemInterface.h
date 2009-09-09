#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class InputSystemHandler
	{
	public:
		virtual bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) = 0;
		virtual bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) = 0;
		virtual bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) = 0;
		virtual bool handleMouseMove( float _x, float _y, int _whell ) = 0;
	};

	class InputSystemInterface
	{
	public:
		virtual bool initialize( WindowHandle _winHandle ) = 0;
		virtual bool captureMouse( float _x, float _y, float _maxX, float _maxY ) = 0;
		virtual void releaseMouse() = 0;
		virtual void update() = 0;
		virtual void destroy() = 0;

		virtual void regHandle( InputSystemHandler * _handle ) = 0;

		virtual bool isKeyDown( int _key ) = 0;
		virtual bool isModifierDown( int _modifier ) = 0;

		virtual float getMouseX() const = 0;
		virtual float getMouseY() const = 0;
		virtual int getMouseWhell() const = 0;

		virtual bool isAnyButtonDown() const = 0;
		virtual bool isButtonDown( int _button ) const = 0;
	};
}

bool initInterfaceSystem(Menge::InputSystemInterface **);
void releaseInterfaceSystem(Menge::InputSystemInterface *);
