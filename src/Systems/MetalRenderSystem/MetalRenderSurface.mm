#include "MetalRenderSurface.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderSurface::MetalRenderSurface()
        : m_device( nil )
        , m_layer( nil )
        , m_drawable( nil )
        , m_depthTarget( nil )
        , m_dpiScale( 1.f )
        , m_waitForVSync( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderSurface::~MetalRenderSurface()
    {
        MENGINE_ASSERTION_FATAL( m_layer == nil, "layer is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSurface::initialize( id<MTLDevice> _device, void * _nativeHandle, const Resolution & _resolution, float _dpiScale )
    {
        if( _device == nil )
        {
            return false;
        }

        if( _nativeHandle == nullptr )
        {
            return false;
        }

        CAMetalLayer * layer = (__bridge CAMetalLayer *)_nativeHandle;

        if( [layer isKindOfClass:[CAMetalLayer class]] == NO )
        {
            return false;
        }

        m_device = _device;
        m_layer = layer;
        m_resolution = _resolution;
        m_dpiScale = _dpiScale;
        m_layer.device = m_device;
        m_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        m_layer.framebufferOnly = YES;
        this->setVSync( m_waitForVSync );

        if( this->resize( m_resolution, m_dpiScale ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSurface::finalize()
    {
        this->release();
        m_depthTarget = nil;
        m_layer = nil;
        m_device = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    const Resolution & MetalRenderSurface::getResolution() const
    {
        return m_resolution;
    }
    //////////////////////////////////////////////////////////////////////////
    float MetalRenderSurface::getDpiScale() const
    {
        return m_dpiScale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSurface::resize( const Resolution & _resolution, float _dpiScale )
    {
        if( m_layer == nil )
        {
            return false;
        }

        uint32_t width = _resolution.getWidth();
        uint32_t height = _resolution.getHeight();

        if( width == 0 || height == 0 )
        {
            return false;
        }

        m_resolution = _resolution;
        m_dpiScale = _dpiScale;
        m_layer.contentsScale = m_dpiScale;
        m_layer.drawableSize = CGSizeMake( width, height );
        m_depthTarget = nil;

        if( this->updateDepthTarget_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSurface::setVSync( bool _vsync )
    {
        m_waitForVSync = _vsync;

#if TARGET_OS_OSX
        if( [m_layer respondsToSelector:@selector(setDisplaySyncEnabled:)] == YES )
        {
            m_layer.displaySyncEnabled = m_waitForVSync ? YES : NO;
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSurface::acquire()
    {
        if( m_layer == nil )
        {
            return false;
        }

        if( this->updateDepthTarget_() == false )
        {
            return false;
        }

        m_drawable = [m_layer nextDrawable];

        return m_drawable != nil;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSurface::present( id<MTLCommandBuffer> _commandBuffer )
    {
        if( _commandBuffer == nil )
        {
            return false;
        }

        if( m_drawable == nil )
        {
            return false;
        }

        [_commandBuffer presentDrawable:m_drawable];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSurface::release()
    {
        m_drawable = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLTexture> MetalRenderSurface::getColorTexture() const
    {
        if( m_drawable == nil )
        {
            return nil;
        }

        return m_drawable.texture;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLTexture> MetalRenderSurface::getDepthTexture() const
    {
        return m_depthTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSurface::updateDepthTarget_()
    {
        NSUInteger width = static_cast<NSUInteger>(m_layer.drawableSize.width);
        NSUInteger height = static_cast<NSUInteger>(m_layer.drawableSize.height);

        if( width == 0 )
        {
            return false;
        }

        if( height == 0 )
        {
            return false;
        }

        if( m_depthTarget != nil && m_depthTarget.width == width && m_depthTarget.height == height )
        {
            return true;
        }

        MTLTextureDescriptor * descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8 width:width height:height mipmapped:NO];
        descriptor.usage = MTLTextureUsageRenderTarget;
        descriptor.storageMode = MTLStorageModePrivate;
        m_depthTarget = [m_device newTextureWithDescriptor:descriptor];

        return m_depthTarget != nil;
    }
    //////////////////////////////////////////////////////////////////////////
}
