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
		DX8RenderTexture( IDirect3DTexture8* _d3dInterface, IDirect3DSurface8* _depthInterface );
		~DX8RenderTexture();

		bool isDirty() const;
		void setDirty( bool _dirty );

		IDirect3DSurface8* getDepthInterface();

		void setTexInterface( IDirect3DTexture8* _d3dInterface );
		void setDepthInterface( IDirect3DSurface8* _d3dInterface );

	protected:
		bool m_dirty;
		IDirect3DSurface8* m_depthInterface;
	};
}	// namespace Menge
