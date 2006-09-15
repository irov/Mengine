#	pragma once

#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>

class CInputCore;

class CInputKeyboard
{
	friend class CInputCore;
public:
	CInputKeyboard(CInputCore *InputCore);

public:
	HRESULT Init();
	HRESULT Acquire();
	HRESULT Unacquire();

	void Update ();
	bool IsActive ()const { return m_bActive; };

public:
	bool IsBut(DWORD,int)const;
	bool IsAnyButD()const;
	bool GetChar(char *,int)const;

private:
	LPDIRECTINPUTDEVICE8 m_pDev;
	CInputCore *m_InputCore;
	bool m_bActive;
	bool m_bUse;

	unsigned char m_LayerKeys;
	unsigned char m_Keys[2][256];	
};


