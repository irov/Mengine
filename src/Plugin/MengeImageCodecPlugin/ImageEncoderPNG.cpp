#	include "ImageEncoderPNG.h"

#	include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void s_handlerError( png_structp _png_ptr, const char * _error )
	{
        (void)_png_ptr;

		LOGGER_ERROR("ImageEncoderPNG::s_handlerError %s"
			, _error 
			);        
	}
    //////////////////////////////////////////////////////////////////////////
    static void s_handlerWarning( png_structp _png_ptr, const char * _error )
    {
        (void)_png_ptr;

        LOGGER_WARNING("ImageEncoderPNG::s_handlerWarning %s"
            , _error 
            );       
    }
	//////////////////////////////////////////////////////////////////////////
	static void	s_writeProc( png_structp _png_ptr, unsigned char *data, png_size_t size )
	{
		png_voidp io_ptr = png_get_io_ptr( _png_ptr );
		OutputStreamInterface* stream = static_cast<OutputStreamInterface*>( io_ptr );

		stream->write( (char*)data, size );
	}
	//////////////////////////////////////////////////////////////////////////
	static void	s_flushProc( png_structp _png_ptr ) 
	{
		png_voidp io_ptr = png_get_io_ptr( _png_ptr );
		OutputStreamInterface* stream = static_cast<OutputStreamInterface*>( io_ptr );

		stream->flush();
	}
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderPNG::ImageEncoderPNG()
		: m_png_ptr(nullptr)
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
	size_t ImageEncoderPNG::encode( const void * _buffer, size_t _size, const CodecDataInfo* _bufferDataInfo )
	{
		(void)_size;

		const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( _bufferDataInfo );
			
		// allocate/initialize the image information data.
		png_infop info_ptr = png_create_info_struct( m_png_ptr );

		if( info_ptr == nullptr )  
		{
			LOGGER_ERROR("PNG encoder error: Can't create info structure" 
				);

			return 0;
		}

		int color_type;
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
			LOGGER_ERROR("PNG codec error: unsupported image format channels %d"
                , dataInfo->channels
                );

			png_destroy_info_struct( m_png_ptr, &info_ptr );

			return 0;
		}

		png_uint_32 width = (png_uint_32)dataInfo->width;
		png_uint_32 height = (png_uint_32)dataInfo->height;
		int pixel_depth = 8;
		
		png_set_IHDR( m_png_ptr, info_ptr, width, height, pixel_depth, color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

		png_set_bgr( m_png_ptr );

		// Write the file header information.
		png_write_info( m_png_ptr, info_ptr );

		size_t pitch = m_options.pitch;

        png_bytep png_buffer = (png_bytep)_buffer;

        for( png_uint_32 k = 0; k < height; ++k) 
        {
            png_write_row( m_png_ptr, png_buffer );
            png_buffer += pitch;
        }
		//}
		// It is REQUIRED to call this to finish writing the rest of the file
		// Bug with png_flush
		png_write_end( m_png_ptr, info_ptr );

		png_destroy_info_struct( m_png_ptr, &info_ptr );

        size_t writeBytes = pitch * height;

		return writeBytes;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageEncoderPNG::_initialize()
	{
		m_png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, (png_voidp)this, s_handlerError, s_handlerWarning );

		if( m_png_ptr == nullptr )  
		{
			LOGGER_ERROR("PNG encoder error: Can't create write structure" 
                );

			return false;
		}

		// init the IO
		png_set_write_fn( m_png_ptr, m_stream.get(), s_writeProc, s_flushProc );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
