#pragma once

#include "Environment/DirectX11/DX11RenderIncluder.h"

#include "DX11RenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderTargetOffscreen
        : public RenderTargetInterface
        , public Factorable
        , public DX11RenderResourceHandler
    {
        DECLARE_FACTORABLE( DX11RenderTargetOffscreen );

    public:
        DX11RenderTargetOffscreen();
        ~DX11RenderTargetOffscreen() override;

    public:
        bool initialize( uint32_t _width, uint32_t _height, ID3D11Texture2D * _textureSource );
        void finalize();

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        EPixelFormat getHWPixelFormat() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        bool getUpscalePow2() const override;

    public:
        bool begin() const override;
        void end() const override;

    public:
        const mt::uv4f & getUV() const override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        ID3D11Texture2DPtr m_pD3DTexture;
        D3D11_TEXTURE2D_DESC m_textureDesc;

        ID3D11Texture2DPtr m_pD3DTextureSource;
        DX11RenderTargetTexturePtr m_renderTargetTexture;

        float m_hwWidthInv;
        float m_hwHeightInv;

        mt::uv4f m_uv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderTargetOffscreen, RenderTargetInterface> DX11RenderTargetOffscreenPtr;
    //////////////////////////////////////////////////////////////////////////
}