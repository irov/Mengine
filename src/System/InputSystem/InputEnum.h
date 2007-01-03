#	pragma once

#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>

enum
{
	DI_DEFAULT = 0,
	DI_RELEASED = 1,
	DI_HELD = 2,
	DI_PRESSED = 3, 

	MOUSE_LEFT = 0,
	MOUSE_RIGHT = 1,
	MOUSE_MIDDLE = 2,

	MOUSE_X = 0,
	MOUSE_Y = 1,
	MOUSE_Z = 2
};