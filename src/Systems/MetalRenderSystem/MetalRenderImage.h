#pragma once

#include "Interface/RenderImageInterface.h"

#include "Environment/Metal/MetalRenderImageExtensionInterface.h"

#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"

#include "Config/Timestamp.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderImage
        : public RenderImageInterface
        , public MetalRenderImageExtensionInterface
        , public MetalRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( RenderImageInterface );
        DECLARE_UNKNOWABLE();

    public:
        MetalRenderImage();
        ~MetalRenderImage() override;

    public:
        bool initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat );
        void finalize();

    public:
        void bind( uint32_t _stage ) override;
        void unbind( uint32_t _stage ) override;

    public:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        bool create();
        void release();

    public:
        RenderImageLockedInterfacePtr lock( uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful ) override;

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;

    public:
        EPixelFormat getHWPixelFormat() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        bool getUpscalePow2() const override;

    public:
        id<MTLTexture> getMetalTexture() const override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        RenderImageProviderInterfacePtr m_renderImageProvider;

        Timestamp m_createTimestamp;

        EPixelFormat m_hwPixelFormat;

        uint32_t m_width;
        uint32_t m_height;

        uint32_t m_hwMipmaps;
        uint32_t m_hwWidth;
        uint32_t m_hwHeight;

        float m_hwWidthInv;
        float m_hwHeightInv;

        id<MTLTexture> m_texture;

        bool m_lockFirst;
        bool m_pow2;
        bool m_upscalePow2;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderImage, RenderImageInterface> MetalRenderImagePtr;
    //////////////////////////////////////////////////////////////////////////
}
