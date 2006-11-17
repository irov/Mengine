#	include "InputKeyboard.h"
#	include "InputMouse.h"
#	include "InputCore.h"
#	include "InputEnum.h"

#	define IFFAILED( X )  if( (HRESULT)((X) < 0) )

CInputCore::CInputCore()
: m_hWnd(0)
, m_Joint(0)
, m_InputKeyboard(0)
, m_InputMouse(0)
{
	m_InputMouse = new CInputMouse(this);
	m_InputKeyboard = new CInputKeyboard(this);
}

CInputCore::~CInputCore()
{
	Release();

	delete m_InputKeyboard;
	delete m_InputMouse;
}

HRESULT CInputCore::Init(HWND hWnd, int Joint)
{
	m_hWnd = hWnd;
	m_Joint = Joint;

	HRESULT	hr = 0;

	IFFAILED(hr = Reset())
	{
		return hr;
	}

	if( m_Joint & INPUT_MOUSE)
	{
		IFFAILED(hr = m_InputMouse->Init())
		{
			return hr;
		}
	}

	if( m_Joint & INPUT_KEYBOARD)
	{
		IFFAILED( hr = m_InputKeyboard->Init())
		{
			return hr;
		}
	};

	return 0;
}

HRESULT CInputCore::Acquire()
{
	HRESULT	hr = 0;

	if( m_Joint & INPUT_KEYBOARD )
	{
		IFFAILED( hr = m_InputKeyboard->m_pDev->Acquire() )
		{
			return hr;
		}
	}

	if( m_Joint & INPUT_MOUSE )
	{
		IFFAILED( hr = m_InputMouse->m_pDev->Acquire() )
		{
			return hr;
		}
	}

	return hr;
}

HRESULT CInputCore::Unacquire()
{
	HRESULT	hr = 0;

	if( m_Joint & INPUT_KEYBOARD )
	{
		IFFAILED( hr = m_InputKeyboard->m_pDev->Unacquire() )
		{
			return hr;
		}
	}

	if( m_Joint & INPUT_MOUSE )
	{
		IFFAILED( hr = m_InputMouse->m_pDev->Unacquire() )
		{
			return hr;
		}
	}

	return hr;
}

void CInputCore::Update()
{
	if( m_Joint & INPUT_KEYBOARD )
	{
		if( m_InputKeyboard->m_bUse && m_InputKeyboard->m_bActive )
		{
			m_InputKeyboard->Update ();
		}
	}

	if( m_Joint & INPUT_MOUSE )
	{
		if( m_InputMouse->m_bUse && m_InputMouse->m_bActive )
		{
			m_InputMouse->Update ();
		}
	}
}

void CInputCore::Release()
{
	// Проверяем, чтоб уже не был уничтожен
	if(m_InputMouse->m_pDev == NULL && m_InputKeyboard->m_pDev == NULL && m_pInput == NULL)
	{	
		return;
	}

	Unacquire();

	if ( m_InputMouse->m_pDev ) 
	{
		m_InputMouse->m_pDev->Release();
		m_InputMouse->m_pDev = 0;
	}
	if ( m_InputKeyboard->m_pDev ) 
	{
		m_InputKeyboard->m_pDev->Release();
		m_InputKeyboard->m_pDev = 0;
	}
	if ( m_pInput ) 
	{
		m_pInput->Release();
		m_pInput = 0;
	}

	m_InputMouse->m_bUse = FALSE;
	m_InputMouse->m_bActive = FALSE;
	m_InputKeyboard->m_bUse = FALSE;
	m_InputKeyboard->m_bActive = FALSE;
}

HRESULT CInputCore::Reset()
{
	HRESULT	hr = 0;

	IFFAILED(
		hr = DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInput,
		0
		)
		)
	{
		return hr;
	}

	return 0;
}