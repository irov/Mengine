#include "ImageDecoderPNG.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/StdSetJMP.h"

#ifndef MENGINE_DECODER_PNG_BYTES_TO_CHECK
#define MENGINE_DECODER_PNG_BYTES_TO_CHECK 8
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void PNGAPI png_handler_error_ptr( png_structp png_ptr, const char * _error )
        {
            MENGINE_UNUSED( _error );

            png_voidp error_ptr = png_get_error_ptr( png_ptr );

            ImageDecoderPNG * decoder = reinterpret_cast<ImageDecoderPNG *>(error_ptr);

            LOGGER_ASSERTION( "png [%s] error: '%s'"
                , Helper::getDebugFullPath( decoder->getStream() )
                , _error
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void PNGAPI png_handler_warning_ptr( png_structp png_ptr, const char * _error )
        {
            MENGINE_UNUSED( _error );

            png_voidp error_ptr = png_get_error_ptr( png_ptr );

            ImageDecoderPNG * decoder = reinterpret_cast<ImageDecoderPNG *>(error_ptr);

            LOGGER_WARNING( "png [%s] warning: '%s'"
                , Helper::getDebugFullPath( decoder->getStream() )
                , _error
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void PNGAPI png_read_proc_ptr( png_structp png_ptr, uint8_t * _data, png_size_t _size )
        {
            png_voidp io_ptr = png_get_io_ptr( png_ptr );

            InputStreamInterface * stream = reinterpret_cast<InputStreamInterface *>(io_ptr);

            stream->read( _data, _size );
        }
        //////////////////////////////////////////////////////////////////////////
        static png_voidp PNGAPI png_malloc_ptr( png_structp png_ptr, png_size_t _size )
        {
            MENGINE_UNUSED( png_ptr );

            void * p = Helper::allocateMemory( _size, "dpng" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void PNGAPI png_free_ptr( png_structp png_ptr, png_voidp _ptr )
        {
            MENGINE_UNUSED( png_ptr );

            Helper::deallocateMemory( _ptr, "dpng" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderPNG::ImageDecoderPNG()
        : m_png_ptr( nullptr )
        , m_info_ptr( nullptr )
        , m_row_bytes( 0 )
        , m_interlace_number_of_passes( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderPNG::~ImageDecoderPNG()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderPNG::_initialize()
    {
        png_const_charp png_ver = PNG_LIBPNG_VER_STRING;

        png_structp png_ptr = png_create_read_struct_2( png_ver, (png_voidp)this, &Detail::png_handler_error_ptr, &Detail::png_handler_warning_ptr, (png_voidp)this, &Detail::png_malloc_ptr, &Detail::png_free_ptr );

        MENGINE_ASSERTION_MEMORY_PANIC( png_ptr );

        m_png_ptr = png_ptr;

        png_infop info_ptr = png_create_info_struct( m_png_ptr );

        if( info_ptr == nullptr )
        {
            LOGGER_ERROR( "can't create png info structure" );

            png_destroy_write_struct( &m_png_ptr, nullptr );

            return false;
        }

        m_info_ptr = info_ptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderPNG::_finalize()
    {
        png_destroy_read_struct( &m_png_ptr, &m_info_ptr, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderPNG::_prepareData()
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        uint8_t png_check[MENGINE_DECODER_PNG_BYTES_TO_CHECK];
        if( stream->read( &png_check, MENGINE_DECODER_PNG_BYTES_TO_CHECK ) != MENGINE_DECODER_PNG_BYTES_TO_CHECK )
        {
            LOGGER_ERROR( "bad or not PNG file '%s' (size)"
                , Helper::getDebugFullPath( this->getStream() )
            );

            return false;
        }

        if( png_sig_cmp( png_check, (png_size_t)0, MENGINE_DECODER_PNG_BYTES_TO_CHECK ) != 0 )
        {
            LOGGER_ERROR( "bad or not PNG file '%s' (sig)"
                , Helper::getDebugFullPath( this->getStream() )
            );

            return false;
        }

        //png_set_crc_action( m_png_ptr, PNG_CRC_WARN_USE, PNG_CRC_WARN_USE );

        png_set_read_fn( m_png_ptr, stream.get(), &Detail::png_read_proc_ptr );

        png_set_sig_bytes( m_png_ptr, MENGINE_DECODER_PNG_BYTES_TO_CHECK );

#if defined(PNG_SETJMP_SUPPORTED)
        if( MENGINE_JMP_SET( png_jmpbuf( m_png_ptr ) ) != 0 )
        {
            return false;
        }
#endif

        png_read_info( m_png_ptr, m_info_ptr );

        png_uint_32 width = 0;
        png_uint_32 height = 0;
        int32_t color_type = 0;
        int32_t bit_depth = 0;
        int32_t interlace_method = 0;

        png_get_IHDR( m_png_ptr, m_info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, nullptr, nullptr );

        if( interlace_method != PNG_INTERLACE_NONE )
        {
            m_interlace_number_of_passes = png_set_interlace_handling( m_png_ptr );
        }

#if defined(PNG_READ_SCALE_16_TO_8_SUPPORTED)
        if( bit_depth == 16 )
        {
            png_set_scale_16( m_png_ptr );
        }
#else
        if( bit_depth == 16 )
        {
            LOGGER_ERROR( "not support png file '%s' scale 16 to 8 bit"
                , Helper::getInputStreamDebugFilePath( decoder->getStream() ).c_str() 
            );

            return false;
        }
#endif

        switch( color_type )
        {
        case PNG_COLOR_TYPE_PALETTE:
            {
                png_set_palette_to_rgb( m_png_ptr );
#if !defined(MENGINE_RENDER_TEXTURE_RGBA)
                png_set_bgr( m_png_ptr );
#endif
            }break;
        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_RGB_ALPHA:
            {
#if !defined(MENGINE_RENDER_TEXTURE_RGBA)
                png_set_bgr( m_png_ptr );
#endif
            }break;
        case PNG_COLOR_TYPE_GRAY:
            {
            }break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            {
                png_set_strip_alpha( m_png_ptr );
            }break;
        default:
            break;
        };

        png_read_update_info( m_png_ptr, m_info_ptr );

        png_get_IHDR( m_png_ptr, m_info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, 0, 0 );

        m_row_bytes = png_get_rowbytes( m_png_ptr, m_info_ptr );

        m_dataInfo.width = width;
        m_dataInfo.height = height;
        m_dataInfo.mipmaps = 1;

        png_byte channels = png_get_channels( m_png_ptr, m_info_ptr );

        switch( channels )
        {
        case 3:
            {
                m_dataInfo.format = PF_R8G8B8;
            }break;
        case 4:
            {
                m_dataInfo.format = PF_A8R8G8B8;
            }break;
        default:
            {
                LOGGER_ERROR( "png file '%s' unsupport channels %u"
                    , Helper::getDebugFullPath( this->getStream() )
                    , channels
                );

                return false;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderPNG::_decode( const DecoderData * _decoderData )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _decoderData );
        MENGINE_ASSERTION_TYPE( _decoderData, const ImageDecoderData * );

        MENGINE_PROFILER_CATEGORY();

        const ImageDecoderData * decoderData = static_cast<const ImageDecoderData *>(_decoderData);

        void * buffer = decoderData->buffer;
        size_t size = decoderData->size;
        size_t pitch = decoderData->pitch;
        uint32_t flags = decoderData->flags;

        uint32_t dataChannels = Helper::getPixelFormatChannels( m_dataInfo.format );
        uint32_t optionChannels = Helper::getPixelFormatChannels( decoderData->format );

        switch( flags & 0x0000ffff )
        {
        case DF_IMAGE_NONE:
            {
                if( dataChannels == optionChannels )
                {
                    if( m_interlace_number_of_passes == 1 )
                    {
                        png_bytep carriage = (png_bytep)buffer;

                        for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                        {
                            png_read_row( m_png_ptr, carriage, nullptr );

                            carriage += pitch;
                        }
                    }
                    else
                    {
                        png_bytep * image = (png_bytep *)png_malloc( m_png_ptr, m_dataInfo.height * sizeof( png_bytep * ) );

                        png_bytep carriage = (png_bytep)buffer;

                        for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                        {
                            image[i] = carriage;

                            carriage += pitch;
                        }

                        png_read_image( m_png_ptr, image );

                        png_free( m_png_ptr, image );
                    }

                    if( flags & DF_IMAGE_PREMULTIPLY_ALPHA && dataChannels == 4 )
                    {
                        png_bytep carriage = (png_bytep)buffer;

                        for( uint32_t j = 0; j != m_dataInfo.height; ++j )
                        {
                            for( uint32_t i = 0; i != m_dataInfo.width; ++i )
                            {
                                png_byte alpha = carriage[i * 4 + 3];

                                carriage[i * 4 + 0] = (png_byte)((png_uint_32)carriage[i * 4 + 0] * alpha / 255);
                                carriage[i * 4 + 1] = (png_byte)((png_uint_32)carriage[i * 4 + 1] * alpha / 255);
                                carriage[i * 4 + 2] = (png_byte)((png_uint_32)carriage[i * 4 + 2] * alpha / 255);
                            }

                            carriage += pitch;
                        }
                    }
                }
                else if( dataChannels == 1 && optionChannels == 4 )
                {
                    png_bytep carriage = (png_bytep)buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, carriage, nullptr );

                        carriage += pitch;
                    }

                    this->sweezleAlpha1( m_dataInfo.width, m_dataInfo.height, buffer, pitch );
                }
                else if( dataChannels == 3 && optionChannels == 4 )
                {
                    png_bytep carriage = (png_bytep)buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, carriage, nullptr );

                        carriage += pitch;
                    }

                    this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, buffer, pitch );
                }
                else
                {
                    LOGGER_ERROR( "png file '%s' DEFAULT not support chanells %u - %u"
                        , Helper::getDebugFullPath( this->getStream() )
                        , dataChannels
                        , optionChannels
                    );

                    return 0;
                }
            }break;
        case DF_IMAGE_READ_ALPHA_ONLY:
            {
                if( dataChannels == 1 && optionChannels == 1 )
                {
                    png_bytep carriage = (png_bytep)buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, carriage, nullptr );

                        carriage += pitch;
                    }
                }
                else if( dataChannels == 4 && optionChannels == 1 )
                {
                    MemoryInterfacePtr row_buffer = Helper::createMemoryCacheBuffer( m_row_bytes, MENGINE_DOCUMENT_FACTORABLE );

                    MENGINE_ASSERTION_MEMORY_PANIC( row_buffer, "invalid create cache buffer" );

                    png_byte * row_memory = row_buffer->getBuffer();

                    png_bytep carriage = static_cast<png_bytep>(buffer);

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, row_memory, nullptr );

                        png_size_t row_alpha = m_row_bytes / 4;
                        for( png_size_t j = 0; j != row_alpha; ++j )
                        {
                            carriage[j] = row_memory[j * 4 + 3];
                        }

                        carriage += pitch;
                    }
                }
                else
                {
                    LOGGER_ERROR( "png file '%s' DF_READ_ALPHA_ONLY not support chanells %u - %u"
                        , Helper::getDebugFullPath( this->getStream() )
                        , dataChannels
                        , optionChannels
                    );

                    return 0;
                }
            }break;
        case DF_IMAGE_WRITE_ALPHA_ONLY:
            {
                if( dataChannels == 1 && optionChannels == 4 )
                {
                    MemoryInterfacePtr row_buffer = Helper::createMemoryCacheBuffer( m_row_bytes, MENGINE_DOCUMENT_FACTORABLE );

                    MENGINE_ASSERTION_MEMORY_PANIC( row_buffer, "invalid create cache buffer" );

                    png_byte * row_memory = row_buffer->getBuffer();

                    png_bytep carriage = static_cast<png_bytep>(buffer);

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, row_memory, nullptr );

                        for( png_uint_32 j = 0; j != m_row_bytes; ++j )
                        {
                            carriage[j * 4 + 3] = row_memory[j];
                        }

                        carriage += pitch;
                    }
                }
                else if( dataChannels == 4 && optionChannels == 4 )
                {
                    MemoryInterfacePtr row_buffer = Helper::createMemoryCacheBuffer( m_row_bytes, MENGINE_DOCUMENT_FACTORABLE );

                    MENGINE_ASSERTION_MEMORY_PANIC( row_buffer, "invalid create cache buffer" );

                    png_byte * row_memory = row_buffer->getBuffer();

                    png_bytep carriage = static_cast<png_bytep>(buffer);

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, row_memory, nullptr );

                        for( png_uint_32 j = 0; j != m_row_bytes; ++j )
                        {
                            carriage[j * 4 + 3] = row_memory[j * 4 + 3];
                        }

                        carriage += pitch;
                    }
                }
                else
                {
                    LOGGER_ERROR( "png file '%s' DF_WRITE_ALPHA_ONLY not support chanells %u - %u"
                        , Helper::getDebugFullPath( this->getStream() )
                        , dataChannels
                        , optionChannels
                    );

                    return 0;
                }
            }break;
        default:
            {
                LOGGER_ERROR( "png file '%s' unsupport options flag %u"
                    , Helper::getDebugFullPath( this->getStream() )
                    , flags
                );

                return 0;
            }break;
        }

        png_read_end( m_png_ptr, m_info_ptr );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderPNG::_rewind()
    {
        png_destroy_read_struct( &m_png_ptr, &m_info_ptr, nullptr );

        png_const_charp png_ver = PNG_LIBPNG_VER_STRING;

        png_structp png_ptr = png_create_read_struct_2( png_ver, (png_voidp)this, &Detail::png_handler_error_ptr, &Detail::png_handler_warning_ptr, (png_voidp)this, &Detail::png_malloc_ptr, &Detail::png_free_ptr );

        MENGINE_ASSERTION_MEMORY_PANIC( png_ptr, "Can't create read structure" );

        m_png_ptr = png_ptr;

        png_infop info_ptr = png_create_info_struct( m_png_ptr );

        if( info_ptr == nullptr )
        {
            png_destroy_write_struct( &m_png_ptr, nullptr );

            return false;
        }

        m_info_ptr = info_ptr;

        const InputStreamInterfacePtr & stream = this->getStream();

        stream->rewind();

        if( this->_prepareData() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
