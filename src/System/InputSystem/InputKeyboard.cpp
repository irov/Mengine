#	include "InputCore.h"
#	include "InputKeyboard.h"
#	include "InputEnum.h"

#	include <string>

#	define IFFAILED( X )  if( (HRESULT)((X) < 0) )

static std::string char_row[] = 
{
	"1234567890-=",
		"qwertyuiop[]",
		"asdfghjkl;'\\",
		"zxcvbnm,./"
};

static std::string shift_char_row[] =
{
	"!@#$%^&*()_+",
		"QWERTYUIOP{}",
		"ASDFGHJKL:\"|",
		"ZXCVBNM<>?"
};

CInputKeyboard::CInputKeyboard(CInputCore *InputCore)
:	m_InputCore(InputCore)
,	m_pDev(0)
,	m_bActive(FALSE)
,	m_bUse(FALSE)
,	m_LayerKeys(false)
{
	for( int i = 0; i<256; ++i )
	{
		m_Keys[0][i] = 0;
		m_Keys[1][i] = 0;
	}
}

HRESULT CInputKeyboard::Init()
{
	if( m_InputCore == 0 )
	{
		return -1;
	}

	HRESULT	hr = 0;

	IFFAILED(hr = m_InputCore->m_pInput->CreateDevice (GUID_SysKeyboard, &m_pDev, 0)) 
	{
		//PUSHERROR("CreateDevice GUID_SysKeyboard error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}
	IFFAILED(hr = m_pDev->SetDataFormat (&c_dfDIKeyboard) )
	{
		//PUSHERROR("SetDataFormat error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}

	//Required for game editor
#ifdef _EDITOR
	IFFAILED(hr = m_pDev->SetCooperativeLevel(m_InputCore->m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))
#else
	IFFAILED(hr = m_pDev->SetCooperativeLevel(m_InputCore->m_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE))
#endif
//	IFFAILED(hr = m_pDev->SetCooperativeLevel (m_InputCore->m_hWnd , DISCL_FOREGROUND | DISCL_EXCLUSIVE))
	{
		//PUSHERROR("SetCooperativeLevel [ DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ] error code = %d [/FATAL ERROR/]",hr);
		return hr;
	}	

	m_bUse = TRUE;
	m_bActive = TRUE;

	for( int i = 0; i<256; ++i )
	{
		m_Keys[0][i] = 0;
		m_Keys[1][i] = 0;
	}

	return 0;
};

HRESULT CInputKeyboard::Acquire()
{
	if(m_pDev)
	{
		return m_pDev->Acquire();
	}
	return -1;
}

HRESULT CInputKeyboard::Unacquire()
{
	if(m_pDev)
	{
		return m_pDev->Unacquire();
	}
	return -1;
}

void CInputKeyboard::Update (void)
{
	HRESULT	hr = 0;

	++m_LayerKeys;
	m_LayerKeys &= 1;

	const int SizeLayerKeys = sizeof(m_Keys[m_LayerKeys]);
	IFFAILED(hr = m_pDev->GetDeviceState (SizeLayerKeys, (LPVOID)&m_Keys[m_LayerKeys]))
	{
#ifndef _EDITOR
		if( hr == DIERR_INPUTLOST )
		{
			hr = m_pDev->Acquire ();
		}

		if( hr == DIERR_NOTACQUIRED )
		{
			hr = m_pDev->Acquire ();
		}
#endif // _EDITOR

		for( int i = 0; i<256; ++i )
		{
			m_Keys[0][i] = 0;
			m_Keys[1][i] = 0;
		}
	}
};

bool CInputKeyboard::IsBut (DWORD index,int key_state)const
{
	int _state;
	if(m_Keys[m_LayerKeys][index] & 0x80) 
	{
		if( m_Keys[(m_LayerKeys+1)&1][index] & 0x80 )
		{
			_state = DI_HELD; 
		}
		else
		{
			_state = DI_PRESSED;
		}
	}
	else
	{
		if( m_Keys[(m_LayerKeys+1)&1][index] & 0x80 )
		{
			_state = DI_RELEASED;
		}
		else
		{
			_state = DI_DEFAULT;
		}
	}
	return (_state == key_state);
}

bool CInputKeyboard::IsAnyButD (void)const
{
	for(int i=0; i<256; i++)
	{
		if(m_Keys[m_LayerKeys][i] & 0x80) return true;
	}

	return false;
};

bool CInputKeyboard::GetChar(char *ch, int char_state)const
{
	*ch = 0;

	bool shift=(m_Keys[m_LayerKeys][DIK_LSHIFT]& 0x80) || (m_Keys[m_LayerKeys][DIK_RSHIFT]& 0x80);

	for (int i = DIK_1; i <= DIK_EQUALS; i++)	// Detection First Row
	{
		if (IsBut(i,char_state))						
		{
			if (shift)		
			{
				*ch = shift_char_row[0][i-DIK_1];		// !@#$%^&*()_+
			}
			else 
			{
				*ch = char_row[0][i - DIK_1];			// 1234567890	
			}
		}
	}

	for (int i = DIK_Q; i <= DIK_RBRACKET; i++) // Detection Second Row
	{
		if (IsBut(i,char_state)) 
		{
			if (shift)
			{
				*ch = shift_char_row[1][i-DIK_Q];		// QWERTYUIOP{}
			}
			else 
			{
				*ch = char_row[1][i - DIK_Q];			// qwertyuiop[]
			}
		}
	}

	for (int i = DIK_A; i <= DIK_APOSTROPHE; i++) // Detection Third Row
	{
		if (IsBut(i,char_state)) 
		{
			if (shift)
			{
				*ch = shift_char_row[2][i-DIK_A];		// ASDFGHJKL:"|
			}
			else 
			{
				*ch = char_row[2][i - DIK_A];			// asdfghjkl;'
			}
		}
	}

	for (int i = DIK_Z; i <= DIK_SLASH ; i++) // Detection First Row
	{
		if (IsBut(i,char_state)) 
		{
			if (shift)
			{
				*ch = shift_char_row[3][i-DIK_Z];
			}
			else 
			{
				*ch = char_row[3][i - DIK_Z];
			}
		}
	}

	if (IsBut(DIK_SPACE,char_state))
	{
		*ch = ' ';
	}
	if (0==*ch)
	{
		return false;
	}

	return true;
}

