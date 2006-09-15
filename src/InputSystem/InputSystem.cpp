#	include "InputSystem.h"

#	include "InputCore.h"
#	include "InputKeyboard.h"
#	include "InputMouse.h"

#	include <windows.h>
#	include <string>

#	pragma comment (lib, "dinput8.lib")
#	pragma comment (lib, "dxguid.lib")

#	define IFFAILED( X )  if( (HRESULT)((X) < 0) )

bool initInterfaceSystem(InputSystemInterface **_system)
{
	try
	{
		*_system = new CInputSystem();
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}

void releaseInterfaceSystem(InputSystemInterface *_system)
{
	delete static_cast<CInputSystem*>(_system);
}

CInputSystem::CInputSystem()
{
	m_InputCore = new CInputCore;
}


CInputSystem::~CInputSystem()
{
	delete m_InputCore;
}

void CInputSystem::destroy()
{
	delete this;
}

bool CInputSystem::init(HWND hWnd, int Joint)
{
	return ( m_InputCore->Init(hWnd,Joint) < 0 );
}

HRESULT CInputSystem::reset()
{
	return m_InputCore->Reset();
}

HRESULT CInputSystem::acquire()
{
	return m_InputCore->Acquire();
};

HRESULT CInputSystem::unacquire()
{
	return m_InputCore->Unacquire();
};

void CInputSystem::update (void)
{
	m_InputCore->Update();
}

bool CInputSystem::isKey(int index,int key_state)const
{
	if( m_InputCore->m_InputKeyboard->IsActive() == false )
	{
		return false;
	}

	return m_InputCore->m_InputKeyboard->IsBut(index, key_state);
}

bool CInputSystem::isAnyKey()
{
	if( m_InputCore->m_InputKeyboard->IsActive() == false )
	{
		return false;
	}

	return m_InputCore->m_InputKeyboard->IsAnyButD();
}

bool CInputSystem::getKey(char *ch,int key_state)
{
	if( m_InputCore->m_InputKeyboard->IsActive() == false )
	{
		return false;
	}

	return m_InputCore->m_InputKeyboard->GetChar(ch,key_state);
}

void CInputSystem::setPositionAndSpeed (int X, int Y, int Z, int Speed)
{
	if( m_InputCore->m_InputKeyboard->IsActive() == false )
	{
		return;
	}

	m_InputCore->m_InputMouse->SetPosSpeed(X,Y,Z,Speed);
}

void CInputSystem::setRange (int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
{
	if( m_InputCore->m_InputMouse->IsActive() == false )
	{
		return;
	}

	m_InputCore->m_InputMouse->SetPosRange(min_x,min_y,min_z,max_x,max_y,max_z);
}

void CInputSystem::setPosition (int axis, int value)
{
	if( m_InputCore->m_InputMouse->IsActive() == false )
	{
		return;
	}

	m_InputCore->m_InputMouse->SetCurPos(axis,value);
}

int CInputSystem::getPosition (int axis)
{
	if( m_InputCore->m_InputMouse->IsActive() == false )
	{
		return 0;
	}

	return m_InputCore->m_InputMouse->GetCurPos(axis);
}

int CInputSystem::getDelta	(int axis)
{
	if( m_InputCore->m_InputMouse->IsActive() == false )
	{
		return 0;
	}

	return m_InputCore->m_InputMouse->GetDeltaPos(axis);
}

bool CInputSystem::isMove()
{
	if( m_InputCore->m_InputMouse->IsActive() == false )
	{
		return false;
	}

	return (m_InputCore->m_InputMouse->IsMove() != 0) ;
}

bool CInputSystem::isButton(int but,int but_state )
{
	if( m_InputCore->m_InputMouse->IsActive() == false )
	{
		return false;
	}

	if( but_state == DI_HELD ) 
	{
		return (m_InputCore->m_InputMouse->IsButD(but) != 0);
	}
	else if( but_state == DI_DEFAULT) 
	{
		return !(m_InputCore->m_InputMouse->IsButD(but) != 0);
	}
	else if( but_state == DI_PRESSED)
	{
		return (m_InputCore->m_InputMouse->IsBJustD(but) != 0);		
	}
	else if( but_state == DI_RELEASED)
	{
		return (m_InputCore->m_InputMouse->IsBJustU(but) != 0);
	}
	return false;
}

bool CInputSystem::isAnyButton()
{
	if( m_InputCore->m_InputMouse->IsActive() == false )
	{
		return false;
	}

	return (m_InputCore->m_InputMouse->IsAnyButD() != 0);
}

void CInputSystem::release()
{
	m_InputCore->Release();
};