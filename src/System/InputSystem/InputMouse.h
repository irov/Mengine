#	pragma once

#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>

class CInputCore;

class CInputMouse
{
	friend class CInputCore;
public:
	CInputMouse(CInputCore *InputCore);

public:
	HRESULT Init();
	void Update ();
	void Restore ();
	bool IsActive () { return m_bActive; };

public:
	void SetPosSpeed (int, int, int, int);
	void SetPosRange (int, int, int, int, int, int);
	void SetCurPos (int axis, int value) { m_CurPos[axis] = value; };

	int GetCurPos		(int axis)const { return m_CurPos[axis]; };
	int GetDeltaPos		(int axis)const { return m_DeltaPos[axis]; };

	bool IsMove ()const { return m_bMove; };
	BYTE IsButD (int but)const { return m_MouseState.rgbButtons[but]; };
	bool IsBJustD (int but)const { return m_But[but]>0 ? true : false; };
	bool IsBJustU (int but)const { return m_But[but]<0 ? true : false; };
	bool IsAnyButD ()const;

private:
	LPDIRECTINPUTDEVICE8 m_pDev;
	CInputCore *m_InputCore;

	bool m_bActive;
	bool m_bUse;

	bool m_bDone;
	DIDEVICEOBJECTDATA m_Data;
	DIMOUSESTATE m_MouseState;
	DIMOUSESTATE m_LastMouseState;
	DWORD m_Elem;

	int m_Speed; 
	bool m_bMove;	
	bool m_bButD[8]; 
	int m_But[8]; 
	int m_OldPos[3]; 
	int m_PosRange[6]; 
	int m_CurPos[3];
	int m_DeltaPos[3]; 
};
