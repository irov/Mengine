/*
 *	DX9RenderTexture.cpp
 *
 *	Created by _Berserk_ on 23.05.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DX9RenderTexture.h"
#	include <d3d9.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9RenderTexture::DX9RenderTexture( IDirect3DSurface9* _d3dInterface, IDirect3DSurface9* _depthInterface )
		: DX8Texture( _d3dInterface )
		, m_dirty( true )
		, m_depthInterface( _depthInterface )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderTexture::~DX9RenderTexture()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderTexture::isDirty() const
	{
		return m_dirty;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTexture::setDirty( bool _dirty )
	{
		m_dirty = _dirty;
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DSurface8* DX9RenderTexture::getDepthInterface()
	{
		return m_depthInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTexture::setTexInterface( IDirect3DSurface9* _d3dInterface )
	{
		m_d3dInterface = _d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTexture::setDepthInterface( IDirect3DSurface9* _d3dInterface )
	{
		m_depthInterface = _d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
