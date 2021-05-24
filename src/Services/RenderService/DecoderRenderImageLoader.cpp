#include "DecoderRenderImageLoader.h"

#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "stdex/memorycopy.h"

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

            MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder '%s':'%s' codec '%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , _codecType.c_str()
            );
        }
        else
        {
            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( "invalid rewind decoder '%s':'%s' codec '%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                    , _codecType.c_str()
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
        _desc->channels = dataInfo->channels;
        _desc->depth = dataInfo->depth;
        _desc->format = dataInfo->format;
    }
    //////////////////////////////////////////////////////////////////////////    
    bool DecoderRenderImageLoader::load( const RenderImageInterfacePtr & _image ) const
    {
        uint32_t image_width = _image->getHWWidth();
        uint32_t image_height = _image->getHWHeight();
        uint32_t image_channels = _image->getHWChannels();

        uint32_t HWWidth = Helper::getTexturePOW2( image_width );
        uint32_t HWHeight = Helper::getTexturePOW2( image_height );

        //ToDo
        uint32_t mipmap = 0;

        uint32_t mipmap_width = HWWidth >> mipmap;
        uint32_t mipmap_height = HWHeight >> mipmap;

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = image_width;
        rect.bottom = image_height;

        RenderImageLockedInterfacePtr locked = _image->lock( 0, rect, false );

        size_t pitch = 0;
        void * textureBuffer = locked->getBuffer( &pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( textureBuffer, "invalid lock mipmap %d rect %d:%d-%d:%d"
            , 0
            , rect.left
            , rect.top
            , rect.right
            , rect.bottom
        );

        ImageCodecOptions options;
        options.pitch = pitch;
        options.channels = image_channels;
        options.mipmap = mipmap;
        options.flags = m_codecFlags;

        m_decoder->setOptions( &options );

        size_t mipmap_size = pitch * mipmap_height;

        if( m_decoder->decode( textureBuffer, mipmap_size ) == 0 )
        {
            LOGGER_ERROR( "invalid decode for" );

            _image->unlock( locked, 0, false );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = m_decoder->getCodecDataInfo();

        // copy pixels on the edge for better image quality
        if( dataInfo->width != mipmap_width )
        {
            uint8_t * image_data = static_cast<uint8_t *>(textureBuffer);
            size_t pixel_size = pitch / HWWidth;

            for( uint32_t j = 0; j != dataInfo->height; ++j )
            {
                stdex::memorycopy( image_data, dataInfo->width * pixel_size, image_data + (dataInfo->width - 1) * pixel_size, pixel_size );

                image_data += pitch;
            }
        }

        if( dataInfo->height != mipmap_height )
        {
            uint8_t * image_data = static_cast<uint8_t *>(textureBuffer);
            size_t pixel_size = pitch / HWWidth;

            stdex::memorycopy( image_data, dataInfo->height * pitch, image_data + (dataInfo->height - 1) * pitch, dataInfo->width * pixel_size );
        }

        _image->unlock( locked, 0, true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DecoderRenderImageLoader::getMemory( uint32_t _codecFlags, const DocumentPtr & _doc ) const
    {
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        const ImageCodecDataInfo * dataInfo = m_decoder->getCodecDataInfo();

        size_t pitch = dataInfo->width * dataInfo->channels;
        size_t dataSize = pitch * dataInfo->height;

        void * buffer = memory->newBuffer( dataSize );

        ImageCodecOptions options;
        options.pitch = pitch;
        options.channels = dataInfo->channels;
        options.mipmap = 0;
        options.flags = _codecFlags;

        m_decoder->setOptions( &options );

        if( m_decoder->decode( buffer, dataSize ) == 0 )
        {
            LOGGER_ERROR( "invalid decode for" );

            return nullptr;
        }

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderInterfacePtr DecoderRenderImageLoader::createImageDecoder_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType ) const
    {
        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open stream '%s:%s' codec '%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _codecType.c_str()
        );

        MENGINE_ASSERTION_FATAL( stream->size() != 0, "empty stream '%s:%s' codec '%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _codecType.c_str()
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( _codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder '%s:%s' codec '%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _codecType.c_str()
        );

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "invalid prepare data '%s:%s' codec '%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , _codecType.c_str()
            );

            return nullptr;
        }

        return decoder;
    }
    //////////////////////////////////////////////////////////////////////////
}
