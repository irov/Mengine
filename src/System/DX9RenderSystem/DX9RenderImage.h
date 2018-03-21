#pragma once

#include "Interface/RenderSystemInterface.h"

#include "Core/ServantBase.h"

#include <d3d9.h>

namespace Mengine
{
	class DX9RenderImage
		: public ServantBase<RenderImageInterface>
	{
	public:
		DX9RenderImage();
        ~DX9RenderImage() override;

    public:
        void initialize( LPDIRECT3DTEXTURE9 _d3dInterface, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, PixelFormat _hwPixelFormat );
		
	protected:
		void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
		const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

	public:
        LPDIRECT3DTEXTURE9 getDXTextureInterface() const;

	public:
		ERenderImageMode getMode() const override;

	public:
		uint32_t getHWWidth() const override;
		uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
		uint32_t getHWDepth() const override;
		uint32_t getHWMipmaps() const override;
		
        PixelFormat getHWPixelFormat() const override;

	public:
        Pointer lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly ) override;
		bool unlock( uint32_t _level, bool _successful ) override;
		               		
	protected:
        LPDIRECT3DTEXTURE9 m_d3dTexture;

		RenderImageProviderInterfacePtr m_renderImageProvider;

		ERenderImageMode m_mode;

		uint32_t m_hwMipmaps;
		uint32_t m_hwWidth;
		uint32_t m_hwHeight;
        uint32_t m_hwChannels;

        PixelFormat m_hwPixelFormat;
	};
	//////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<DX9RenderImage> DX9RenderImagePtr;
}	
