#	pragma once

#	include <windows.h>

class InputSystemInterface
{
public:
	virtual bool init(HWND hWnd, int Joint) = 0;
	virtual void release() = 0;
	virtual void destroy() = 0;

	virtual void update() = 0;

	//keyboard
	virtual bool isKey(int index,int key_state)const = 0;
	virtual bool isAnyKey() = 0;
	virtual bool getKey(char *ch,int key_state) = 0;

	//mouse
	virtual void setPositionAndSpeed (int X, int Y, int Z, int Speed) = 0;
	virtual void setRange (int min_x, int min_y, int min_z, int max_x, int max_y, int max_z) = 0;
	virtual void setPosition (int axis, int value) = 0;

	virtual int getPosition (int axis) = 0;
	virtual int getDelta	(int axis) = 0;

	virtual bool isMove() = 0;
	virtual bool isAnyButton() = 0;
	virtual bool isButton(int but,int but_state) = 0;
};

bool initInterfaceSystem(InputSystemInterface **);
void releaseInterfaceSystem(InputSystemInterface *);