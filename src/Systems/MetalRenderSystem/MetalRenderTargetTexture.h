#pragma once

#include "Interface/RenderTargetInterface.h"
#include "Kernel/Factorable.h"
#include "math/uv4.h"
#include "MetalRenderTypes.h"

namespace Mengine
{
    class MetalRenderTargetTexture
        : public RenderTargetInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderTargetTexture );

    public:
        MetalRenderTargetTexture( MetalDeviceId _device );
        ~MetalRenderTargetTexture() override;
        bool initialize( uint32_t _width, uint32_t _height, EPixelFormat _format );
        void finalize();

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        EPixelFormat getHWPixelFormat() const override;
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;
        bool getUpscalePow2() const override;

        bool begin() const override;
        void end() const override;

        const mt::uv4f & getUV() const override;

        bool getData( void * const _buffer, size_t _pitch ) const override;

    public:
        id<MTLTexture> getTexture() const;
        MetalDeviceId getDevice() const;

    protected:
        MetalDeviceId m_device;
        id<MTLTexture> m_texture;
        uint32_t m_width;
        uint32_t m_height;
        float m_hwWidthInv;
        float m_hwHeightInv;
        EPixelFormat m_format;
        mt::uv4f m_uv;
    };

    typedef IntrusivePtr<MetalRenderTargetTexture, RenderTargetInterface> MetalRenderTargetTexturePtr;
}

