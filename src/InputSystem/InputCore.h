# pragma once

#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>


class CInputKeyboard;
class CInputMouse;
class CInputSystem;

class CInputCore
{
	friend class CInputKeyboard;
	friend class CInputMouse;
	friend class CInputSystem;

public:
	CInputCore();
	~CInputCore();

public:
	HRESULT Init( HWND hWnd, int Joint);
	void Update();
	void Release();
	HRESULT Acquire();
	HRESULT Unacquire();

	HRESULT Reset();


private:
	LPDIRECTINPUT8	m_pInput;
	HWND       		m_hWnd;	
	int				m_Joint;

	CInputKeyboard	*m_InputKeyboard;
	CInputMouse		*m_InputMouse;

};
