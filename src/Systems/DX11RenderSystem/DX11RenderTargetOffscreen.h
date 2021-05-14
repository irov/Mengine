#pragma once

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

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
        bool initialize( DX11RenderTargetTexturePtr _renderTargetTexture );
        bool initialize( ID3D11Texture2D * _textureSource );
        void finalize();

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
        uint32_t getHWDepth() const override;
        EPixelFormat getHWPixelFormat() const override;

        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        bool begin() const override;
        void end() const override;

    public:
        void calcViewport( const mt::vec2f & _size, Viewport * const _viewport ) const override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        ID3D11Texture2D * m_pD3DTexture;
        D3D11_TEXTURE2D_DESC m_textureDesc;

        ID3D11Texture2D * m_pD3DTextureSource;
        DX11RenderTargetTexturePtr m_renderTargetTexture;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderTargetOffscreen, RenderTargetInterface> DX11RenderTargetOffscreenPtr;
    //////////////////////////////////////////////////////////////////////////
}