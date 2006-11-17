#	include "InputCore.h"
#	include "InputEnum.h"

#	include "InputMouse.h"

#	define IFFAILED( X )  if( (HRESULT)((X) < 0) )

CInputMouse::CInputMouse(CInputCore *InputCore)
: m_InputCore(InputCore)
, m_pDev(0)
, m_bUse(FALSE)
, m_bActive(FALSE)
{
	for( int i = 0; i< 8; ++i)
	{
		m_bButD[i] = 1;
	}
	for( int i = 0; i < 4; ++i)
	{
		m_MouseState.rgbButtons[i] = 0;
	}
}

HRESULT CInputMouse::Init()
{
	HRESULT	hr = 0;

	IFFAILED(hr = m_InputCore->m_pInput->CreateDevice (GUID_SysMouse, &m_pDev, 0))
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
#ifdef _EDITOR
	IFFAILED(hr = m_pDev->SetCooperativeLevel(m_InputCore->m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))
	{
		//PUSHERROR("SetCooperativeLevel [ DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ] error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}
#else
	IFFAILED(hr = m_pDev->SetCooperativeLevel(m_InputCore->m_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE))
	{
		//PUSHERROR("SetCooperativeLevel [ DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ] error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}
#endif


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

	m_bUse = true;
	m_bActive = true;

	return 0;
};

void CInputMouse::Restore (void)
{
	m_pDev->Acquire ();
	for( int i = 0; i< 8; ++i)
	{
		m_bButD[i] = 1;
	}

	for( int i = 0; i < 4; ++i)
	{
		m_MouseState.rgbButtons[i] = 0;
	}
}

void CInputMouse::SetPosSpeed(int in_x0, int in_y0, int in_z0,int in_speed)
{
	ZeroMemory (m_But, sizeof(m_But));

	m_CurPos[0] = in_x0;
	m_CurPos[1] = in_y0;
	m_CurPos[2] = in_z0;

	m_Speed = in_speed;
};

void CInputMouse::SetPosRange(int in_x1, int in_y1, int in_z1,int in_x2, int in_y2, int in_z2)
{
	m_PosRange[0] = in_x1;
	m_PosRange[1] = in_y1;
	m_PosRange[2] = in_z1;
	m_PosRange[0+3] = in_x2;
	m_PosRange[1+3] = in_y2;
	m_PosRange[2+3] = in_z2;
};

void CInputMouse::Update (void)
{
	HRESULT	hr = 0;

	memcpy (m_OldPos, m_CurPos, sizeof(m_CurPos));
	ZeroMemory (m_But, sizeof(m_But));
	ZeroMemory (m_DeltaPos, sizeof(m_DeltaPos));

	m_bDone = false;

	IFFAILED(hr = m_pDev->GetDeviceState( sizeof(m_MouseState),&m_MouseState))
	{
#ifndef _EDITOR
		if( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
		{
			Restore();
		}
#endif // _EDITOR
		return;
	}

	for( int i = 0; i<4; i++ )
	{
		m_But[i] = m_MouseState.rgbButtons[i] - m_LastMouseState.rgbButtons[i];
	}
	m_LastMouseState = m_MouseState;


	while(!m_bDone)
	{

		m_Elem = 1;
		if(FAILED(hr = m_pDev->GetDeviceData (sizeof(m_Data), &m_Data, &m_Elem, 0)))
		{
#ifndef _EDITOR
			if( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
			{
				Restore();
			}
#endif // _EDITOR
			break;

		}

		if(m_Elem == 1)
		{
			switch(m_Data.dwOfs)
			{
			case DIMOFS_X:
			case DIMOFS_Y:
			case DIMOFS_Z:
				{
					int axis = m_Data.dwOfs >> 2;

					m_DeltaPos[axis] += m_Data.dwData * m_Speed;
					m_CurPos[axis] += m_DeltaPos[axis];

					if( m_CurPos[axis] < m_PosRange[axis]) 
					{
						m_CurPos[axis] = m_PosRange[axis];
					}
					else 
					{
						if(m_CurPos[axis] > m_PosRange[axis+3]) 
						{
							m_CurPos[axis] = m_PosRange[axis+3];
						}
					}
				}break;
			}
		}
		else
		{
			if(m_Elem == 0)
			{
				m_bDone = true;
			}
		}

	};

	m_bMove = ( m_DeltaPos[0] != 0 || m_DeltaPos[1] != 0 )?true:false;
};

bool CInputMouse::IsAnyButD (void)const
{
	for( int i=0; i<8; ++i)
	{
		if( m_bButD[i] )
		{
			return true;
		}
	}
	return false;
};
