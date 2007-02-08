#	include "InputKeyboard.h"

#	include "InputEnum.h"

#	define IFFAILED( X )  if( (HRESULT)((X) < 0) )
//////////////////////////////////////////////////////////////////////////
namespace
{
	static const char * char_row[4] = 
	{
		"1234567890-=",
		"qwertyuiop[]",
		"asdfghjkl;'\\",
		"zxcvbnm,./"
	};

	static const char * shift_char_row[4] =
	{
		"!@#$%^&*()_+",
		"QWERTYUIOP{}",
		"ASDFGHJKL:\"|",
		"ZXCVBNM<>?"
	};
}
//////////////////////////////////////////////////////////////////////////
InputKeyboard::InputKeyboard( InputSystem * _system )
: InputJoint( _system )
{
	restore();
}
//////////////////////////////////////////////////////////////////////////
HRESULT InputKeyboard::restore()
{
	m_layerKeys = 0;

	for( int i = 0; i < 256; ++i )
	{
		m_keysBuffer[0][i] = 0;
		m_keysBuffer[1][i] = 0;
	}

	return acquire();
}
//////////////////////////////////////////////////////////////////////////
HRESULT InputKeyboard::init()
{
	HRESULT	hr = 0;

	LPDIRECTINPUT8 directInput = getDirectInput();

	IFFAILED(hr = directInput->CreateDevice (GUID_SysKeyboard, &m_pDev, 0)) 
	{
		//PUSHERROR("CreateDevice GUID_SysKeyboard error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}

	IFFAILED(hr = m_pDev->SetDataFormat (&c_dfDIKeyboard) )
	{
		//PUSHERROR("SetDataFormat error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}


	HWND hWnd = getHWnd();

	//Required for game editor
//#ifdef _EDITOR
	IFFAILED(hr = m_pDev->SetCooperativeLevel( hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE ) )
//#else
	//IFFAILED(hr = m_pDev->SetCooperativeLevel(m_InputCore->m_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE))
//#endif
	//IFFAILED(hr = m_pDev->SetCooperativeLevel (m_InputCore->m_hWnd , DISCL_FOREGROUND | DISCL_EXCLUSIVE))
	{
		//PUSHERROR("SetCooperativeLevel [ DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ] error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}	

	IFFAILED( hr = restore() )
	{
		return hr;
	}

	setActive( true );

	return 0;
};
//////////////////////////////////////////////////////////////////////////
void InputKeyboard::update (void)
{
	HRESULT	hr = 0;

	++m_layerKeys;
	m_layerKeys &= 1;

	const int sizeLayerKeys = sizeof(m_keysBuffer[m_layerKeys]);
	IFFAILED(hr = m_pDev->GetDeviceState (sizeLayerKeys, (LPVOID)&m_keysBuffer[m_layerKeys]))
	{
		if( hr == DIERR_INPUTLOST && hr == DIERR_NOTACQUIRED )
		{
			restore();
		}
		else
		{
			for( int i = 0; i < 256; ++i )
			{
				m_keysBuffer[0][i] = 0;
				m_keysBuffer[1][i] = 0;
			}
		}
	}
};
//////////////////////////////////////////////////////////////////////////
bool InputKeyboard::isKey( int _key, int _state ) const
{
	typedef const unsigned char TChar256 [256];
	TChar256 &lastbuffer = m_keysBuffer[((m_layerKeys + 1)&1)];
	TChar256 &buffer = m_keysBuffer[m_layerKeys];

	if( buffer[_key] & 0x80 ) 
	{
		if( lastbuffer[_key] & 0x80 )
		{
			return _state == DI_HELD; 
		}
		else
		{
			return _state == DI_PRESSED;
		}
	}
	else
	{
		if( lastbuffer[_key] & 0x80 )
		{
			return _state == DI_RELEASED;
		}
		else
		{
			return _state == DI_DEFAULT;
		}
	}

	return false;
}

bool InputKeyboard::isAnyKeyDown() const
{
	for(int i=0; i<256; i++)
	{
		if( m_keysBuffer[m_layerKeys][i] & 0x80 ) return true;
	}

	return false;
};

bool InputKeyboard::getChar( char *_char, int _state )const
{
	*_char = 0;

	bool shift=
		(m_keysBuffer[m_layerKeys][DIK_LSHIFT]& 0x80) || 
		(m_keysBuffer[m_layerKeys][DIK_RSHIFT]& 0x80);

	const char ** cur_char_row = ( shift )? shift_char_row : char_row;

	for( int i = DIK_1; i <= DIK_EQUALS; ++i)	// Detection First Row
	{
		if( isKey( i, _state ) )
		{
			*_char = cur_char_row[0][i-DIK_1];		// !@#$%^&*()_+ // 1234567890
		}
	}

	for( int i = DIK_Q; i <= DIK_RBRACKET; ++i ) // Detection Second Row
	{
		if( isKey( i, _state ) )
		{
			*_char = cur_char_row[1][i-DIK_Q];		// QWERTYUIOP{} // qwertyuiop[]
		}
	}

	for( int i = DIK_A; i <= DIK_APOSTROPHE; ++i ) // Detection Third Row
	{
		if( isKey( i, _state ) )
		{
			*_char = cur_char_row[2][i-DIK_A];		// ASDFGHJKL:"| // asdfghjkl;'
		}
	}

	for( int i = DIK_Z; i <= DIK_SLASH ; ++i ) // Detection First Row
	{
		if( isKey( i, _state ) )
		{
			*_char = cur_char_row[3][i-DIK_Z]; 
		}
	}

	if( isKey( DIK_SPACE, _state ) )
	{
		*_char = ' ';
	}

	if( *_char == 0 )
	{
		return false;
	}

	return true;
}