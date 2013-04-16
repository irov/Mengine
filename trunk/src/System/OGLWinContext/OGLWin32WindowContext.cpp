/*
 *	OGLWin32WindowContext.cpp
 *
 *	Created by _Berserk_ on 8.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "OGLWin32WindowContext.h"

namespace Menge
{
	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

	//////////////////////////////////////////////////////////////////////////
	void createWindowContext( OGLWindowContext** _pWindowContext )
	{
		*_pWindowContext = new OGLWin32WindowContext();
	}
	//////////////////////////////////////////////////////////////////////////
	void releaseWindowContext( OGLWindowContext* _pWindowContext )
	{
		delete static_cast<OGLWin32WindowContext*>( _pWindowContext );
	}
	//////////////////////////////////////////////////////////////////////////
	OGLWin32WindowContext::OGLWin32WindowContext()
		: m_hdc( NULL )
		, m_hglrc( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OGLWin32WindowContext::~OGLWin32WindowContext()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLWin32WindowContext::initialize( std::size_t _width, std::size_t _height, int _bits,
											bool _fullscreen, WindowHandle _winHandle, bool _waitForVSync )
	{
		m_winHandle = _winHandle;
		m_hdc = GetDC((HWND)m_winHandle);

		if( m_hdc == NULL )
		{
			return false;
		}

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = (_bits > 16)? 24 : _bits;
		pfd.cAlphaBits = (_bits > 16)? 8 : 0;
		pfd.cDepthBits = 16;
		pfd.cStencilBits = 0;

		int format = 0;
		format = ChoosePixelFormat(m_hdc, &pfd);
		if( format == 0 )
		{
			//LOG_ERROR( "OpenGL error: failed choose pixel format" );
			return false;
		}
		BOOL res = SetPixelFormat(m_hdc, format, &pfd);
		if( res == FALSE )
		{
			//LOG_ERROR( "OpenGL error: failed set pixel format" );
			return false;
		}
		m_hglrc = wglCreateContext(m_hdc);
		if( m_hglrc == 0 )
		{
			//LOG_ERROR( "OpenGL error: failed to create OpenGL context" );
			return false;
		}
		if( wglMakeCurrent(m_hdc, m_hglrc) != TRUE )
		{
			return false;
		}

		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLWin32WindowContext::swapBuffers()
	{
		SwapBuffers( m_hdc );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLWin32WindowContext::setVSync( bool _vsync )
	{
		if( wglSwapIntervalEXT != NULL )
		{
			wglSwapIntervalEXT( _vsync ? 1 : 0 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLWin32WindowContext::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		if( _fullscreen )
		{
			DEVMODE dm;
			dm.dmSize = sizeof(DEVMODE);
			dm.dmPelsWidth = _width;
			dm.dmPelsHeight = _height;
			dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
		}
		else
		{
			// drop out of fullscreen
			ChangeDisplaySettings(NULL, 0);
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
