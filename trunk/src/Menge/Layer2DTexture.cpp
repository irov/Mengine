/*
 *	Layer2DTexture.cpp
 *
 *	Created by _Berserk_ on 25.2.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#include "Layer2DTexture.h"

#	include "ObjectImplement.h"

#	include "Camera2D.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Layer2DTexture);
	//////////////////////////////////////////////////////////////////////////
	Layer2DTexture::Layer2DTexture()
		: m_viewportSize( 0.0f, 0.0f )
		, m_cameraOffset( 0.0f, 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Layer2DTexture::~Layer2DTexture()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DTexture::loader( XmlElement * _xml )
	{
		Layer2D::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "RenderTargetName", "Value", &Layer2DTexture::setRenderTargetName );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "ViewportSize", "Value", &Layer2DTexture::setViewportSize );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DTexture::render( Camera2D * _camera )
	{
		const String& oldTarget = RenderEngine::hostage()->getRenderTarget();
		RenderEngine::hostage()
			->setRenderTarget( m_renderTargetName, true );
		Layer2D::render( _camera );
		RenderEngine::hostage()
			->setRenderTarget( oldTarget, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DTexture::setRenderTargetName( const String& _name )
	{
		m_renderTargetName = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Layer2DTexture::getRenderTargetName() const
	{
		return m_renderTargetName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DTexture::setViewportSize( const mt::vec2f& _size )
	{
		m_viewportSize = _size;
		if( m_camera2D != NULL )
		{
			m_camera2D->setViewportSize( m_viewportSize );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Layer2DTexture::getViewportSize() const
	{
		return m_viewportSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DTexture::setCameraOffset( const mt::vec2f _offset )
	{
		m_cameraOffset = _offset;
		if( m_camera2D != NULL )
		{
			m_camera2D->setOffset( m_cameraOffset );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Layer2DTexture::getCameraOffset() const
	{
		return m_cameraOffset;
	}
	////////////////////////////////////////////////////////////////////////////
	bool Layer2DTexture::_activate()
	{
		if( Layer2D::_activate() == false )
		{
			return false;
		}

		m_camera2D->setViewportSize( m_viewportSize );
		m_camera2D->setOffset( m_cameraOffset );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
