#   include "DecoderRenderImageProvider.h"

#	include "Interface/PrefetcherInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageProviderInterface::DecoderRenderImageProviderInterface()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageProviderInterface::~DecoderRenderImageProviderInterface()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    void DecoderRenderImageProviderInterface::initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName )
    {
        m_pakName = _pakName;
        m_fileName = _fileName;
        m_codecName = _codecName;
    }
    //////////////////////////////////////////////////////////////////////////    
    MemoryInterfacePtr DecoderRenderImageProviderInterface::getImageMemory( uint32_t & _mipmaps, uint32_t & _width, uint32_t & _height, uint32_t & _channels, uint32_t & _depth, PixelFormat & _format ) const
    {
        ImageDecoderInterfacePtr decoder;
        if( PREFETCHER_SERVICE( m_serviceProvider )
            ->getImageDecoder( m_pakName, m_fileName, decoder ) == false )
        {
            decoder = this->createImageDecoder_();

            if( decoder == nullptr )
            {
                LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::getImageMemory invalid create decoder '%s':'%s' codec '%s'"
                    , m_pakName.c_str()
                    , m_fileName.c_str()
                    , m_codecName.c_str()
                    );

                return nullptr;
            }
        }
        else
        {
            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::loadTexture invalid rewind decoder '%s':'%s' codec '%s'"
                    , m_pakName.c_str()
                    , m_fileName.c_str()
                    , m_codecName.c_str()
                    );

                return nullptr;
            }
        }

        const ImageCodecDataInfo* dataInfo = decoder->getCodecDataInfo();

        uint32_t image_mipmaps = dataInfo->mipmaps;
        uint32_t image_width = dataInfo->width;
        uint32_t image_height = dataInfo->height;
        uint32_t image_channels = dataInfo->channels;
        uint32_t image_depth = dataInfo->depth;
        PixelFormat image_format = dataInfo->format;

        _mipmaps = image_mipmaps;
        _width = image_width;
        _height = image_height;
        _channels = image_channels;
        _depth = image_depth;
        _format = image_format;


        uint32_t HWWidth = Helper::getTexturePOW2( image_width );
        uint32_t HWHeight = Helper::getTexturePOW2( image_height );

        uint32_t mipmap = 0;

        MemoryInterfacePtr memory = MEMORY_SERVICE( m_serviceProvider )
            ->createMemoryCacheBuffer();

        if( memory == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::getImageMemory invalid create memory for '%s:%s' codec '%s'"
                , m_pakName.c_str()
                , m_fileName.c_str()
                , m_codecName.c_str()
                );

            return nullptr;
        }

        uint32_t mipmap_width = HWWidth >> mipmap;
        uint32_t mipmap_height = HWHeight >> mipmap;

        size_t mipmap_size = Helper::getTextureMemorySize( mipmap_width, mipmap_height, image_channels, image_depth, image_format );

        void * memory_buffer = memory->newMemory( mipmap_size, __FILE__, __LINE__ );

        if( memory_buffer == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::getImageMemory invalid memory allocate '%u' for '%s:%s' codec '%s'"
                , (uint32_t)mipmap_size
                , m_pakName.c_str()
                , m_fileName.c_str()
                , m_codecName.c_str()
                );

            return nullptr;
        }

        uint32_t HWPitch = mipmap_size / mipmap_height;

        ImageCodecOptions options;
        options.pitch = HWPitch;
        options.channels = image_channels;
        options.mipmap = mipmap;
            
        decoder->setOptions( &options );

        if( decoder->decode( memory_buffer, mipmap_size ) == 0 )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::getImageMemory invalid decode for '%s:%s' codec '%s'"
                , m_pakName.c_str()
                , m_fileName.c_str()
                , m_codecName.c_str()
                );

            return nullptr;
        }

        // copy pixels on the edge for better image quality
        if( HWWidth != mipmap_width )
        {
            unsigned char * image_data = static_cast<unsigned char *>(memory_buffer);
            size_t pixel_size = HWPitch / HWWidth;

            for( uint32_t j = 0; j != mipmap_height; ++j )
            {
                stdex::memorycopy( image_data, mipmap_width * pixel_size, image_data + (mipmap_width - 1) * pixel_size, pixel_size );

                image_data += HWPitch;
            }
        }

        if( HWHeight > mipmap_height )
        {
            unsigned char * image_data = static_cast<unsigned char *>(memory_buffer);

            stdex::memorycopy( image_data, mipmap_height * HWPitch, image_data + (mipmap_height - 1) * HWPitch, HWPitch );
        }

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderInterfacePtr DecoderRenderImageProviderInterface::createImageDecoder_() const
    {
        InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
            ->openInputFile( m_pakName, m_fileName, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::createImageDecoder_ invalid open stream '%s:%s' codec '%s'"
                , m_pakName.c_str()
                , m_fileName.c_str()
                );

            return nullptr;
        }

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE( m_serviceProvider )
            ->createDecoderT<ImageDecoderInterfacePtr>( m_codecName );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::createImageDecoder_ invalid create decoder '%s:%s' codec '%s'"
                , m_pakName.c_str()
                , m_fileName.c_str()
                , m_codecName.c_str()
                );

            return nullptr;
        }

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("DecoderRenderImageProviderInterface::createImageDecoder_ invalid prepare data '%s:%s' codec '%s'"
                , m_pakName.c_str()
                , m_fileName.c_str()
                , m_codecName.c_str()
                );

            return nullptr;
        }

        return decoder;
    }
}	// namespace Menge
