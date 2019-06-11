#pragma once

#include "DX9RenderTargetTexture.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderTargetOffscreen
        : public DX9RenderTargetTexture
    {
    public:
        DX9RenderTargetOffscreen();
        ~DX9RenderTargetOffscreen() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool getData( void * _buffer, size_t _pitch ) override;

    protected:
        LPDIRECT3DSURFACE9 m_surfacePlain;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderTargetOffscreen> DX9RenderTargetOffscreenPtr;
}