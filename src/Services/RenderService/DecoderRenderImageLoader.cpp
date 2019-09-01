#include "DecoderRenderImageLoader.h"

#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageLoader::DecoderRenderImageLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageLoader::~DecoderRenderImageLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DecoderRenderImageLoader::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecName )
    {
        ImageDecoderInterfacePtr decoder;
        if( PREFETCHER_SERVICE()
            ->getImageDecoder( _fileGroup, _filePath, decoder ) == false )
        {
            decoder = this->createImageDecoder_( _fileGroup, _filePath, _codecName );

            MENGINE_ASSERTION_MEMORY_PANIC( decoder, false, "invalid create decoder '%s':'%s' codec '%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , _codecName.c_str()
            );
        }
        else
        {
            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( "invalid rewind decoder '%s':'%s' codec '%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                    , _codecName.c_str()
                );

                return false;
            }
        }

        m_decoder = decoder;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageDesc DecoderRenderImageLoader::getImageDesc() const
    {
        const ImageCodecDataInfo * dataInfo = m_decoder->getCodecDataInfo();

        RenderImageDesc desc;
        desc.mipmaps = dataInfo->mipmaps;
        desc.width = dataInfo->width;
        desc.height = dataInfo->height;
        desc.channels = dataInfo->channels;
        desc.depth = dataInfo->depth;
        desc.format = dataInfo->format;

        return desc;
    }
    //////////////////////////////////////////////////////////////////////////    
    bool DecoderRenderImageLoader::load( const RenderImageInterfacePtr & _image ) const
    {
        //uint32_t image_mipmaps = dataInfo->mipmaps;
        uint32_t image_width = _image->getHWWidth();
        uint32_t image_height = _image->getHWHeight();
        uint32_t image_channels = _image->getHWChannels();
        uint32_t image_depth = _image->getHWDepth();
        EPixelFormat image_format = _image->getHWPixelFormat();

        uint32_t HWWidth = Helper::getTexturePOW2( image_width );
        uint32_t HWHeight = Helper::getTexturePOW2( image_height );

        uint32_t mipmap = 0;

        uint32_t mipmap_width = HWWidth >> mipmap;
        uint32_t mipmap_height = HWHeight >> mipmap;

        size_t mipmap_size = Helper::getTextureMemorySize( mipmap_width, mipmap_height, image_channels, image_depth, image_format );

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = image_width;
        rect.bottom = image_height;

        size_t pitch = 0;
        void * textureBuffer = _image->lock( &pitch, 0, rect, false );

        MENGINE_ASSERTION_MEMORY_PANIC( textureBuffer, false, "Invalid lock mipmap %d rect %d:%d-%d:%d"
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

        m_decoder->setOptions( &options );

        if( m_decoder->decode( textureBuffer, mipmap_size ) == 0 )
        {
            LOGGER_ERROR( "invalid decode for"
            );

            _image->unlock( 0, false );

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

        _image->unlock( 0, true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderInterfacePtr DecoderRenderImageLoader::createImageDecoder_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecName ) const
    {
        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( _fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr, "invalid open stream '%s:%s' codec '%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _codecName.c_str()
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( _codecName, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, nullptr, "invalid create decoder '%s:%s' codec '%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _codecName.c_str()
        );

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "invalid prepare data '%s:%s' codec '%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , _codecName.c_str()
            );

            return nullptr;
        }

        return decoder;
    }
}
