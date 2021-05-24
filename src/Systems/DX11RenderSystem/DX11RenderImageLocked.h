#pragma once

#include "Interface/RenderImageLockedInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderImageLocked
        : public RenderImageLockedInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderImageLocked );

    public:
        DX11RenderImageLocked();
        ~DX11RenderImageLocked();

    public:
        bool initialize( const ID3D11DevicePtr & _pD3DDevice, const ID3D11DeviceContextPtr & _pImmediateContext, const ID3D11Texture2DPtr & _pMainTexture, uint32_t _offsetX, uint32_t _offsetY, uint32_t _width, uint32_t _height );
        void finalize();

    public:
        const ID3D11Texture2DPtr & getStagingTexture() const;
        uint32_t getStagingOffsetX() const;
        uint32_t getStagingOffsetY() const;

    public:
        Pointer getBuffer( size_t * const _pitch ) const override;

    protected:
        D3D11_MAPPED_SUBRESOURCE m_stagingTextureMemory;

        ID3D11Texture2DPtr m_pD3DStagingTexture;
        uint32_t m_stagingOffsetX;
        uint32_t m_stagingOffsetY;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderImageLocked, RenderImageLockedInterface> DX11RenderImageLockedPtr;
    //////////////////////////////////////////////////////////////////////////
}