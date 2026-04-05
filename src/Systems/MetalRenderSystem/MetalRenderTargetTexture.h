#pragma once

#include "Interface/RenderTargetInterface.h"

#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderTargetTexture
        : public RenderTargetInterface
        , public MetalRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderTargetTexture );

    public:
        MetalRenderTargetTexture();
        ~MetalRenderTargetTexture() override;

    public:
        bool initialize( uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat );
        void finalize();

    public:
        bool create();
        void release();

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
        bool begin() const override;
        void end() const override;

    public:
        const mt::uv4f & getUV() const override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    public:
        id<MTLTexture> getMetalTexture() const;

    public:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        uint32_t m_width;
        uint32_t m_height;

        uint32_t m_hwWidth;
        uint32_t m_hwHeight;

        float m_hwWidthInv;
        float m_hwHeightInv;

        mt::uv4f m_uv;

        EPixelFormat m_hwPixelFormat;

        id<MTLTexture> m_texture;

        bool m_pow2;
        bool m_upscalePow2;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderTargetTexture, RenderTargetInterface> MetalRenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}
