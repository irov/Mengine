#	include "InputSystem.h"

#	include "InputKeyboard.h"
#	include "InputMouse.h"

#	define IFFAILED( X )  if( (HRESULT)((X) < 0) )
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem(InputSystemInterface **_system)
{
	try
	{
		*_system = new InputSystem();
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem(InputSystemInterface *_system)
{
	delete static_cast<InputSystem*>(_system);
}
//////////////////////////////////////////////////////////////////////////
InputSystem::InputSystem()
: m_pDirectInput(0)
, m_hWnd(0)
, m_joint(0)
{
	m_mouse = new InputMouse(this);
	m_keyboard = new InputKeyboard(this);
}
//////////////////////////////////////////////////////////////////////////
InputSystem::~InputSystem()
{
	release();

	delete m_mouse;
	delete m_keyboard;
}
//////////////////////////////////////////////////////////////////////////
void InputSystem::destroy()
{
	delete this;
}
//////////////////////////////////////////////////////////////////////////
bool InputSystem::init( HWND _hWnd, int _joint )
{
	m_hWnd = _hWnd;
	m_joint = _joint;

	HRESULT	hr = 0;

	IFFAILED(
		hr = DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pDirectInput,
		0
		)
		)
	{
		return false;
	}

	if( m_joint & INPUT_MOUSE)
	{
		IFFAILED(hr = m_mouse->init())
		{
			return false;
		}
	}

	if( m_joint & INPUT_KEYBOARD)
	{
		IFFAILED( hr = m_keyboard->init())
		{
			return false;
		}
	};

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool InputSystem::acquire()
{
	HRESULT	hr = 0;

	if( m_joint & INPUT_KEYBOARD )
	{
		IFFAILED( hr = m_keyboard->acquire() )
		{
			return false;
		}
	}

	if( m_joint & INPUT_MOUSE )
	{
		IFFAILED( hr = m_mouse->acquire() )
		{
			return false;
		}
	}

	return true;
};
//////////////////////////////////////////////////////////////////////////
bool InputSystem::unacquire()
{
	HRESULT	hr = 0;

	if( m_joint & INPUT_KEYBOARD )
	{
		IFFAILED( hr = m_keyboard->unacquire() )
		{
			return false;
		}
	}

	if( m_joint & INPUT_MOUSE )
	{
		IFFAILED( hr = m_mouse->unacquire() )
		{
			return false;
		}
	}

	return true;
};
//////////////////////////////////////////////////////////////////////////
void InputSystem::update (void)
{
	if( m_joint & INPUT_KEYBOARD )
	{
		if( m_keyboard->isActive() )
		{
			m_keyboard->update();
		}
	}

	if( m_joint & INPUT_MOUSE )
	{
		if( m_mouse->isActive() )
		{
			m_mouse->update();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool InputSystem::isKey( int _key, int _state ) const
{
	if( m_keyboard->isActive() == false )
	{
		return false;
	}

	return m_keyboard->isKey( _key, _state );
}
//////////////////////////////////////////////////////////////////////////
bool InputSystem::isAnyKeyDown() const
{
	if( m_keyboard->isActive() == false )
	{
		return false;
	}

	return m_keyboard->isAnyKeyDown();
}
//////////////////////////////////////////////////////////////////////////
bool InputSystem::getChar( char *_char, int _state )
{
	if( m_keyboard->isActive() == false )
	{
		return false;
	}

	return m_keyboard->getChar( _char, _state );
}
//////////////////////////////////////////////////////////////////////////
void InputSystem::setPosition( float _x, float _y, float _whell )
{
	m_mouse->setPosition( _x, _y, _whell );
}
//////////////////////////////////////////////////////////////////////////
void InputSystem::setSensitivity( float _sensitivity )
{
	m_mouse->setSensitivity( _sensitivity );
}
//////////////////////////////////////////////////////////////////////////
void InputSystem::setRange( const float *_minRange, const float * _maxRange )
{
	m_mouse->setRange( _minRange, _maxRange );
}
//////////////////////////////////////////////////////////////////////////
const float * InputSystem::getPosition() const
{
	if( m_mouse->isActive() == false )
	{
		return 0;
	}

	return m_mouse->getPosition();
}
//////////////////////////////////////////////////////////////////////////
const float * InputSystem::getDelta() const
{
	if( m_mouse->isActive() == false )
	{
		return 0;
	}

	return m_mouse->getDelta();
}
//////////////////////////////////////////////////////////////////////////
bool InputSystem::isMove() const
{
	if( m_mouse->isActive() == false )
	{
		return false;
	}

	return m_mouse->isMove();
}
//////////////////////////////////////////////////////////////////////////
bool InputSystem::isAnyButtonDown() const
{
	if( m_mouse->isActive() == false )
	{
		return false;
	}

	return m_mouse->isAnyButtonDown();
}
//////////////////////////////////////////////////////////////////////////
bool InputSystem::isButton( int _button, int _state ) const
{
	if( m_mouse->isActive() == false )
	{
		return false;
	}

	switch( _state )
	{
	case DI_HELD:
		return (m_mouse->isButtonDown(_button) != 0);
	case DI_DEFAULT:
		return !(m_mouse->isButtonDown(_button) != 0);
	case DI_PRESSED:
		return (m_mouse->isButtonJustDown(_button) != 0);
	case DI_RELEASED:
		return (m_mouse->isButtonJustUp(_button) != 0);
	};

	return false;
}
//////////////////////////////////////////////////////////////////////////
void InputSystem::release()
{
	unacquire();

	m_mouse->release();
	m_keyboard->release();

	if ( m_pDirectInput ) 
	{
		m_pDirectInput->Release();
		m_pDirectInput = 0;
	}
};
//////////////////////////////////////////////////////////////////////////
LPDIRECTINPUT8 InputSystem::getDirectInput()
{
	return m_pDirectInput;
}
//////////////////////////////////////////////////////////////////////////
HWND InputSystem::getHWnd()
{
	return m_hWnd;
}