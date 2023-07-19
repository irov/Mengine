#include "DecoderRenderImageLoader.h"

#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/DocumentableHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/MemoryCopy.h"

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
    bool DecoderRenderImageLoader::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, uint32_t _codecFlags )
    {
        ImageDecoderInterfacePtr decoder;
        if( PREFETCHER_SERVICE()
            ->getImageDecoder( _fileGroup, _filePath, &decoder ) == false )
        {
            decoder = this->createImageDecoder_( _fileGroup, _filePath, _codecType );

            MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder '%s' codec '%s' (doc: %s)"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , _codecType.c_str()
                , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
            );
        }
        else
        {
            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( "invalid rewind decoder '%s' codec '%s' (doc: %s)"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , _codecType.c_str()
                    , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
                );

                return false;
            }
        }

        m_decoder = decoder;
        m_codecFlags = _codecFlags;

        return true;
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

        uint32_t mipmap = 0;

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = image_width;
        rect.bottom = image_height;

        RenderImageLockedInterfacePtr locked = _image->lock( 0, rect, false );

        size_t pitch = 0;
        void * textureBuffer = locked->getBuffer( &pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( textureBuffer, "invalid lock mipmap %u rect %u:%u-%u:%u"
            , 0
            , rect.left
            , rect.top
            , rect.right
            , rect.bottom
        );

        size_t mipmap_size = pitch * image_height;

        EPixelFormat hwPixelFormat = _image->getHWPixelFormat();

        ImageDecoderData data;
        data.buffer = textureBuffer;
        data.size = mipmap_size;
        data.pitch = pitch;
        data.format = hwPixelFormat;
        data.mipmap = mipmap;
        data.flags = m_codecFlags;

        if( m_decoder->decode( &data ) == 0 )
        {
            _image->unlock( locked, 0, false );

            return false;
        }

        if( _image->getUpscalePow2() == true )
        {
            uint32_t mipmap_width = image_width >> mipmap;
            uint32_t mipmap_height = image_height >> mipmap;

            uint32_t pixel_size = Helper::getPixelFormatChannels( hwPixelFormat );

            const ImageCodecDataInfo * dataInfo = m_decoder->getCodecDataInfo();

            // copy pixels on the edge for better image quality
            if( dataInfo->width != mipmap_width )
            {
                uint8_t * image_data = static_cast<uint8_t *>(textureBuffer);

                for( uint32_t j = 0; j != dataInfo->height; ++j )
                {
                    Helper::memoryCopy( image_data, dataInfo->width * pixel_size, image_data + (dataInfo->width - 1) * pixel_size, pixel_size );

                    image_data += pitch;
                }
            }

            if( dataInfo->height != mipmap_height )
            {
                uint8_t * image_data = static_cast<uint8_t *>(textureBuffer);

                Helper::memoryCopy( image_data, dataInfo->height * pitch, image_data + (dataInfo->height - 1) * pitch, dataInfo->width * pixel_size );
            }
        }

        _image->unlock( locked, 0, true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DecoderRenderImageLoader::getMemory( uint32_t _codecFlags, const DocumentInterfacePtr & _doc ) const
    {
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        const ImageCodecDataInfo * dataInfo = m_decoder->getCodecDataInfo();

        uint32_t channels = Helper::getPixelFormatChannels( dataInfo->format );

        size_t pitch = dataInfo->width * channels;
        size_t dataSize = pitch * dataInfo->height;

        void * buffer = memory->newBuffer( dataSize );

        ImageDecoderData data;
        data.buffer = buffer;
        data.size = dataSize;
        data.pitch = pitch;
        data.format = dataInfo->format;
        data.flags = _codecFlags;
        data.mipmap = 0;

        if( m_decoder->decode( &data ) == 0 )
        {
            return nullptr;
        }

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderInterfacePtr DecoderRenderImageLoader::createImageDecoder_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType ) const
    {
        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open stream '%s' codec '%s' (doc: %s)"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            , _codecType.c_str()
            , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
        );

        MENGINE_ASSERTION_FATAL( stream->size() != 0, "empty stream '%s' codec '%s' (doc: %s)"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            , _codecType.c_str()
            , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( _codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder '%s' codec '%s' (doc: %s)"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            , _codecType.c_str()
            , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
        );

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "invalid prepare data '%s' codec '%s' (doc: %s)"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , _codecType.c_str()
                , MENGINE_DOCUMENTABLE_STR( this, "DecoderRenderImageLoader" )
            );

            return nullptr;
        }

        return decoder;
    }
    //////////////////////////////////////////////////////////////////////////
}
