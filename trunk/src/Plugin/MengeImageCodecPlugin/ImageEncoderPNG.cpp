#	include "ImageEncoderPNG.h"

#	include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void s_handlerError( png_structp png_ptr, const char * _error ) 
	{
		png_voidp error_ptr = png_get_error_ptr( png_ptr );
		ImageEncoderPNG * imageEncoderPNG = static_cast<ImageEncoderPNG*>(error_ptr);

		ServiceProviderInterface * serviceProvider = imageEncoderPNG->getServiceProvider();

		LOGGER_ERROR(serviceProvider)( "ImageEncoderPNG::s_handlerError %s"
			, _error 
			);
        
        longjmp( png_jmpbuf( png_ptr ), 1);
	}
    //////////////////////////////////////////////////////////////////////////
    static void s_handlerWarning( png_structp _png_ptr, const char * _error )
    {
        png_voidp error_ptr = png_get_error_ptr( _png_ptr );
        ImageEncoderPNG * imageEncoderPNG = static_cast<ImageEncoderPNG*>(error_ptr);

        ServiceProviderInterface * serviceProvider = imageEncoderPNG->getServiceProvider();

        LOGGER_WARNING(serviceProvider)( "ImageEncoderPNG::s_handlerWarning %s"
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
		: m_png_ptr(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderPNG::~ImageEncoderPNG()
	{
		if( m_png_ptr != NULL )
		{
			png_destroy_write_struct( &m_png_ptr, NULL );
			m_png_ptr = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageEncoderPNG::encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo )
	{
		const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( _bufferDataInfo );

		png_infop info_ptr;
		png_uint_32 width = (png_uint_32)dataInfo->width;
		png_uint_32 height = (png_uint_32)dataInfo->height;
		//int bit_depth = 8;
		int color_type;
		int pitch = m_options.pitch;
		int pixel_depth = 8;

		// allocate/initialize the image information data.
		info_ptr = png_create_info_struct( m_png_ptr );

		if( info_ptr == NULL )  
		{
			LOGGER_ERROR(m_serviceProvider)( "PNG encoder error: Can't create info structure" );
			return 0;
		}

		// Set error handling.  REQUIRED if you aren't supplying your own
		// error handling functions in the png_create_write_struct() call.
		if( setjmp( png_jmpbuf( m_png_ptr ) ) )  
		{
			// if we get here, we had a problem reading the file
			png_destroy_info_struct( m_png_ptr, &info_ptr );

			return 0;
		}

		if( dataInfo->channels == 3 )
		{
			color_type = PNG_COLOR_TYPE_RGB;
		}
		else if( dataInfo->channels == 4 )
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		}
		else
		{
			LOGGER_ERROR(m_serviceProvider)( "PNG codec error: unsupported image format channels %d"
                , dataInfo->channels
                );

			png_destroy_info_struct( m_png_ptr, &info_ptr );

			return 0;
		}

		png_set_IHDR( m_png_ptr, info_ptr, width, height, pixel_depth, color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

		png_set_bgr( m_png_ptr );

		// Write the file header information.
		png_write_info( m_png_ptr, info_ptr );

		// write out the image data
		//if( dataInfo->format == PF_X8R8G8B8 )
		//{
		//	unsigned char* rowBuffer = new unsigned char[24*width];
		//	for( png_uint_32 k = 0; k < height; ++k )
		//	{
		//		for( png_uint_32 u = 0; u < width; ++u )
		//		{
		//			std::copy( _buffer+u*4, _buffer + u*4 + 3, rowBuffer + u*3 );
		//		}
		//		png_write_row( m_png_ptr, rowBuffer );
		//		_buffer += pitch;
		//	}
		//	delete[] rowBuffer;
		//}
		//else
		//{

        png_const_bytep png_buffer = (png_const_bytep)_buffer;

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

		if( m_png_ptr == NULL )  
		{
			LOGGER_ERROR(m_serviceProvider)( "PNG encoder error: Can't create write structure" 
                );

			return false;
		}

		// init the IO
		png_set_write_fn( m_png_ptr, m_stream.get(), s_writeProc, s_flushProc );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
