#pragma once

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderTargetOffscreen
        : public DX11RenderTargetTexture
    {
    public:
        DX11RenderTargetOffscreen();
        ~DX11RenderTargetOffscreen() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    protected:
        IDirect3DSurface9 * m_pD3DSurfacePlain;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderTargetOffscreen, RenderTargetInterface> DX11RenderTargetOffscreenPtr;
    //////////////////////////////////////////////////////////////////////////
}