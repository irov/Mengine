#pragma once

#include "Interface/RenderImageInterface.h"
#include "Kernel/Factorable.h"
#include "MetalRenderTypes.h"

#include "Kernel/Rect.h"

namespace Mengine
{
    class MetalRenderImage
        : public RenderImageInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderImage );

    public:
        MetalRenderImage( MetalDeviceId _device );
        ~MetalRenderImage() override;
        bool initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, bool _upscalePow2 );
        bool initializeFromTexture( id<MTLTexture> _texture, uint32_t _width, uint32_t _height, EPixelFormat _format );
        void finalize() override;

    public:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        EPixelFormat getHWPixelFormat() const override;
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;
        bool getUpscalePow2() const override;

        RenderImageLockedInterfacePtr lock( uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( const RenderImageLockedInterfacePtr & _lock, uint32_t _level, bool _successful ) override;

    public:
        id<MTLTexture> getTexture() const;

    protected:
        RenderImageProviderInterfacePtr m_renderImageProvider;
        MetalDeviceId m_device;
        id<MTLTexture> m_texture;
        uint32_t m_mipmaps;
        uint32_t m_width;
        uint32_t m_height;
        float m_hwWidthInv;
        float m_hwHeightInv;
        EPixelFormat m_format;
        bool m_upscalePow2;
    };

    typedef IntrusivePtr<MetalRenderImage, RenderImageInterface> MetalRenderImagePtr;
}

