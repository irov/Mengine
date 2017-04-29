#   include "DecoderRenderImageLoader.h"

#	include "Interface/PrefetcherInterface.h"

#   include "Logger/Logger.h"

namespace Menge
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
    bool DecoderRenderImageLoader::initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName )
    {
        ImageDecoderInterfacePtr decoder;
        if( PREFETCHER_SERVICE( m_serviceProvider )
            ->getImageDecoder( _pakName, _fileName, decoder ) == false )
        {
            decoder = this->createImageDecoder_( _pakName, _fileName, _codecName );

            if( decoder == nullptr )
            {
                LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::getImageMemory invalid create decoder '%s':'%s' codec '%s'"
                    , _pakName.c_str()
                    , _fileName.c_str()
                    , _codecName.c_str()
                    );

                return false;
            }
        }
        else
        {
            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::loadTexture invalid rewind decoder '%s':'%s' codec '%s'"
                    , _pakName.c_str()
                    , _fileName.c_str()
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
        const ImageCodecDataInfo* dataInfo = m_decoder->getCodecDataInfo();

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
    bool DecoderRenderImageLoader::load( void * _buffer, uint32_t _pitch ) const
    {
        const ImageCodecDataInfo* dataInfo = m_decoder->getCodecDataInfo();

        //uint32_t image_mipmaps = dataInfo->mipmaps;
        uint32_t image_width = dataInfo->width;
        uint32_t image_height = dataInfo->height;
        uint32_t image_channels = dataInfo->channels;
        uint32_t image_depth = dataInfo->depth;
        PixelFormat image_format = dataInfo->format;

        uint32_t HWWidth = Helper::getTexturePOW2( image_width );
        uint32_t HWHeight = Helper::getTexturePOW2( image_height );

        uint32_t mipmap = 0;

        uint32_t mipmap_width = HWWidth >> mipmap;
        uint32_t mipmap_height = HWHeight >> mipmap;

        size_t mipmap_size = Helper::getTextureMemorySize( mipmap_width, mipmap_height, image_channels, image_depth, image_format );

        ImageCodecOptions options;
        options.pitch = _pitch;
        options.channels = image_channels;
        options.mipmap = mipmap;
            
        m_decoder->setOptions( &options );

        if( m_decoder->decode( _buffer, mipmap_size ) == 0 )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::getImageMemory invalid decode for"
                );

            return false;
        }

        // copy pixels on the edge for better image quality
        if( image_width != mipmap_width )
        {
            unsigned char * image_data = static_cast<unsigned char *>(_buffer);
            size_t pixel_size = _pitch / HWWidth;

            for( uint32_t j = 0; j != image_height; ++j )
            {
                stdex::memorycopy( image_data, image_width * pixel_size, image_data + (image_width - 1) * pixel_size, pixel_size );

                image_data += _pitch;
            }
        }

        if( image_height != mipmap_height )
        {
            unsigned char * image_data = static_cast<unsigned char *>(_buffer);

            stdex::memorycopy( image_data, image_height * _pitch, image_data + (image_height - 1) * _pitch, _pitch );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderInterfacePtr DecoderRenderImageLoader::createImageDecoder_( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName ) const
    {
        InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
            ->openInputFile( _pakName, _fileName, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::createImageDecoder_ invalid open stream '%s:%s' codec '%s'"
                , _pakName.c_str()
                , _fileName.c_str()
                );

            return nullptr;
        }

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE( m_serviceProvider )
            ->createDecoderT<ImageDecoderInterfacePtr>( _codecName );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::createImageDecoder_ invalid create decoder '%s:%s' codec '%s'"
                , _pakName.c_str()
                , _fileName.c_str()
                , _codecName.c_str()
                );

            return nullptr;
        }

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::createImageDecoder_ invalid prepare data '%s:%s' codec '%s'"
                , _pakName.c_str()
                , _fileName.c_str()
                , _codecName.c_str()
                );

            return nullptr;
        }

        return decoder;
    }
}	// namespace Menge
