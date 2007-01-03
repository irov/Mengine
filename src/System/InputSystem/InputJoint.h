#	pragma once

#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>

class InputSystem;

class InputJoint
{
public:
	InputJoint( InputSystem * _system );

public:
	virtual HRESULT init() = 0;
	virtual HRESULT restore() = 0;
	virtual void update() = 0;
	
public:
	HRESULT acquire();
	HRESULT unacquire();
	void release();

	
	bool isActive() const;
	void setActive( bool _value );

protected:
	LPDIRECTINPUT8 getDirectInput();
	HWND getHWnd();

protected:
	InputSystem * m_system;
	bool m_active;

	LPDIRECTINPUTDEVICE8 m_pDev;

	friend class InputSystem;
};