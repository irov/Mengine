#	pragma once

#	include <windows.h>

enum
{
	INPUT_KEYBOARD	= 0x00000001,
	INPUT_MOUSE		= 0x00000002,
	INPUT_ALL		= 0x00000003
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