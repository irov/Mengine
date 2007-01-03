#	include "InputMouse.h"

#	include "InputSystem.h"

#	include "InputEnum.h"

#	include <algorithm>

#	include <math.h>

#	define IFFAILED( X )  if( (HRESULT)((X) < 0) )

//////////////////////////////////////////////////////////////////////////
InputMouse::InputMouse( InputSystem *_system )
: InputJoint( _system )
, m_sensitivity(0)
, m_bMove(false)
{
	restore();
}
//////////////////////////////////////////////////////////////////////////
HRESULT InputMouse::restore()
{
	for( int i = 0; i< 8; ++i)
	{
		m_button[i] = 0;
	}

	for( int i = 0; i < 4; ++i)
	{
		m_lastState.rgbButtons[i] = 0;
	}

	return acquire();
};
//////////////////////////////////////////////////////////////////////////
HRESULT InputMouse::init()
{
	HRESULT	hr = 0;

	LPDIRECTINPUT8 directInput = getDirectInput();

	IFFAILED(hr = directInput->CreateDevice (GUID_SysMouse, &m_pDev, 0))
	{
		//PUSHERROR("CreateDevice GUID_SysMouse error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}

	IFFAILED(hr = m_pDev->SetDataFormat (&c_dfDIMouse))
	{
		//PUSHERROR("SetDataFormat error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}
//Required for game editor
//#ifdef _EDITOR
	//IFFAILED(hr = m_pDev->SetCooperativeLevel(m_InputCore->m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))
	//{
	//	//PUSHERROR("SetCooperativeLevel [ DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ] error code = %d [/FATAL ERROR/]",hr);
	//	return hr;
	//}
//#else

	HWND hWnd = getHWnd();

	IFFAILED(hr = m_pDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE))
	{
		//PUSHERROR("SetCooperativeLevel [ DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ] error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}
//#endif


	DIPROPDWORD MProp;
	MProp.diph.dwSize = sizeof (DIPROPDWORD);
	MProp.diph.dwHeaderSize = sizeof (DIPROPHEADER);
	MProp.diph.dwObj = 0;
	MProp.diph.dwHow = DIPH_DEVICE;
	MProp.dwData = 64;
	IFFAILED(hr = m_pDev->SetProperty (DIPROP_BUFFERSIZE, &MProp.diph))
	{
		//PUSHERROR("SetProperty [ DIPROP_BUFFERSIZE ] error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}

	IFFAILED(hr = m_pDev->Acquire())
	{
		//PUSHERROR("SetDataFormat error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}

	setActive( true );

	return hr;
};
//////////////////////////////////////////////////////////////////////////
void InputMouse::setPosition( float _x, float _y, float _whell )
{
	//std::fill(m_button, 8, 0);
	m_position[0] = _x;
	m_position[1] = _y;
	m_position[2] = _whell;
};
//////////////////////////////////////////////////////////////////////////
void InputMouse::setSensitivity( float _sensitivity )
{
	m_sensitivity = _sensitivity;
}
//////////////////////////////////////////////////////////////////////////
void InputMouse::setRange( const float *_minRange, const float * _maxRange )
{
	std::copy(_minRange, _minRange + 3, stdext::checked_array_iterator<float *>(m_range + 0, 6) );
	std::copy(_maxRange, _maxRange + 3, stdext::checked_array_iterator<float *>(m_range + 3, 6) );
};
//////////////////////////////////////////////////////////////////////////
const float * InputMouse::getPosition() const
{
	return m_position;
}
//////////////////////////////////////////////////////////////////////////
const float * InputMouse::getDelta() const
{
	return m_deltaPosition;
}
//////////////////////////////////////////////////////////////////////////
void InputMouse::update()
{
	HRESULT	hr = 0;

	std::copy( m_position, m_position + 3, stdext::checked_array_iterator<float *>(m_oldPosition, 3) );

	std::fill( m_deltaPosition, m_deltaPosition + 3, 0.f );

	DIMOUSESTATE state;

	IFFAILED( hr = m_pDev->GetDeviceState( sizeof(state),&state) )
	{
		if( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
		{
			restore();
		}
		return;
	}

	for( int i = 0; i < 8; i++ )
	{
		m_button[i] = state.rgbButtons[i] - m_lastState.rgbButtons[i];
	}
	
	m_lastState = state;

	DIDEVICEOBJECTDATA data;

	while( true )
	{
		DWORD elem = 1;

		if(FAILED(hr = m_pDev->GetDeviceData (sizeof(data), &data, &elem, 0)))
		{
			if( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
			{
				restore();
			}
			break;
		}

		if( elem != 1 )
		{
			break;
		}

		switch( data.dwOfs )
		{
		case DIMOFS_X:
		case DIMOFS_Y:
		case DIMOFS_Z:
			{
				int axis = data.dwOfs >> 2;

				m_deltaPosition[axis] += int(data.dwData) * m_sensitivity;
				m_position[axis] += m_deltaPosition[axis];

				if( m_position[axis] < m_range[axis]) 
				{
					m_position[axis] = m_range[axis];
				}
				else 
				{
					if( m_position[axis] > m_range[axis+3] ) 
					{
						m_position[axis] = m_range[axis+3];
					}
				}
			}break;
		}
	};
	
	m_bMove = ( fabsf(m_deltaPosition[0]) >  0.00001 || fabsf(m_deltaPosition[1]) > 0.0001 )? true : false;
};
//////////////////////////////////////////////////////////////////////////
bool InputMouse::isMove() const 
{ 
	return m_bMove; 
};
//////////////////////////////////////////////////////////////////////////
bool InputMouse::isButtonDown(int _button) const 
{ 
	return m_lastState.rgbButtons[_button] != 0; 
};
//////////////////////////////////////////////////////////////////////////
bool InputMouse::isButtonJustDown(int _button) const 
{
	return m_button[_button]>0 ? true : false; 
}
//////////////////////////////////////////////////////////////////////////
bool InputMouse::isButtonJustUp(int _button) const 
{ 
	return m_button[_button]<0 ? true : false; 
};
//////////////////////////////////////////////////////////////////////////
bool InputMouse::isAnyButtonDown() const
{
	for( int i=0; i<8; ++i)
	{
		if( m_button[i] != 0 )
		{
			return true;
		}
	}
	return false;
};
