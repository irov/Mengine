#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/DX11RenderImageExtensionInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderTargetTexture.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderImageTarget
        : public RenderImageInterface
        , public DX11RenderImageExtensionInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderImageTarget );
        DECLARE_UNKNOWABLE();

    public:
        DX11RenderImageTarget();
        ~DX11RenderImageTarget() override;

    public:
        void initialize( const DX11RenderTargetTexturePtr & _target );
        void finalize();

    public:
        void bind( ID3D11DeviceContext * _pD3DImmediateContext, uint32_t _stage );
        void unbind( ID3D11DeviceContext * _pD3DImmediateContext, uint32_t _stage );

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
        Pointer lock( size_t * const _pitch, uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( uint32_t _level, bool _successful ) override;

    public:
        UnknownPointer getRenderImageExtention() override;

    public:
        ID3D11Device * getD3DDevice() const override;
        ID3D11Texture2D * getD3DTexture() const override;
        ID3D11ShaderResourceView * getD3DShaderResource() const override;

    protected:
        DX11RenderTargetTexturePtr m_target;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderImageTarget, RenderImageInterface> DX11RenderImageTargetPtr;
    //////////////////////////////////////////////////////////////////////////
}
