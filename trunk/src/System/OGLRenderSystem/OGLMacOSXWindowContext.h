/*
*	OGLMacOSXWindowContext.h
*
*	Created by _Berserk_ on 21.10.2009
*	Copyright 2009 Menge. All rights reserved.
*
*/

#	pragma once

#	include "OGLWindowContext.h"
#	include <AGL/agl.h>

namespace Menge
{
	class OGLMacOSXWindowContext
		: public OGLWindowContext
	{
	public:
		OGLMacOSXWindowContext();
		~OGLMacOSXWindowContext();

	public:
		bool initialize( std::size_t _width, std::size_t _height, int _bits,
			bool _fullscreen, WindowHandle _winHandle, bool _waitForVSync ) override;
		void swapBuffers() override;
		void setVSync( bool _vsync ) override;
		void setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen ) override;

	private:
		CGDirectDisplayID m_mainDisplayID;
		CFDictionaryRef m_desktopDisplayMode;
		bool m_fullscreen;
		AGLContext m_aglContext;
		WindowRef m_windowRef;
	};
}	// namespace Menge
