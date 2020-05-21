#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/DX9RenderImageExtensionInterface.h"

#include "DX9RenderResourceHandler.h"

#include "Kernel/Factorable.h"

#include "d3d9.h"

namespace Mengine
{
    typedef IntrusivePtr<class DX9RenderTargetTexture> DX9RenderTargetTexturePtr;

    class DX9RenderImageTarget
        : public RenderImageInterface
        , public DX9RenderImageExtensionInterface
        , public Factorable
    {
    public:
        DX9RenderImageTarget();
        ~DX9RenderImageTarget() override;

    public:
        void initialize( const DX9RenderTargetTexturePtr & _target );
        void finalize();

    public:
        void bind( uint32_t _stage ) override;

    protected:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        ERenderImageMode getMode() const override;

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
        Pointer lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( uint32_t _level, bool _successful ) override;

    public:
        UnknownPointer getRenderImageExtention() override;

    public:
        IDirect3DDevice9 * getD3DDevice() const override;
        IDirect3DTexture9 * getD3DTexture() const override;

    protected:
        DX9RenderTargetTexturePtr m_target;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderImageTarget> DX9RenderImageTargetPtr;
}
