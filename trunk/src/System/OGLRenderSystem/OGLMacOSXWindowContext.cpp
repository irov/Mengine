/*
*	OGLMacOSXWindowContext.cpp
*
*	Created by _Berserk_ on 21.10.2009
*	Copyright 2009 Menge. All rights reserved.
*
*/

#	include "OGLMacOSXWindowContext.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void createWindowContext( OGLWindowContext** _pWindowContext )
	{
		*_pWindowContext = new OGLMacOSXWindowContext();
	}
	//////////////////////////////////////////////////////////////////////////
	void releaseWindowContext( OGLWindowContext* _pWindowContext )
	{
		delete static_cast<OGLMacOSXWindowContext*>( _pWindowContext );
	}
	//////////////////////////////////////////////////////////////////////////
	OGLMacOSXWindowContext::OGLMacOSXWindowContext()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OGLMacOSXWindowContext::~OGLMacOSXWindowContext()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLMacOSXWindowContext::initialize( std::size_t _width, std::size_t _height, int _bits,
		bool _fullscreen, WindowHandle _winHandle, bool _waitForVSync )
	{
		int i = 0;
		AGLPixelFormat pixelFormat;
		GLint attribs[] = { AGL_NO_RECOVERY, GL_TRUE, AGL_ACCELERATED, GL_TRUE, AGL_RGBA, AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 16, AGL_NONE };
		
		GDHandle gdhDisplay;
		m_mainDisplayID = CGMainDisplayID();
		m_desktopDisplayMode = CGDisplayCurrentMode( m_mainDisplayID );
		
		m_fullscreen = _fullscreen;
		/*if( m_fullscreen )
		{
			CFDictionaryRef refDisplayMode = 0;
			refDisplayMode = CGDisplayBestModeForParameters( CGMainDisplayID(), _bits, _width, _height, NULL );
			CGDisplaySwitchToMode( CGMainDisplayID(), refDisplayMode ); 
		}
		
		if( DMGetGDeviceByDisplayID( (DisplayIDType)m_mainDisplayID, &gdhDisplay, false ) != noErr )
		{
			return false;
		}*/
		
		
		pixelFormat = aglChoosePixelFormat( 0, 0, attribs );
	
		// Note: when using a single context with AGL one must ensure that there is always a context attached to a window to ensure
		// the accelerated surface is not destroyed.  We use buffer naming and use a dummy context to hold accelerated surface in place.
		// The following code points both contexts to the same buffer name (thus buffer) and then leaves the dummy context attached to the
		// window.
		//m_aglDummyContext = aglCreateContext( pixelFormat, NULL );
		//GLint bufferName = 1;
		//aglSetInteger( m_aglDummyContext, AGL_BUFFER_NAME, &bufferName ); // set buffer name for this window context
		m_aglContext = aglCreateContext(pixelFormat, NULL);
		if( m_aglContext == NULL )
		{
			return false;
		}
		//aglSetInteger( m_aglContext, AGL_BUFFER_NAME, &bufferName); // set same buffer name to share hardware buffers
	
		aglDestroyPixelFormat( pixelFormat );
		
		m_windowRef = (WindowRef)_winHandle;
		
	    GrafPtr portSave = NULL;
	    GetPort(&portSave);
		SetPort((GrafPtr) GetWindowPort(m_windowRef));
				
		/*if( !aglSetDrawable( m_aglDummyContext, GetWindowPort(m_windowRef) ) ) // force creation of buffers for window
		{
			return false;
		}*/
		//aglSetDrawable( m_aglDummyContext, GetWindowPort(m_windowRef) );
		
//		if( m_fullscreen )
//		{
//			aglSetFullScreen( m_aglContext, 0, 0, 0, 0 );
//		}
//		else
		{
			aglSetDrawable(m_aglContext, GetWindowPort(m_windowRef) );
		}
		aglSetCurrentContext(m_aglContext);
		GLint val = 1;
		aglSetInteger( m_aglContext, AGL_STATE_VALIDATION, &val );
		
		SetPort(portSave);
	
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLMacOSXWindowContext::swapBuffers()
	{
		aglSwapBuffers(m_aglContext);
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLMacOSXWindowContext::setVSync( bool _vsync )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLMacOSXWindowContext::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		if( _fullscreen == true )
		{
			//ConstrainWindowToScreen( m_window, kWindowContentRgn, kWindowConstrainMayResize | kWindowConstrainMoveRegardlessOfFit, NULL, NULL );
			Rect winRect;
			winRect.top = 0;
			winRect.left = 0;
			winRect.right = _width;
			winRect.bottom = _height;
			SetWindowBounds( m_windowRef, kWindowContentRgn, &winRect );
		}
		else
		{
			Rect winRect;
			winRect.top = 0;
			winRect.left = 0;
			winRect.right = _width;
			winRect.bottom = _height;
			SetWindowBounds( m_windowRef, kWindowContentRgn, &winRect );
			RepositionWindow( m_windowRef, NULL, kWindowCenterOnMainScreen );
		}
	
		aglUpdateContext( m_aglContext );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
