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
    public:
        DX11RenderTargetTexture();
        ~DX11RenderTargetTexture() override;

    public:
        bool initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format );
        void finalize();

    protected:
        virtual bool _initialize();
        virtual void _finalize();

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
        uint32_t getHWDepth() const override;
        EPixelFormat getHWPixelFormat() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        void calcViewport( const mt::vec2f & _size, Viewport * const _viewport ) const override;

    public:
        bool begin() override;
        void end() override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    public:
        ID3D11Device * getDirect3dDevice11() const;
		ID3D11Texture2D * getD3DTexture() const;
		ID3D11ShaderResourceView * getD3DShaderResource() const;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
		ID3D11Texture2D * m_pD3DTexture;
		ID3D11ShaderResourceView* m_pD3DResourceView;
		// interface for render target
		ID3D11RenderTargetView* m_pRenderTargetView;

		D3D11_TEXTURE2D_DESC m_textureDesc;

		EPixelFormat m_hwPixelFormat;

		uint32_t m_hwChannels;

		float m_hwWidthInv;
		float m_hwHeightInv;

		// bind
		ID3D11RenderTargetView* m_pRenderTargetViewOld;
		ID3D11DepthStencilView* m_pDepthStencilMain;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderTargetTexture, RenderTargetInterface> DX11RenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}