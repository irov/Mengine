#	pragma once

#	include "DX8Texture.h"

namespace Menge
{
	class DX8RenderTexture
		: public DX8Texture
	{
	public:
		DX8RenderTexture();
		~DX8RenderTexture();

    public:
        void setSurface( IDirect3DSurface8 * _depthInterface );

	public:
		bool isDirty() const;
		void setDirty( bool _dirty );

	public:
		IDirect3DSurface8* getDepthInterface();

	public:
		void setTexInterface( IDirect3DTexture8* _d3dInterface );
		void setDepthInterface( IDirect3DSurface8* _d3dInterface );

	public:
		bool _destroy() override;

	protected:
		bool m_dirty;
		IDirect3DSurface8* m_depthInterface;
	};

    typedef IntrusivePtr<DX8RenderTexture> DX8RenderTexturePtr;
}	// namespace Menge
