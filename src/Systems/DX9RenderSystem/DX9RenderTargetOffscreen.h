#pragma once

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "DX9RenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderTargetOffscreen
        : public DX9RenderTargetTexture
    {
        DECLARE_FACTORABLE( DX9RenderTargetOffscreen );

    public:
        DX9RenderTargetOffscreen();
        ~DX9RenderTargetOffscreen() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    protected:
        IDirect3DSurface9 * m_pD3DSurfacePlain;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderTargetOffscreen, RenderTargetInterface> DX9RenderTargetOffscreenPtr;
    //////////////////////////////////////////////////////////////////////////
}