#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/DX11RenderImageExtensionInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderImage
        : public RenderImageInterface
        , public DX11RenderImageExtensionInterface
        , public Factorable
        , public DX11RenderResourceHandler
    {
        DECLARE_FACTORABLE( DX11RenderImage );
        DECLARE_UNKNOWABLE();

    public:
        DX11RenderImage();
        ~DX11RenderImage() override;

    public:
        bool initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _pixelFormat );
        void finalize();

    public:
        void bind( uint32_t _stage ) override;
        void unbind( uint32_t _stage ) override;

    protected:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
        uint32_t getHWDepth() const override;
        uint32_t getHWMipmaps() const override;

        EPixelFormat getHWPixelFormat() const override;

    protected:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        Pointer lock(size_t * const _pitch, uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( uint32_t _level, bool _successful ) override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    public:
        UnknownPointer getRenderImageExtention() override;

    public:
        ID3D11Device * getD3DDevice() const override;
		ID3D11Texture2D * getD3DTexture() const override;
		ID3D11ShaderResourceView * getD3DShaderResource() const override;

    protected:
		ID3D11Texture2D * m_pD3DTexture;
		// to bind texture to shaders
		ID3D11ShaderResourceView* m_pD3DResourceView;

        RenderImageProviderInterfacePtr m_renderImageProvider;

		D3D11_TEXTURE2D_DESC m_textureDesc;

        EPixelFormat m_hwPixelFormat;

		uint32_t m_hwChannels;
		uint32_t m_hwDepth;

        float m_hwWidthInv;
        float m_hwHeightInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderImage, RenderImageInterface> DX11RenderImagePtr;
    //////////////////////////////////////////////////////////////////////////
}
