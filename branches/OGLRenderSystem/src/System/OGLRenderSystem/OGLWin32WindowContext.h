/*
 *	OGLWin32WindowContext.h
 *
 *	Created by _Berserk_ on 8.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "OGLWindowContext.h"

#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>

namespace Menge
{
	class OGLWin32WindowContext
		: public OGLWindowContext
	{
	public:
		OGLWin32WindowContext();
		~OGLWin32WindowContext();

	public:
		bool initialize( std::size_t _width, std::size_t _height, int _bits,
						bool _fullscreen, WindowHandle _winHandle, bool _waitForVSync ) override;
		void swapBuffers() override;
		void setVSync( bool _vsync ) override;

	private:
		HDC m_hdc;
		WindowHandle m_winHandle;
		HGLRC m_hglrc;

	};
}	// namespace Menge
