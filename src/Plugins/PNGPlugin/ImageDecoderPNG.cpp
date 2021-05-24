#include "ImageDecoderPNG.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#ifndef MENGINE_DECODER_PNG_BYTES_TO_CHECK
#define MENGINE_DECODER_PNG_BYTES_TO_CHECK 8
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void s_handlerError( png_structp png_ptr, const char * _error )
    {
        MENGINE_UNUSED( png_ptr );
        MENGINE_UNUSED( _error );

        LOGGER_ERROR( "png error: '%s'"
            , _error
        );
    }
    //////////////////////////////////////////////////////////////////////////
    static void PNGAPI s_handlerWarning( png_structp png_ptr, const char * _error )
    {
        MENGINE_UNUSED( png_ptr );
        MENGINE_UNUSED( _error );

        LOGGER_INFO( "png", "png warning: '%s'"
            , _error
        );
    }
    //////////////////////////////////////////////////////////////////////////
    static void PNGAPI s_readProc( png_structp _png_ptr, uint8_t * _data, png_size_t _size )
    {
        png_voidp io_ptr = png_get_io_ptr( _png_ptr );
        InputStreamInterface * stream = reinterpret_cast<InputStreamInterface *>(io_ptr);

        stream->read( _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    static png_voidp PNGAPI s_png_malloc_ptr( png_structp _png, png_size_t _size )
    {
        MENGINE_UNUSED( _png );

        void * p = Helper::allocateMemory( _size, "dpng" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void PNGAPI s_png_free_ptr( png_structp _png, png_voidp _ptr )
    {
        MENGINE_UNUSED( _png );

        Helper::deallocateMemory( _ptr, "dpng" );
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

        png_structp png_ptr = png_create_read_struct_2( png_ver, (png_voidp)this, &s_handlerError, &s_handlerWarning, (png_voidp)this, &s_png_malloc_ptr, &s_png_free_ptr );

        MENGINE_ASSERTION_MEMORY_PANIC( png_ptr );

        m_png_ptr = png_ptr;

        png_infop info_ptr = png_create_info_struct( m_png_ptr );

        if( info_ptr == nullptr )
        {
            LOGGER_ERROR( "Can't create info structure" );

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
        uint8_t png_check[MENGINE_DECODER_PNG_BYTES_TO_CHECK];
        m_stream->read( &png_check, MENGINE_DECODER_PNG_BYTES_TO_CHECK );

        if( png_sig_cmp( png_check, (png_size_t)0, MENGINE_DECODER_PNG_BYTES_TO_CHECK ) != 0 )
        {
            LOGGER_ERROR( "Bad or not PNG file" );

            return false;
        }

        //png_set_crc_action( m_png_ptr, PNG_CRC_WARN_USE, PNG_CRC_WARN_USE );

        png_set_read_fn( m_png_ptr, m_stream.get(), &s_readProc );

        png_set_sig_bytes( m_png_ptr, MENGINE_DECODER_PNG_BYTES_TO_CHECK );

#if defined(PNG_SETJMP_SUPPORTED) && !defined(MENGINE_SETJMP_UNSUPPORTED)
#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#endif 
        if( setjmp( png_jmpbuf( m_png_ptr ) ) )
        {
            LOGGER_ERROR( "jmp" );

            return false;
        }
#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#endif
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

#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
        if( bit_depth == 16 )
        {
            png_set_scale_16( m_png_ptr );
        }
#else
        if( bit_depth == 16 )
        {
            LOGGER_ERROR( "not support scale 16 to 8 bit" );

            return false;
        }
#endif

        switch( color_type )
        {
        case  PNG_COLOR_TYPE_PALETTE:
            {
                png_set_palette_to_rgb( m_png_ptr );
#ifndef MENGINE_RENDER_TEXTURE_RGBA
                png_set_bgr( m_png_ptr );
#endif
            }break;
        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_RGB_ALPHA:
            {
#ifndef MENGINE_RENDER_TEXTURE_RGBA
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

        png_byte channels = png_get_channels( m_png_ptr, m_info_ptr );

        m_dataInfo.width = width;
        m_dataInfo.height = height;
        m_dataInfo.depth = 1;
        m_dataInfo.mipmaps = 1;
        m_dataInfo.channels = channels;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderPNG::_decode( void * const _buffer, size_t _bufferSize )
    {
        switch( m_options.flags & 0x0000ffff )
        {
        case DF_NONE:
            {
                if( m_dataInfo.channels == m_options.channels )
                {
                    if( m_interlace_number_of_passes == 1 )
                    {
                        png_bytep carriage = (png_bytep)_buffer;

                        for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                        {
                            png_read_row( m_png_ptr, carriage, nullptr );

                            carriage += m_options.pitch;
                        }
                    }
                    else
                    {
                        png_bytep * image = (png_bytep *)png_malloc( m_png_ptr, m_dataInfo.height * sizeof( png_bytep * ) );

                        png_bytep carriage = (png_bytep)_buffer;

                        for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                        {
                            image[i] = carriage;

                            carriage += m_options.pitch;
                        }

                        png_read_image( m_png_ptr, image );

                        png_free( m_png_ptr, image );
                    }

                    if( m_options.flags & DF_PREMULTIPLY_ALPHA && m_dataInfo.channels == 4 )
                    {
                        png_bytep carriage = (png_bytep)_buffer;

                        for( uint32_t j = 0; j != m_dataInfo.height; ++j )
                        {
                            for( uint32_t i = 0; i != m_dataInfo.width; ++i )
                            {
                                png_byte alpha = carriage[i * 4 + 3];

                                carriage[i * 4 + 0] = (png_byte)((png_uint_32)carriage[i * 4 + 0] * alpha / 255);
                                carriage[i * 4 + 1] = (png_byte)((png_uint_32)carriage[i * 4 + 1] * alpha / 255);
                                carriage[i * 4 + 2] = (png_byte)((png_uint_32)carriage[i * 4 + 2] * alpha / 255);
                            }

                            carriage += m_options.pitch;
                        }
                    }
                }
                else if( m_dataInfo.channels == 1 && m_options.channels == 4 )
                {
                    png_bytep carriage = (png_bytep)_buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, carriage, nullptr );

                        carriage += m_options.pitch;
                    }

                    this->sweezleAlpha1( m_dataInfo.width, m_dataInfo.height, _buffer, m_options.pitch );
                }
                else if( m_dataInfo.channels == 3 && m_options.channels == 4 )
                {
                    png_bytep carriage = (png_bytep)_buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, carriage, nullptr );

                        carriage += m_options.pitch;
                    }

                    this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, _buffer, m_options.pitch );
                }
                else
                {
                    LOGGER_ERROR( "DEFAULT not support chanells %d - %d"
                        , m_dataInfo.channels
                        , m_options.channels
                    );

                    return 0;
                }
            }break;
        case DF_READ_ALPHA_ONLY:
            {
                if( m_dataInfo.channels == 1 && m_options.channels == 1 )
                {
                    png_bytep carriage = (png_bytep)_buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, carriage, nullptr );

                        carriage += m_options.pitch;
                    }
                }
                else if( m_dataInfo.channels == 4 && m_options.channels == 1 )
                {
                    MemoryInterfacePtr row_buffer = Helper::createMemoryCacheBuffer( m_row_bytes, MENGINE_DOCUMENT_FACTORABLE );

                    MENGINE_ASSERTION_MEMORY_PANIC( row_buffer, "invalid create cache buffer" );

                    png_byte * row_memory = row_buffer->getBuffer();

                    png_bytep carriage = (png_bytep)_buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, row_memory, nullptr );

                        png_size_t row_alpha = m_row_bytes / 4;
                        for( png_size_t j = 0; j != row_alpha; ++j )
                        {
                            carriage[j] = row_memory[j * 4 + 3];
                        }

                        carriage += m_options.pitch;
                    }
                }
                else
                {
                    LOGGER_ERROR( "DF_READ_ALPHA_ONLY not support chanells %d - %d"
                        , m_dataInfo.channels
                        , m_options.channels
                    );

                    return 0;
                }
            }break;
        case DF_WRITE_ALPHA_ONLY:
            {
                if( m_dataInfo.channels == 1 && m_options.channels == 4 )
                {
                    MemoryInterfacePtr row_buffer = Helper::createMemoryCacheBuffer( m_row_bytes, MENGINE_DOCUMENT_FACTORABLE );

                    MENGINE_ASSERTION_MEMORY_PANIC( row_buffer, "invalid create cache buffer" );

                    png_byte * row_memory = row_buffer->getBuffer();

                    png_bytep carriage = (png_bytep)_buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, row_memory, nullptr );

                        for( png_uint_32 j = 0; j != m_row_bytes; ++j )
                        {
                            carriage[j * 4 + 3] = row_memory[j];
                        }

                        carriage += m_options.pitch;
                    }
                }
                else if( m_dataInfo.channels == 4 && m_options.channels == 4 )
                {
                    MemoryInterfacePtr row_buffer = Helper::createMemoryCacheBuffer( m_row_bytes, MENGINE_DOCUMENT_FACTORABLE );

                    MENGINE_ASSERTION_MEMORY_PANIC( row_buffer, "invalid create cache buffer" );

                    png_byte * row_memory = row_buffer->getBuffer();

                    png_bytep carriage = (png_bytep)_buffer;

                    for( uint32_t i = 0; i != m_dataInfo.height; ++i )
                    {
                        png_read_row( m_png_ptr, row_memory, nullptr );

                        for( png_uint_32 j = 0; j != m_row_bytes; ++j )
                        {
                            carriage[j * 4 + 3] = row_memory[j * 4 + 3];
                        }

                        carriage += m_options.pitch;
                    }
                }
                else
                {
                    LOGGER_ERROR( "DF_WRITE_ALPHA_ONLY not support chanells %d - %d"
                        , m_dataInfo.channels
                        , m_options.channels
                    );

                    return 0;
                }
            }break;
        default:
            {
                LOGGER_ERROR( "unsupport options flag %d"
                    , m_options.flags
                );

                return 0;
            }break;
        }

        png_read_end( m_png_ptr, m_info_ptr );

        return _bufferSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderPNG::_rewind()
    {
        png_destroy_read_struct( &m_png_ptr, &m_info_ptr, nullptr );

        png_const_charp png_ver = PNG_LIBPNG_VER_STRING;

        //m_png_ptr = png_create_read_struct( png_ver, (png_voidp)this, &s_handlerError, &s_handlerWarning );
        m_png_ptr = png_create_read_struct_2( png_ver, (png_voidp)this, &s_handlerError, &s_handlerWarning, (png_voidp)this, &s_png_malloc_ptr, &s_png_free_ptr );

        MENGINE_ASSERTION_MEMORY_PANIC( m_png_ptr, "Can't create read structure" );

        m_info_ptr = png_create_info_struct( m_png_ptr );

        if( m_info_ptr == nullptr )
        {
            LOGGER_ERROR( "Can't create info structure" );

            png_destroy_write_struct( &m_png_ptr, nullptr );

            return false;
        }

        if( m_stream->seek( 0 ) == false )
        {
            return false;
        }

        if( this->_prepareData() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
