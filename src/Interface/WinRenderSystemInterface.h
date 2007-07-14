#	pragma once

#	include "RenderSystemInterface.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

class	WinRenderSystemInterface
	: public RenderSystemInterface
{
public:
	virtual bool	createDisplay(
		HWND _hWnd, int _width, int _height, int _bits, bool _fullScreen,
		bool _vsync, bool _stencilBuffer, bool _antiAlias, bool _pureSoftware) = 0;
};
