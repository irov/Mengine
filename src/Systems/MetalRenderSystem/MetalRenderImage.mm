#include "MetalRenderImage.h"
#include "MetalRenderImageLocked.h"

#include "Kernel/PixelFormatHelper.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderImage::MetalRenderImage( MetalDeviceId _device )
        : m_renderImageProvider( nullptr )
        , m_device( _device )
        , m_texture( nil )
        , m_mipmaps( 0 )
        , m_width( 0 )
        , m_height( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_format( PF_UNKNOWN )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderImage::~MetalRenderImage() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, bool _upscalePow2 )
    {
        m_mipmaps = _mipmaps;
        m_width = _width;
        m_height = _height;
        m_format = _format;
        m_upscalePow2 = _upscalePow2;
        m_hwWidthInv = 1.f / (float)_width;
        m_hwHeightInv = 1.f / (float)_height;

        MTLPixelFormat pixelFormat = MTLPixelFormatRGBA8Unorm;

        MTLTextureDescriptor * descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat width:_width height:_height mipmapped:(_mipmaps > 1 ? YES : NO)];
        descriptor.usage = MTLTextureUsageShaderRead;

        m_texture = [m_device newTextureWithDescriptor:descriptor];

        return m_texture != nil;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImage::initializeFromTexture( id<MTLTexture> _texture, uint32_t _width, uint32_t _height, EPixelFormat _format )
    {
        m_texture = _texture;
        m_mipmaps = 1;
        m_width = _width;
        m_height = _height;
        m_format = _format;
        m_upscalePow2 = false;
        m_hwWidthInv = 1.f / (float)_width;
        m_hwHeightInv = 1.f / (float)_height;

        return m_texture != nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImage::finalize()
    {
        m_renderImageProvider = nullptr;
        m_texture = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImage::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        m_renderImageProvider = _renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & MetalRenderImage::getRenderImageProvider() const
    {
        return m_renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImage::getHWMipmaps() const
    {
        return m_mipmaps;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImage::getHWWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImage::getHWHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MetalRenderImage::getHWPixelFormat() const
    {
        return m_format;
    }
    //////////////////////////////////////////////////////////////////////////
    float MetalRenderImage::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float MetalRenderImage::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImage::getUpscalePow2() const
    {
        return m_upscalePow2;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLockedInterfacePtr MetalRenderImage::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _readOnly );

        uint32_t rectWidth = _rect.getWidth();
        uint32_t rectHeight = _rect.getHeight();

        uint32_t miplevelWidth = rectWidth >> _level;
        uint32_t miplevelHeight = rectHeight >> _level;

        size_t size = Helper::getTextureMemorySize( miplevelWidth, miplevelHeight, m_format );
        size_t pitch = size / miplevelHeight;

        MetalRenderImageLockedPtr locked = Helper::makeFactorableUnique<MetalRenderImageLocked>( MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( locked, "invalid create image locked" );

        locked->initialize( size, pitch, _rect );

        return locked;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImage::unlock( const RenderImageLockedInterfacePtr & _lock, uint32_t _level, bool _successful )
    {
        if( _successful == false )
        {
            return true;
        }

        if( m_texture == nil )
        {
            return false;
        }

        MetalRenderImageLocked * locked = _lock.getT<MetalRenderImageLocked *>();

        const Rect & lockedRect = locked->getLockedRect();

        size_t pitch;
        void * buffer = locked->getBuffer( &pitch );

        uint32_t lockedWidth = lockedRect.getWidth();
        uint32_t lockedHeight = lockedRect.getHeight();

        MTLRegion region = MTLRegionMake2D( lockedRect.left, lockedRect.top, lockedWidth, lockedHeight );
        [m_texture replaceRegion:region mipmapLevel:_level withBytes:buffer bytesPerRow:pitch];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLTexture> MetalRenderImage::getTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
}

