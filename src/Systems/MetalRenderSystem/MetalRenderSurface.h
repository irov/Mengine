#pragma once

#include "Interface/RenderSurfaceInterface.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderSurface
        : public RenderSurfaceInterface
    {
        DECLARE_FACTORABLE( MetalRenderSurface );

    public:
        MetalRenderSurface();
        ~MetalRenderSurface() override;

    public:
        bool initialize( id<MTLDevice> _device, void * _nativeHandle, const Resolution & _resolution, float _dpiScale );
        void finalize();

    public:
        const Resolution & getResolution() const override;
        float getDpiScale() const override;

    public:
        bool resize( const Resolution & _resolution, float _dpiScale ) override;
        void setVSync( bool _vsync ) override;

    public:
        bool acquire();
        void release();

    public:
        bool present( id<MTLCommandBuffer> _commandBuffer );

    public:
        id<MTLTexture> getColorTexture() const;
        id<MTLTexture> getDepthTexture() const;

    protected:
        bool updateDepthTarget_();

    protected:
        id<MTLDevice> m_device;
        CAMetalLayer * m_layer;
        id<CAMetalDrawable> m_drawable;
        id<MTLTexture> m_depthTarget;
        Resolution m_resolution;
        float m_dpiScale;
        bool m_waitForVSync;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderSurface, RenderSurfaceInterface> MetalRenderSurfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
