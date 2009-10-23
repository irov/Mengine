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
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLMacOSXWindowContext::swapBuffers()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLMacOSXWindowContext::setVSync( bool _vsync )
	{
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
