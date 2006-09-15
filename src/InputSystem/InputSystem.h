#	pragma once

#	include "InputEnum.h"
#	include "interfaces.h"

class CInputCore;

class CInputSystem
	: public InputSystemInterface
{
public:
	CInputSystem();
	virtual ~CInputSystem();

public:
	bool init(HWND hWnd, int Joint) override;
	void destroy() override;
	void release() override;

	HRESULT reset() ;
	HRESULT acquire() ;
	HRESULT unacquire() ;

	void update() override;

	//keyboard
	bool isKey(int index,int key_state)const override;
	bool isAnyKey() override;
	bool getKey(char *ch,int key_state) override;

	//mouse
	void setPositionAndSpeed (int X, int Y, int Z, int Speed) override;
	void setRange (int min_x, int min_y, int min_z, int max_x, int max_y, int max_z) override;
	void setPosition (int axis, int value) override;

	int getPosition (int axis) override;
	int getDelta	(int axis) override;

	bool isMove() override;
	bool isAnyButton() override;
	bool isButton(int but,int but_state) override;

private:
	CInputCore	*m_InputCore;
};