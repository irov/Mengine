#	pragma once

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

enum
{
	INPUT_KEYBOARD	= 0x00000001,
	INPUT_MOUSE		= 0x00000002,
	INPUT_ALL		= 0x00000003
};

enum
{
	DI_DEFAULT = 0,
	DI_RELEASED = 1,
	DI_HELD = 2,
	DI_PRESSED = 3, 

	MOUSE_LEFT = 0,
	MOUSE_RIGHT = 1,
	MOUSE_MIDDLE = 2,

	MOUSE_X = 0,
	MOUSE_Y = 1,
	MOUSE_Z = 2
};

class InputSystemInterface
{
public:
	virtual bool init( HWND _hWnd, int _joint ) = 0;
	virtual void update() = 0;
	virtual void destroy() = 0;

	virtual bool isKey( int _key, int _state ) const = 0;
	virtual bool isAnyKeyDown() const = 0;
	virtual bool getChar( char *_char, int _state ) = 0;

	virtual void setPosition( float _x, float _y, float _whell ) = 0;
	virtual void setSensitivity( float _sensitivity ) = 0;
	virtual void setRange ( const float *_minRange, const float * _maxRange ) = 0;

	virtual const float * getPosition() const = 0;
	virtual const float * getDelta() const = 0;

	virtual bool isMove() const	= 0;
	virtual bool isAnyButtonDown() const = 0;
	virtual bool isButton( int _button, int _state ) const = 0;
};

bool initInterfaceSystem(InputSystemInterface **);
void releaseInterfaceSystem(InputSystemInterface *);