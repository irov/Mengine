#	pragma once

typedef void* WINDOW_HANDLE;

class InputSystemHandler
{
public:
	virtual bool handleKeyEvent( size_t _key, size_t _char, bool _isDown ) = 0;
	virtual bool handleMouseButtonEvent( size_t _button, bool _isDown ) = 0;
	virtual bool handleMouseMove( int _x, int _y, int _whell ) = 0;
};

class InputSystemInterface
{
public:
	virtual bool initialize( WINDOW_HANDLE _winHandle ) = 0;
	virtual bool captureMouse( int _x, int _y, int _maxX, int _maxY ) = 0;
	virtual void releaseMouse() = 0;
	virtual void update() = 0;
	virtual void destroy() = 0;

	virtual void regHandle( InputSystemHandler * _handle ) = 0;

	virtual bool isKeyDown( int _key ) = 0;
	virtual bool isModifierDown( int _modifier ) = 0;

	virtual int getMouseX() const = 0;
	virtual int getMouseY() const = 0;
	virtual int getMouseWhell() const = 0;

	virtual bool isAnyButtonDown() const = 0;
	virtual bool isButtonDown( int _button ) const = 0;
};

bool initInterfaceSystem(InputSystemInterface **);
void releaseInterfaceSystem(InputSystemInterface *);