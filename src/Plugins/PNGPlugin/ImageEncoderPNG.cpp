#include "ImageEncoderPNG.h"

#include "Kernel/Logger.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void png_handler_error( png_structp _png_ptr, const char * _error )
        {
            MENGINE_UNUSED( _png_ptr );
            MENGINE_UNUSED( _error );

            LOGGER_ERROR( "%s"
                , _error
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void png_handler_warning( png_structp _png_ptr, const char * _error )
        {
            MENGINE_UNUSED( _png_ptr );
            MENGINE_UNUSED( _error );

            LOGGER_WARNING( "%s"
                , _error
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void png_write_proc( png_structp _png_ptr, uint8_t * data, png_size_t size )
        {
            png_voidp io_ptr = png_get_io_ptr( _png_ptr );
            OutputStreamInterface * stream = static_cast<OutputStreamInterface *>(io_ptr);

            stream->write( (char *)data, size );
        }
        //////////////////////////////////////////////////////////////////////////
        static void png_flush_proc( png_structp _png_ptr )
        {
            png_voidp io_ptr = png_get_io_ptr( _png_ptr );
            OutputStreamInterface * stream = static_cast<OutputStreamInterface *>(io_ptr);

            stream->flush();
        }
        //////////////////////////////////////////////////////////////////////////
        static png_voidp PNGAPI png_malloc_ptr( png_structp _png, png_size_t _size )
        {
            MENGINE_UNUSED( _png );

            void * p = Helper::allocateMemory( _size, "epng" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void PNGAPI png_free_ptr( png_structp _png, png_voidp _ptr )
        {
            MENGINE_UNUSED( _png );

            Helper::deallocateMemory( _ptr, "epng" );
        }
        //////////////////////////////////////////////////////////////////////////
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

        png_structp png_ptr = png_create_write_struct_2( png_ver, (png_voidp)this, &Detail::png_handler_error, &Detail::png_handler_warning, (png_voidp)this, &Detail::png_malloc_ptr, &Detail::png_free_ptr );

        MENGINE_ASSERTION_MEMORY_PANIC( png_ptr, "PNG encoder error: Can't create write structure" );

        m_png_ptr = png_ptr;

        // init the IO
        png_set_write_fn( m_png_ptr, m_stream.get(), &Detail::png_write_proc, &Detail::png_flush_proc );

        // allocate/initialize the image information data.
        png_infop info_ptr = png_create_info_struct( m_png_ptr );

        if( info_ptr == nullptr )
        {
            LOGGER_ERROR( "can't create info structure" );

            return false;
        }

        m_info_ptr = info_ptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageEncoderPNG::_finalize()
    {
        png_destroy_write_struct( &m_png_ptr, &m_info_ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageEncoderPNG::encode( const void * _buffer, size_t _size, const CodecDataInfo * _dataInfo )
    {
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const ImageCodecDataInfo * );

        const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_dataInfo);

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
            LOGGER_ERROR( "unsupported image format channels %d"
                , dataInfo->channels
            );

            return 0;
        }

        png_uint_32 width = (png_uint_32)dataInfo->width;
        png_uint_32 height = (png_uint_32)dataInfo->height;
        int32_t pixel_depth = 8;

        png_set_IHDR( m_png_ptr, m_info_ptr, width, height, pixel_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

        png_set_bgr( m_png_ptr );

        png_write_info( m_png_ptr, m_info_ptr );

        size_t pitch = m_options.pitch;

        png_bytep png_buffer = (png_bytep)_buffer;

        for( png_uint_32 k = 0; k < height; ++k )
        {
            png_write_row( m_png_ptr, png_buffer );
            png_buffer += pitch;
        }

        png_write_end( m_png_ptr, m_info_ptr );

        size_t writeBytes = pitch * height;

        return writeBytes;
    }
    //////////////////////////////////////////////////////////////////////////
}
