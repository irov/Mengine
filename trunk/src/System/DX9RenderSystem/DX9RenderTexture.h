#	pragma once

#	include "DX9Texture.h"

namespace Menge
{
	class DX9RenderTexture
		: public DX9Texture
	{
	public:
		DX9RenderTexture();
		~DX9RenderTexture();

    public:
        void setSurface( IDirect3DSurface9 * _depthInterface );

	public:
		bool isDirty() const;
		void setDirty( bool _dirty );

	public:
		IDirect3DSurface9 * getDepthInterface();

	public:
		void setTexInterface( IDirect3DTexture9 * _d3dInterface );
		void setDepthInterface( IDirect3DSurface9 * _d3dInterface );

	protected:
		bool m_dirty;
		IDirect3DSurface9 * m_depthInterface;
	};

    typedef IntrusivePtr<DX9RenderTexture> DX8RenderTexturePtr;
}
