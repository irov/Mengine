#include "MetalRenderImage.h"

#include "Interface/RenderSystemInterface.h"

#include "Environment/Metal/MetalRenderSystemExtensionInterface.h"

#include "MetalRenderImageLockedFactoryStorage.h"
#include "MetalRenderEnum.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/TimestampHelper.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderImage::MetalRenderImage()
        : m_createTimestamp( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_width( 0 )
        , m_height( 0 )
        , m_hwMipmaps( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_texture( nil )
        , m_lockFirst( false )
        , m_pow2( false )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderImage::~MetalRenderImage()
    {
        MENGINE_ASSERTION_FATAL( m_texture == nil, "texture is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat )
    {
        MENGINE_ASSERTION_FATAL( _width != 0, "invalid create texture width == 0" );
        MENGINE_ASSERTION_FATAL( _height != 0, "invalid create texture height == 0" );

        m_width = _width;
        m_height = _height;
        m_hwMipmaps = _mipmaps;
        m_hwWidth = Helper::getTexturePow2( _width );
        m_hwHeight = Helper::getTexturePow2( _height );
        m_hwPixelFormat = _pixelFormat;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_pow2 = Helper::isTexturePow2( _width ) == true && Helper::isTexturePow2( _height ) == true;
        m_upscalePow2 = _width != m_hwWidth || _height != m_hwHeight;

        if( this->create() == false )
        {
            LOGGER_ERROR( "invalid create texture for size %u:%u PF %u"
                , _width
                , _height
                , _pixelFormat
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImage::finalize()
    {
        this->release();

        m_renderImageProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImage::getHWMipmaps() const
    {
        return m_hwMipmaps;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImage::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImage::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MetalRenderImage::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
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
    void MetalRenderImage::bind( uint32_t _stage )
    {
        MENGINE_UNUSED( _stage );

        // Binding is handled in the render command encoder
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImage::unbind( uint32_t _stage )
    {
        MENGINE_UNUSED( _stage );

        // Unbinding is handled in the render command encoder
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
    bool MetalRenderImage::create()
    {
        MENGINE_ASSERTION_FATAL( m_texture == nil, "texture already created" );

        MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        id<MTLDevice> device = extension->getMetalDevice();

        MTLPixelFormat pixelFormat = Helper::toMTLPixelFormat( m_hwPixelFormat );

        if( pixelFormat == MTLPixelFormatInvalid )
        {
            LOGGER_ERROR( "unsupported pixel format %d"
                , m_hwPixelFormat
            );

            return false;
        }

        MTLTextureDescriptor * descriptor = [[MTLTextureDescriptor alloc] init];
        descriptor.textureType = MTLTextureType2D;
        descriptor.pixelFormat = pixelFormat;
        descriptor.width = m_hwWidth;
        descriptor.height = m_hwHeight;
        descriptor.mipmapLevelCount = m_hwMipmaps > 0 ? m_hwMipmaps : 1;
        descriptor.usage = MTLTextureUsageShaderRead;
        descriptor.storageMode = MTLStorageModeShared;

        id<MTLTexture> texture = [device newTextureWithDescriptor:descriptor];

        if( texture == nil )
        {
            LOGGER_ERROR( "invalid create texture size %u:%u"
                , m_hwWidth
                , m_hwHeight
            );

            return false;
        }

        m_texture = texture;
        m_lockFirst = true;

        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE, m_hwWidth * m_hwHeight * Helper::getPixelFormatChannels( m_hwPixelFormat ) );

        m_createTimestamp = Helper::getSystemTimestamp();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImage::release()
    {
        if( m_texture == nil )
        {
            return;
        }

        STATISTIC_DEL_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE, m_hwWidth * m_hwHeight * Helper::getPixelFormatChannels( m_hwPixelFormat ) );

        m_texture = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLockedInterfacePtr MetalRenderImage::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _readOnly );

        uint32_t rect_width = _rect.getWidth();
        uint32_t rect_height = _rect.getHeight();

        uint32_t miplevel_width = rect_width >> _level;
        uint32_t miplevel_height = rect_height >> _level;

        size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwPixelFormat );

        MetalRenderImageLockedPtr imageLocked = MetalRenderImageLockedFactoryStorage::createObject( MENGINE_DOCUMENT_FACTORABLE );

        size_t pitch = size / miplevel_height;

        imageLocked->initialize( size, pitch, _rect );

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_LOCK_COUNT );
        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_LOCK_PIXEL, (_rect.bottom - _rect.top) * (_rect.right - _rect.left) );

        return imageLocked;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImage::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful )
    {
        if( _successful == false )
        {
            return true;
        }

        const Rect & lockedRect = _locked->getLockedRect();

        size_t pitch;
        void * buffer = _locked->getLockedBuffer( &pitch );

        uint32_t miplevel_hwwidth = m_hwWidth >> _level;
        uint32_t miplevel_hwheight = m_hwHeight >> _level;

        MENGINE_UNUSED( miplevel_hwheight );

        uint32_t upload_x = lockedRect.left >> _level;
        uint32_t upload_y = lockedRect.top >> _level;
        uint32_t upload_width = (lockedRect.right - lockedRect.left) >> _level;
        uint32_t upload_height = (lockedRect.bottom - lockedRect.top) >> _level;

        if( lockedRect.full( m_hwWidth, m_hwHeight ) == true )
        {
            upload_x = 0;
            upload_y = 0;
            upload_width = miplevel_hwwidth;
            upload_height = miplevel_hwheight;
        }

        MTLRegion region = MTLRegionMake2D( upload_x, upload_y, upload_width, upload_height );

        [m_texture replaceRegion:region
                     mipmapLevel:_level
                       withBytes:buffer
                     bytesPerRow:pitch];

        m_lockFirst = false;

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_UNLOCK_COUNT );
        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_UNLOCK_PIXEL, (lockedRect.bottom - lockedRect.top) * (lockedRect.right - lockedRect.left) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLTexture> MetalRenderImage::getMetalTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImage::onRenderReset()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImage::onRenderRestore()
    {
        if( this->create() == false )
        {
            return false;
        }

        if( m_renderImageProvider == nullptr )
        {
            return true;
        }

        RenderImageLoaderInterfacePtr loader = m_renderImageProvider->getLoader( MENGINE_DOCUMENT_FACTORABLE );

        if( loader->load( RenderImageInterfacePtr( this ) ) == false )
        {
            LOGGER_ERROR( "invalid decode image" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
