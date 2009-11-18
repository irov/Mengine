/*
 *	Camera.cpp
 *
 *	Created by _Berserk_ on 24.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Camera.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Camera::Camera()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera::setRenderTarget( const String& _renderTarget )
	{
		m_renderTarget = _renderTarget;
	}
}	// namespace Menge
