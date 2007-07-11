#	pragma once

#	define DIRECTINPUT_VERSION 0x0800

#	include "Interface/WinInputSystemInterface.h"

#	include <dinput.h>

class InputKeyboard;
class InputMouse;

class InputSystem
	: public WinInputSystemInterface
{
public:
	InputSystem();
	~InputSystem();

public:
	bool init( HWND _hWnd, int _joint ) override;
	void update() override;
	void destroy() override;
	

	//keyboard
	bool isKey( int _key, int _state ) const override;
	bool isAnyKeyDown() const override;
	bool getChar( char *_char, int _state ) override;

	//mouse
	void setPosition( float _x, float _y, float _whell );
	void setSensitivity( float _sensitivity );

	void setRange( const float *_minRange, const float * _maxRange ) override;

	const float * getPosition() const override;
	const float * getDelta() const override;

	bool isMove() const override;
	bool isAnyButtonDown() const override;
	bool isButton( int _button, int _state ) const override;

protected:
	void release();

	bool acquire();
	bool unacquire();

private:
	LPDIRECTINPUT8 getDirectInput();
	HWND getHWnd();

private:
	LPDIRECTINPUT8	m_pDirectInput;
	HWND       		m_hWnd;	
	int				m_joint;

	InputKeyboard	*m_keyboard;
	InputMouse		*m_mouse;

	friend class InputJoint;
};