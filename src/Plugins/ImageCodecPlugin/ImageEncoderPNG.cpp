#include "ImageEncoderPNG.h"

#include "Kernel/Logger.h"

#include "stdex/allocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void s_handlerError( png_structp _png_ptr, const char * _error )
    {
        (void)_png_ptr;

        LOGGER_ERROR( "%s"
            , _error
        );
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_handlerWarning( png_structp _png_ptr, const char * _error )
    {
        (void)_png_ptr;

        LOGGER_WARNING( "%s"
            , _error
        );
    }
    //////////////////////////////////////////////////////////////////////////
    static void	s_writeProc( png_structp _png_ptr, uint8_t *data, png_size_t size )
    {
        png_voidp io_ptr = png_get_io_ptr( _png_ptr );
        OutputStreamInterface* stream = static_cast<OutputStreamInterface*>(io_ptr);

        stream->write( (char*)data, size );
    }
    //////////////////////////////////////////////////////////////////////////
    static void	s_flushProc( png_structp _png_ptr )
    {
        png_voidp io_ptr = png_get_io_ptr( _png_ptr );
        OutputStreamInterface* stream = static_cast<OutputStreamInterface*>(io_ptr);

        stream->flush();
    }
    //////////////////////////////////////////////////////////////////////////
    static png_voidp PNGAPI s_png_malloc_ptr( png_structp _png, png_size_t _size )
    {
        (void)_png;

        return stdex_malloc( _size, "png encoder" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void PNGAPI s_png_free_ptr( png_structp _png, png_voidp _ptr )
    {
        (void)_png;

        stdex_free( _ptr, "png encoder" );
    }
    //////////////////////////////////////////////////////////////////////////
    ImageEncoderPNG::ImageEncoderPNG()
        : m_png_ptr( nullptr )
        , m_info_ptr( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageEncoderPNG::~ImageEncoderPNG()
    {
        if( m_png_ptr != nullptr )
        {
            png_destroy_write_struct( &m_png_ptr, nullptr );
            m_png_ptr = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageEncoderPNG::_initialize()
    {
        png_const_charp png_ver = PNG_LIBPNG_VER_STRING;

        m_png_ptr = png_create_write_struct_2( png_ver, (png_voidp)this, &s_handlerError, &s_handlerWarning, (png_voidp)this, &s_png_malloc_ptr, &s_png_free_ptr );

        if( m_png_ptr == nullptr )
        {
            LOGGER_ERROR( "PNG encoder error: Can't create write structure"
            );

            return false;
        }

        // init the IO
        png_set_write_fn( m_png_ptr, m_stream.get(), s_writeProc, s_flushProc );

        // allocate/initialize the image information data.
        m_info_ptr = png_create_info_struct( m_png_ptr );

        if( m_info_ptr == nullptr )
        {
            LOGGER_ERROR( "PNG encoder error: Can't create info structure"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageEncoderPNG::_finalize()
    {
        png_destroy_write_struct( &m_png_ptr, &m_info_ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageEncoderPNG::encode( const void * _buffer, size_t _size, const CodecDataInfo* _bufferDataInfo )
    {
        (void)_size;

        const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>(_bufferDataInfo);

        int32_t color_type;
        if( dataInfo->channels == 1 )
        {
            color_type = PNG_COLOR_TYPE_GRAY;
        }
        else if( dataInfo->channels == 3 )
        {
            color_type = PNG_COLOR_TYPE_RGB;
        }
        else if( dataInfo->channels == 4 )
        {
            color_type = PNG_COLOR_TYPE_RGB_ALPHA;
        }
        else
        {
            LOGGER_ERROR( "PNG codec error: unsupported image format channels %d"
                , dataInfo->channels
            );

            return 0;
        }

        png_uint_32 width = (png_uint_32)dataInfo->width;
        png_uint_32 height = (png_uint_32)dataInfo->height;
        int32_t pixel_depth = 8;

        png_set_IHDR( m_png_ptr, m_info_ptr, width, height, pixel_depth, color_type, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

        png_set_bgr( m_png_ptr );

        // Write the file header information.
        png_write_info( m_png_ptr, m_info_ptr );

        size_t pitch = m_options.pitch;

        png_bytep png_buffer = (png_bytep)_buffer;

        for( png_uint_32 k = 0; k < height; ++k )
        {
            png_write_row( m_png_ptr, png_buffer );
            png_buffer += pitch;
        }
        //}
        // It is REQUIRED to call this to finish writing the rest of the file
        // Bug with png_flush
        png_write_end( m_png_ptr, m_info_ptr );

        size_t writeBytes = pitch * height;

        return writeBytes;
    }
    //////////////////////////////////////////////////////////////////////////
}
