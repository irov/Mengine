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
	DX8RenderTexture::DX8RenderTexture( IDirect3DTexture8* _d3dInterface, size_t _hwWidth, size_t _hwHeight, size_t _hwChannels, PixelFormat _hwPixelFormat, IDirect3DSurface8* _depthInterface )
		: DX8Texture(_d3dInterface, _hwWidth, _hwHeight, _hwChannels, _hwPixelFormat)
		, m_depthInterface(_depthInterface)
		, m_dirty(true)
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
	void DX8RenderTexture::release()
	{
        DX8Texture::release();

		if( m_depthInterface )
		{
			m_depthInterface->Release();
		}
	}
}	// namespace Menge
