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
		: m_debugMask(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera::setRenderTarget( const String& _renderTarget )
	{
		m_renderTarget = _renderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera::setDebugMask( unsigned int _debugMask )
	{
		m_debugMask = _debugMask;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Camera::getDebugMask() const
	{
		return m_debugMask;
	}
}	// namespace Menge
