#include "DecoderRenderImageLoader.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/PrefetcherHelper.h"
#include "Kernel/DocumentableHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageLoader::DecoderRenderImageLoader()
        : m_codecFlags( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageLoader::~DecoderRenderImageLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DecoderRenderImageLoader::initialize( const ContentInterfacePtr & _content, uint32_t _codecFlags )
    {
        ImageDecoderInterfacePtr decoder;
        if( Helper::getPrefetchImageDecoder( _content, &decoder ) == false )
        {
            decoder = this->createImageDecoder_( _content );
        }
        else
        {
            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( "invalid rewind decoder '%s' codec '%s' (doc: %s)"
                    , Helper::getContentFullPath( _content ).c_str()
                    , _content->getCodecType().c_str()
                    , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
                );

                return false;
            }
        }

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "invalid create decoder '%s' codec '%s' (doc: %s)"
                , Helper::getContentFullPath( _content ).c_str()
                , _content->getCodecType().c_str()
                , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        if( dataInfo->width == 0 || dataInfo->height == 0 )
        {
            LOGGER_ERROR( "invalid image size %u:%u '%s' codec '%s' (doc: %s)"
                , dataInfo->width
                , dataInfo->height
                , Helper::getContentFullPath( _content ).c_str()
                , _content->getCodecType().c_str()
                , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
            );

            return false;
        }

        m_decoder = decoder;
        m_codecFlags = _codecFlags;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DecoderRenderImageLoader::finalize()
    {
        m_decoder = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DecoderRenderImageLoader::getImageDesc( RenderImageDesc * const _desc ) const
    {
        const ImageCodecDataInfo * dataInfo = m_decoder->getCodecDataInfo();

        _desc->mipmaps = dataInfo->mipmaps;
        _desc->width = dataInfo->width;
        _desc->height = dataInfo->height;
        _desc->format = dataInfo->format;
    }
    //////////////////////////////////////////////////////////////////////////    
    bool DecoderRenderImageLoader::load( const RenderImageInterfacePtr & _image ) const
    {
        uint32_t image_width = _image->getHWWidth();
        uint32_t image_height = _image->getHWHeight();

        const ImageCodecDataInfo * dataInfo = m_decoder->getCodecDataInfo();

        MENGINE_ASSERTION_FATAL( dataInfo->width <= image_width, "image width %u exceed texture width %u"
            , dataInfo->width
            , image_width
        );

        MENGINE_ASSERTION_FATAL( dataInfo->height <= image_height, "image height %u exceed texture height %u"
            , dataInfo->height
            , image_height
        );

        uint32_t mipmap = 0;

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = image_width;
        rect.bottom = image_height;

        RenderImageLockedInterfacePtr locked = _image->lock( 0, 0, rect, false );

        MENGINE_ASSERTION_MEMORY_PANIC( locked, "invalid lock texture %u:%u"
            , image_width
            , image_height
        );

        size_t pitch = 0;
        void * textureBuffer = locked->getLockedBuffer( &pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( textureBuffer, "invalid lock mipmap %u rect %u:%u-%u:%u"
            , 0
            , rect.left
            , rect.top
            , rect.right
            , rect.bottom
        );

        size_t mipmap_size = pitch * image_height;

        EPixelFormat hwPixelFormat = _image->getHWPixelFormat();

        if( _image->getUpscalePow2() == true )
        {
            StdString::memset( textureBuffer, 0, mipmap_size );
        }

        ImageDecoderData data;
        data.buffer = textureBuffer;
        data.size = mipmap_size;
        data.pitch = pitch;
        data.format = hwPixelFormat;
        data.mipmap = mipmap;
        data.flags = m_codecFlags;

        if( m_decoder->decode( &data ) == 0 )
        {
            LOGGER_ERROR( "invalid decode image %u:%u format %u"
                , dataInfo->width
                , dataInfo->height
                , dataInfo->format
            );

            _image->unlock( locked, 0, 0, false );

            return false;
        }

        if( _image->getUpscalePow2() == true )
        {
            uint32_t mipmap_width = image_width >> mipmap;
            uint32_t mipmap_height = image_height >> mipmap;

            uint32_t pixel_size = Helper::getPixelFormatChannels( hwPixelFormat );

            uint32_t width = dataInfo->width;
            uint32_t height = dataInfo->height;

            // copy pixels on the edge for better image quality
            if( width != mipmap_width )
            {
                uint8_t * image_data = static_cast<uint8_t *>(textureBuffer);

                for( uint32_t j = 0; j != height; ++j )
                {
                    size_t dst_offset = width * pixel_size;
                    size_t src_offset = (width - 1) * pixel_size;
                    size_t copy_size = pixel_size;

                    Helper::memoryCopy( image_data, dst_offset, image_data, src_offset, copy_size );

                    image_data += pitch;
                }
            }

            if( dataInfo->height != mipmap_height )
            {
                uint8_t * image_data = static_cast<uint8_t *>(textureBuffer);

                size_t dst_offset = height * pitch;
                size_t src_offset = (height - 1) * pitch;
                size_t copy_size = size_t( width ) * pixel_size;

                if( width != mipmap_width )
                {
                    copy_size += pixel_size;
                }

                Helper::memoryCopy( image_data, dst_offset, image_data, src_offset, copy_size );
            }
        }

        if( _image->unlock( locked, 0, 0, true ) == false )
        {
            LOGGER_ERROR( "invalid upload texture %u:%u format %u"
                , image_width
                , image_height
                , hwPixelFormat
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DecoderRenderImageLoader::getMemory( uint32_t _codecFlags, const DocumentInterfacePtr & _doc ) const
    {
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        const ImageCodecDataInfo * dataInfo = m_decoder->getCodecDataInfo();

        EPixelFormat format = dataInfo->format;
        uint32_t width = dataInfo->width;
        uint32_t height = dataInfo->height;

        uint32_t channels = Helper::getPixelFormatChannels( format );

        size_t pitch = size_t( width ) * channels;
        size_t dataSize = pitch * height;

        void * buffer = memory->newBuffer( dataSize );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid allocate image buffer %zu"
            , dataSize
        );

        ImageDecoderData data;
        data.buffer = buffer;
        data.size = dataSize;
        data.pitch = pitch;
        data.format = format;
        data.flags = _codecFlags;
        data.mipmap = 0;

        if( m_decoder->decode( &data ) == 0 )
        {
            LOGGER_ERROR( "invalid decode image %u:%u format %u"
                , dataInfo->width
                , dataInfo->height
                , dataInfo->format
            );

            return nullptr;
        }

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderInterfacePtr DecoderRenderImageLoader::createImageDecoder_( const ContentInterfacePtr & _content ) const
    {
        InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open stream '%s' codec '%s' (doc: %s)"
            , Helper::getContentFullPath( _content ).c_str()
            , _content->getCodecType().c_str()
            , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
        );

        MENGINE_ASSERTION_FATAL( stream->size() != 0, "empty stream '%s' codec '%s' (doc: %s)"
            , Helper::getContentFullPath( _content ).c_str()
            , _content->getCodecType().c_str()
            , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
        );

        const ConstString & codecType = _content->getCodecType();

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder '%s' codec '%s' (doc: %s)"
            , Helper::getContentFullPath( _content ).c_str()
            , _content->getCodecType().c_str()
            , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
        );

        if( decoder->prepareData( _content, stream ) == false )
        {
            LOGGER_ERROR( "invalid prepare data '%s' codec '%s' (doc: %s)"
                , Helper::getContentFullPath( _content ).c_str()
                , _content->getCodecType().c_str()
                , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
            );

            return nullptr;
        }

        return decoder;
    }
    //////////////////////////////////////////////////////////////////////////
}
