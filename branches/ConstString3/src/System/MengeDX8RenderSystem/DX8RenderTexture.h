/*
 *	DX8RenderTexture.h
 *
 *	Created by _Berserk_ on 23.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "DX8Texture.h"

struct IDirect3DSurface8;

namespace Menge
{
	class DX8RenderTexture
		: public DX8Texture
	{
	public:
		DX8RenderTexture( IDirect3DTexture8* _d3dInterface, size_t _hwWidth, size_t _hwHeight, PixelFormat _hwPixelFormat, IDirect3DSurface8* _depthInterface );
		~DX8RenderTexture();

	public:
		bool isDirty() const;
		void setDirty( bool _dirty );

	public:
		IDirect3DSurface8* getDepthInterface();

	public:
		void setTexInterface( IDirect3DTexture8* _d3dInterface );
		void setDepthInterface( IDirect3DSurface8* _d3dInterface );

	public:
		void release() override;

	protected:
		bool m_dirty;
		IDirect3DSurface8* m_depthInterface;
	};
}	// namespace Menge
