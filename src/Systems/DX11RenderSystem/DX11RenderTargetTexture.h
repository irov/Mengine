#pragma once

#include "Interface/RenderTargetInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderTargetTexture
        : public RenderTargetInterface
        , public Factorable
        , public DX11RenderResourceHandler
    {
        DECLARE_FACTORABLE( DX11RenderTargetTexture );

    public:
        DX11RenderTargetTexture();
        ~DX11RenderTargetTexture() override;

    public:
        bool initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format );
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
        void calcViewport( const mt::vec2f & _size, Viewport * const _viewport ) const override;

    public:
        bool begin() const override;
        void end() const override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    public:
        const ID3D11DevicePtr & getDirect3dDevice11() const;
        const ID3D11Texture2DPtr & getD3DTexture() const;
        const ID3D11ShaderResourceViewPtr & getD3DShaderResource() const;

        const D3D11_TEXTURE2D_DESC & GetTextureDesc() const;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        ID3D11Texture2DPtr m_pD3DTexture;
        ID3D11ShaderResourceViewPtr m_pD3DResourceView;

        ID3D11RenderTargetViewPtr m_pRenderTargetView;
        mutable ID3D11RenderTargetViewPtr m_pRenderTargetViewOld;
        ID3D11DepthStencilViewPtr m_pDepthStencilMain;
        mutable ID3D11DepthStencilViewPtr m_pDepthStencilMainOld;

        D3D11_TEXTURE2D_DESC m_textureDesc;

        EPixelFormat m_hwPixelFormat;

        uint32_t m_hwChannels;

        float m_hwWidthInv;
        float m_hwHeightInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderTargetTexture, RenderTargetInterface> DX11RenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}