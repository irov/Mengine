#include "MetalRenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderTargetTexture::MetalRenderTargetTexture( MetalDeviceId _device )
        : m_device( _device )
        , m_texture( nil )
        , m_width( 0 )
        , m_height( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_format( PF_UNKNOWN )
        , m_uv( mt::uv4f::identity() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderTargetTexture::~MetalRenderTargetTexture() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::initialize( uint32_t _width, uint32_t _height, EPixelFormat _format )
    {
        m_width = _width;
        m_height = _height;
        m_format = _format;
        m_hwWidthInv = 1.f / (float)_width;
        m_hwHeightInv = 1.f / (float)_height;

        MTLPixelFormat pixelFormat = MTLPixelFormatRGBA8Unorm;

        MTLTextureDescriptor * descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat width:_width height:_height mipmapped:NO];
        descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;

        m_texture = [m_device newTextureWithDescriptor:descriptor];

        return m_texture != nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderTargetTexture::finalize()
    {
        m_texture = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderTargetTexture::getHWMipmaps() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderTargetTexture::getHWWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderTargetTexture::getHWHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MetalRenderTargetTexture::getHWPixelFormat() const
    {
        return m_format;
    }
    //////////////////////////////////////////////////////////////////////////
    float MetalRenderTargetTexture::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float MetalRenderTargetTexture::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::getUpscalePow2() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::begin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderTargetTexture::end() const
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & MetalRenderTargetTexture::getUV() const
    {
        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        if( m_texture == nil )
        {
            return false;
        }

        MTLRegion region = MTLRegionMake2D( 0, 0, m_width, m_height );
        [m_texture getBytes:_buffer bytesPerRow:_pitch fromRegion:region mipmapLevel:0];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLTexture> MetalRenderTargetTexture::getTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    MetalDeviceId MetalRenderTargetTexture::getDevice() const
    {
        return m_device;
    }
    //////////////////////////////////////////////////////////////////////////
}

