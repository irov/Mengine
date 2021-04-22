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
    public:
        DX11RenderTargetOffscreen();
        ~DX11RenderTargetOffscreen() override;

    public:
		bool initialize(DX11RenderTargetTexturePtr _renderTargetTexture);
		bool initialize(ID3D11Texture2D * _textureSource);
		void finalize();

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    protected:
		ID3D11Texture2D * m_pD3DTexture;

		ID3D11Texture2D * m_pD3DTextureSource;
		DX11RenderTargetTexturePtr m_renderTargetTexture;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderTargetOffscreen, RenderTargetInterface> DX11RenderTargetOffscreenPtr;
    //////////////////////////////////////////////////////////////////////////
}