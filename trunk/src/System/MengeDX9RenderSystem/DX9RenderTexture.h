/*
 *	DX9RenderTexture.h
 *
 *	Created by _Berserk_ on 23.05.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "DX9Texture.h"

struct IDirect3DSurface9;

namespace Menge
{
	class DX9RenderTexture
		: public DX9Texture
	{
	public:
		DX9RenderTexture( IDirect3DTexture9* _d3dInterface, IDirect3DSurface9* _depthInterface );
		~DX9RenderTexture();

		bool isDirty() const;
		void setDirty( bool _dirty );
		void release() override;
		IDirect3DSurface9* getDepthInterface();

		void setTexInterface( IDirect3DTexture9* _d3dInterface );
		void setDepthInterface( IDirect3DSurface9* _d3dInterface );

	protected:
		bool m_dirty;
		IDirect3DSurface9* m_depthInterface;
	};
}	// namespace Menge
