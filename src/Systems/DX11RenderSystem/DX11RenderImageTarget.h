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
        , public DX11RenderResourceHandler
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
        void bind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage ) override;
        void unbind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage ) override;

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
        RenderImageLockedInterfacePtr lock( uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful ) override;

    public:
        const ID3D11DevicePtr & getD3DDevice() const override;
        const ID3D11Texture2DPtr & getD3DTexture() const override;
        const ID3D11ShaderResourceViewPtr & getD3DShaderResource() const override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        DX11RenderTargetTexturePtr m_target;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderImageTarget, RenderImageInterface> DX11RenderImageTargetPtr;
    //////////////////////////////////////////////////////////////////////////
}
