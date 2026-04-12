#include "MetalRenderTargetTexture.h"

#include "MetalRenderEnum.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/PixelFormatHelper.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderTargetTexture::MetalRenderTargetTexture()
        : m_width( 0 )
        , m_height( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_texture( nil )
        , m_commandBufferRef( nullptr )
        , m_renderEncoderRef( nullptr )
        , m_drawableTextureRef( nullptr )
        , m_depthStencilTextureRef( nullptr )
        , m_pow2( false )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderTargetTexture::~MetalRenderTargetTexture()
    {
        MENGINE_ASSERTION_FATAL( m_texture == nil, "texture is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::initialize( uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat )
    {
        MENGINE_ASSERTION_FATAL( _width != 0, "invalid width == 0" );
        MENGINE_ASSERTION_FATAL( _height != 0, "invalid height == 0" );

        m_width = _width;
        m_height = _height;
        m_hwWidth = Helper::getTexturePow2( _width );
        m_hwHeight = Helper::getTexturePow2( _height );

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_hwPixelFormat = _pixelFormat;

        float u = float( m_width ) / float( m_hwWidth );
        float v = float( m_height ) / float( m_hwHeight );

        mt::uv4_from_mask( &m_uv, mt::vec4f( 0.f, v, u, 0.f ) );

        m_pow2 = Helper::isTexturePow2( _width ) == true && Helper::isTexturePow2( _height ) == true;
        m_upscalePow2 = _width != m_hwWidth || _height != m_hwHeight;

        if( this->create() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderTargetTexture::finalize()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::create()
    {
        MTLPixelFormat pixelFormat = Helper::toMTLPixelFormat( m_hwPixelFormat );

        if( pixelFormat == MTLPixelFormatInvalid )
        {
            pixelFormat = MTLPixelFormatBGRA8Unorm;
        }

        MTLTextureDescriptor * descriptor = [[MTLTextureDescriptor alloc] init];
        descriptor.textureType = MTLTextureType2D;
        descriptor.pixelFormat = pixelFormat;
        descriptor.width = m_hwWidth;
        descriptor.height = m_hwHeight;
        descriptor.mipmapLevelCount = 1;
        descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
        descriptor.storageMode = MTLStorageModePrivate;

        id<MTLTexture> texture = [m_device newTextureWithDescriptor:descriptor];

        if( texture == nil )
        {
            LOGGER_ERROR( "invalid create render target texture size %u:%u"
                , m_hwWidth
                , m_hwHeight
            );

            return false;
        }

        m_texture = texture;

        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE, m_hwWidth * m_hwHeight * Helper::getPixelFormatChannels( m_hwPixelFormat ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderTargetTexture::release()
    {
        if( m_texture == nil )
        {
            return;
        }

        STATISTIC_DEL_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE, m_hwWidth * m_hwHeight * Helper::getPixelFormatChannels( m_hwPixelFormat ) );

        m_texture = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderTargetTexture::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderTargetTexture::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderTargetTexture::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MetalRenderTargetTexture::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
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
        return m_upscalePow2;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::begin() const
    {
        if( m_renderEncoderRef == nullptr || m_commandBufferRef == nullptr || m_texture == nil )
        {
            return false;
        }

        if( *m_renderEncoderRef != nil )
        {
            [*m_renderEncoderRef endEncoding];
            *m_renderEncoderRef = nil;
        }

        MTLRenderPassDescriptor * desc = [MTLRenderPassDescriptor renderPassDescriptor];
        desc.colorAttachments[0].texture = m_texture;
        desc.colorAttachments[0].loadAction = MTLLoadActionClear;
        desc.colorAttachments[0].storeAction = MTLStoreActionStore;
        desc.colorAttachments[0].clearColor = MTLClearColorMake( 0.0, 0.0, 0.0, 0.0 );

        *m_renderEncoderRef = [*m_commandBufferRef renderCommandEncoderWithDescriptor:desc];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderTargetTexture::end() const
    {
        if( m_renderEncoderRef == nullptr || m_commandBufferRef == nullptr )
        {
            return;
        }

        if( *m_renderEncoderRef != nil )
        {
            [*m_renderEncoderRef endEncoding];
            *m_renderEncoderRef = nil;
        }

        if( m_drawableTextureRef == nullptr || *m_drawableTextureRef == nil )
        {
            return;
        }

        MTLRenderPassDescriptor * desc = [MTLRenderPassDescriptor renderPassDescriptor];
        desc.colorAttachments[0].texture = *m_drawableTextureRef;
        desc.colorAttachments[0].loadAction = MTLLoadActionLoad;
        desc.colorAttachments[0].storeAction = MTLStoreActionStore;

        if( m_depthStencilTextureRef != nullptr && *m_depthStencilTextureRef != nil )
        {
            desc.depthAttachment.texture = *m_depthStencilTextureRef;
            desc.depthAttachment.loadAction = MTLLoadActionLoad;
            desc.depthAttachment.storeAction = MTLStoreActionDontCare;

            desc.stencilAttachment.texture = *m_depthStencilTextureRef;
            desc.stencilAttachment.loadAction = MTLLoadActionLoad;
            desc.stencilAttachment.storeAction = MTLStoreActionDontCare;
        }

        *m_renderEncoderRef = [*m_commandBufferRef renderCommandEncoderWithDescriptor:desc];
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & MetalRenderTargetTexture::getUV() const
    {
        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLTexture> MetalRenderTargetTexture::getMetalTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderTargetTexture::setRenderEncoderContext( id<MTLCommandBuffer> * _commandBufferRef
        , id<MTLRenderCommandEncoder> * _renderEncoderRef
        , id<MTLTexture> * _drawableTextureRef
        , id<MTLTexture> * _depthStencilTextureRef )
    {
        m_commandBufferRef = _commandBufferRef;
        m_renderEncoderRef = _renderEncoderRef;
        m_drawableTextureRef = _drawableTextureRef;
        m_depthStencilTextureRef = _depthStencilTextureRef;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderTargetTexture::onRenderReset()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderTargetTexture::onRenderRestore()
    {
        if( this->create() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
