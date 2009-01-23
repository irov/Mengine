/*
 *	DX8RenderTexture.cpp
 *
 *	Created by _Berserk_ on 23.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DX8RenderTexture.h"
#	include <d3d8.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX8RenderTexture::DX8RenderTexture( IDirect3DTexture8* _d3dInterface, IDirect3DSurface8* _depthInterface, 
		const String& _name, std::size_t _width, std::size_t _height, PixelFormat _format )
		: DX8Texture( _d3dInterface, _name, _width, _height, _format )
		, m_dirty( true )
		, m_depthInterface( _depthInterface )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX8RenderTexture::~DX8RenderTexture()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderTexture::isDirty() const
	{
		return m_dirty;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderTexture::setDirty( bool _dirty )
	{
		m_dirty = _dirty;
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DSurface8* DX8RenderTexture::getDepthInterface()
	{
		return m_depthInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderTexture::setTexInterface( IDirect3DTexture8* _d3dInterface )
	{
		m_d3dInterface = _d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderTexture::setDepthInterface( IDirect3DSurface8* _d3dInterface )
	{
		m_depthInterface = _d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
