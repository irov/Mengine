#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/DX9RenderImageExtensionInterface.h"

#include "Environment/DirectX9/DirectX9RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class DX9RenderTargetTexture, class RenderTargetInterface> DX9RenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderImageTarget
        : public RenderImageInterface
        , public DX9RenderImageExtensionInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX9RenderImageTarget );
        DECLARE_UNKNOWABLE();

    public:
        DX9RenderImageTarget();
        ~DX9RenderImageTarget() override;

    public:
        void initialize( const DX9RenderTargetTexturePtr & _target );
        void finalize();

    public:
        void bind( uint32_t _stage );
        void unbind( uint32_t _stage );

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
        UnknownPointer getRenderImageExtention() override;

    public:
        IDirect3DDevice9 * getD3DDevice() const override;
        IDirect3DTexture9 * getD3DTexture() const override;

    protected:
        DX9RenderTargetTexturePtr m_target;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderImageTarget, RenderImageInterface> DX9RenderImageTargetPtr;
    //////////////////////////////////////////////////////////////////////////
}
