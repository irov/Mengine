
#	include "Config/Config.h"

#	include "ImageCodecPNG.h"

#	include "libPNG/png.h"

#	include "LogEngine.h"

#	define PNG_BYTES_TO_CHECK 8

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void s_errorHandler( png_structp _png_ptr, const char * _error ) 
	{
		MENGE_LOG_ERROR( _error );
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_readProc( png_structp _png_ptr, unsigned char* _data, png_size_t _size )
	{
		DataStreamInterface* stream = static_cast<DataStreamInterface*>( _png_ptr->io_ptr );
		stream->read( _data, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	static void	s_writeProc( png_structp png_ptr, unsigned char *data, png_size_t size )
	{
		OutStreamInterface* outStream = static_cast<OutStreamInterface*>( png_get_io_ptr( png_ptr ) );
		outStream->write( (char*)data, size );
	}
	//////////////////////////////////////////////////////////////////////////
	static void	s_flushProc( png_structp png_ptr ) 
	{
			// empty flush implementation
	}
	//////////////////////////////////////////////////////////////////////////
	ImageCodecPNG::ImageCodecPNG()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageCodecPNG::~ImageCodecPNG()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPNG::code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const
	{
		ImageCodec::ImageData* imageData = static_cast<ImageCodec::ImageData*>( _data );
		png_structp png_ptr;
		png_infop info_ptr;
		png_uint_32 width = (png_uint_32)imageData->width;
		png_uint_32 height = (png_uint_32)imageData->height;
		int bit_depth = 8;
		int color_type;
		int pitch = width * PixelUtil::getNumElemBytes( imageData->format );
		int pixel_depth = 8;
		unsigned char* buffer = _buffer;


		png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, (png_voidp)0, s_errorHandler, s_errorHandler );

		if( png_ptr == 0 )  
		{
			MENGE_LOG_ERROR( "PNG codec error: Can't create write structure" );
			return false;
		}

		// allocate/initialize the image information data.
		info_ptr = png_create_info_struct( png_ptr );

		if( info_ptr == 0 )  
		{
			MENGE_LOG_ERROR( "PNG codec error: Can't create info structure" );
			png_destroy_write_struct( &png_ptr, (png_infopp)0 );
			return false;
		}

		// Set error handling.  REQUIRED if you aren't supplying your own
		// error handling functions in the png_create_write_struct() call.
		if( setjmp( png_jmpbuf( png_ptr ) ) )  
		{
			// if we get here, we had a problem reading the file
			png_destroy_write_struct( &png_ptr, &info_ptr );
			return false;
		}

		// init the IO
		png_set_write_fn( png_ptr, _outStream, s_writeProc, s_flushProc );

		if( imageData->format == PF_R8G8B8 || imageData->format == PF_X8R8G8B8 )
		{
			color_type = PNG_COLOR_TYPE_RGB;
		}
		else if( imageData->format == PF_A8R8G8B8 )
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		}
		else
		{
			MENGE_LOG_ERROR( "PNG codec error: unsupported image format" );
			png_destroy_write_struct( &png_ptr, &info_ptr );
			return false;
		}

		png_set_IHDR( png_ptr, info_ptr, width, height, pixel_depth, color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

		png_set_bgr( png_ptr );

		// Write the file header information.
		png_write_info(png_ptr, info_ptr);

		// write out the image data
		for( png_uint_32 k = 0; k < height; k++) 
		{
			png_write_row( png_ptr, buffer );
			buffer += pitch;
		}
		// It is REQUIRED to call this to finish writing the rest of the file
		// Bug with png_flush
		png_write_end( png_ptr, info_ptr );

		png_destroy_write_struct( &png_ptr, &info_ptr );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPNG::getDataInfo( DataStreamInterface* _inputData, CodecInterface::CodecData* _codecData ) const
	{
		png_structp png_ptr = 0;
		png_infop info_ptr = 0;
		png_uint_32 width, height;
		int color_type;
		int bit_depth, pixel_depth;		// pixel_depth = bit_depth * channels
		ImageData* codecData = static_cast<ImageData*>( _codecData );

		if( _inputData == 0 )
		{
			MENGE_LOG_ERROR( "PNG codec error: PNG codec can't decode stream" );
			return false;
		}

		// check for png signature
		unsigned char png_check[PNG_BYTES_TO_CHECK];
		_inputData->read( &png_check, PNG_BYTES_TO_CHECK );

		if (png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0)
		{
			MENGE_LOG_ERROR( "PNG codec error: Bad or not PNG file" );
			return false;
		}

		// create the chunk manage structure
		png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
											(png_voidp)0,
											s_errorHandler,
											s_errorHandler );

		if( png_ptr == 0 )
		{
			MENGE_LOG_ERROR( "PNG codec error: Can't create read structure" );
			return false;
		}

		// create the info structure
		info_ptr = png_create_info_struct(png_ptr);

		if( info_ptr == 0 ) 
		{
			MENGE_LOG_ERROR( "PNG codec error: Can't create info structure" );
			png_destroy_read_struct( &png_ptr, (png_infopp)0, (png_infopp)0 );
			return false;
		}

		// init the IO
		png_set_read_fn( png_ptr, _inputData, s_readProc );

		if( setjmp( png_jmpbuf( png_ptr ) ) ) 
		{
			png_destroy_read_struct( &png_ptr, &info_ptr, 0 );
			return false;
		}

		// because we have already read the signature...
		png_set_sig_bytes( png_ptr, PNG_BYTES_TO_CHECK );

		// read the IHDR chunk
		png_read_info( png_ptr, info_ptr );
		png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL );
		pixel_depth = info_ptr->pixel_depth;

		std::size_t decodedDataSize = pixel_depth * width * height / 8;

		codecData->width = width;
		codecData->height = height;
		codecData->depth = 1;
		codecData->size = decodedDataSize;
		codecData->num_mipmaps = 0;
		codecData->flags = 0;
		codecData->format = PF_UNKNOWN;
		if( bit_depth == 16 ) 
		{
			if( (pixel_depth == 16) && (color_type == PNG_COLOR_TYPE_GRAY) )
			{
				codecData->format = PF_L16;
			} 
			else if( (pixel_depth == 48) && (color_type == PNG_COLOR_TYPE_RGB) )
			{
				codecData->format = PF_SHORT_RGB;
			} 
			else if( (pixel_depth == 64) && (color_type == PNG_COLOR_TYPE_RGB_ALPHA) )
			{
				codecData->format = PF_SHORT_RGBA;
			}
		}
		if( pixel_depth == 32 )
		{
			codecData->format = PF_A8R8G8B8;
		}
		else
		{
			codecData->format = PF_R8G8B8;
		}

		if( png_ptr )
		{
			// clean up after the read, and free any memory allocated - REQUIRED
			png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)0 );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPNG::decode( DataStreamInterface* _inputData, unsigned char* _buffer, unsigned int _options ) const
	{
		png_structp png_ptr = 0;
		png_infop info_ptr = 0;
		png_uint_32 width, height;
		int color_type;
		int bit_depth, pixel_depth;		// pixel_depth = bit_depth * channels
		unsigned char* readBuffer = _buffer;

		if( _inputData == 0 )
		{
			MENGE_LOG_ERROR( "PNG codec error: PNG codec can't decode stream" );
			return 0;
		}

		// check for png signature
		unsigned char png_check[PNG_BYTES_TO_CHECK];
		_inputData->read( &png_check, PNG_BYTES_TO_CHECK );

		if (png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0)
		{
			MENGE_LOG_ERROR( "PNG codec error: Bad or not PNG file" );
			return 0;
		}

		// create the chunk manage structure
		png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
			(png_voidp)0,
			s_errorHandler,
			s_errorHandler );

		if( png_ptr == 0 )
		{
			MENGE_LOG_ERROR( "PNG codec error: Can't create read structure" );
			return 0;
		}

		// create the info structure
		info_ptr = png_create_info_struct(png_ptr);

		if( info_ptr == 0 ) 
		{
			MENGE_LOG_ERROR( "PNG codec error: Can't create info structure" );
			png_destroy_read_struct( &png_ptr, (png_infopp)0, (png_infopp)0 );
			return 0;
		}

		// init the IO
		png_set_read_fn( png_ptr, _inputData, s_readProc );

		if( setjmp( png_jmpbuf( png_ptr ) ) ) 
		{
			png_destroy_read_struct( &png_ptr, &info_ptr, 0 );
			return 0;
		}

		// because we have already read the signature...
		png_set_sig_bytes( png_ptr, PNG_BYTES_TO_CHECK );

		// read the IHDR chunk
		png_read_info( png_ptr, info_ptr );
		png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL );
		pixel_depth = info_ptr->pixel_depth;			

		// set some additional flags
		switch( color_type )
		{
				case PNG_COLOR_TYPE_RGB:
				case PNG_COLOR_TYPE_RGB_ALPHA:
//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
//					// flip the RGB pixels to BGR (or RGBA to BGRA)
//
//					if(image_type == FIT_BITMAP)
					png_set_bgr(png_ptr);
//#endif
					break;

				case PNG_COLOR_TYPE_PALETTE:
					// expand palette images to the full 8 bits from 2 bits/pixel

					if( pixel_depth == 2 )
					{
						png_set_packing( png_ptr );
						pixel_depth = 8;
					}					

					break;

				case PNG_COLOR_TYPE_GRAY:
					// expand grayscale images to the full 8 bits from 2 bits/pixel

					if( pixel_depth == 2 )
					{
						png_set_expand(png_ptr);
						pixel_depth = 8;
					}

					break;

				case PNG_COLOR_TYPE_GRAY_ALPHA:
					// expand 8-bit greyscale + 8-bit alpha to 32-bit

					png_set_gray_to_rgb( png_ptr );
//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
//					// flip the RGBA pixels to BGRA
//
					png_set_bgr(png_ptr);
//#endif
					pixel_depth = 32;

					break;

				default:
					MENGE_LOG_ERROR( "PNG codec error: PNG format not supported" );
					return false;
		}

		// all transformations have been registered; now update info_ptr data
		png_read_update_info(png_ptr, info_ptr);

		int row_stride = width * pixel_depth / 8;
		if( _options & DF_CUSTOM_PITCH > 0 )
		{
			row_stride = _options >> 16;
		}
		
		if( _options & DF_READ_ALPHA_ONLY > 0 )
		{
			unsigned char* rowBuffer = new unsigned char[row_stride];
			for( png_uint_32 i = 0; i <height; i++ )
			{
				png_read_row( png_ptr, rowBuffer, png_bytep_NULL );
				for( int i = 0; i < row_stride; i++ )
				{
					readBuffer[i*4 + 3] = rowBuffer[i];
				}
				readBuffer += row_stride * 4;
			}
			delete[] rowBuffer;
		}
		else
		{
			for( png_uint_32 i = 0; i <height; i++ )
			{
				png_read_row( png_ptr, readBuffer, png_bytep_NULL );
				readBuffer += row_stride;
			}
		}

		if( png_ptr )
		{
			// clean up after the read, and free any memory allocated - REQUIRED
			png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)0 );
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPNG::start( DataStreamInterface* _inputData, CodecData* _codecData )
	{
		// streaming not implemented
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ImageCodecPNG::read( void* _buf, std::streamsize _count )
	{
		// streaming not implemented
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPNG::eof() const
	{
		// streaming not implemented
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	int ImageCodecPNG::sync( float _timing )
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecPNG::finish()
	{
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
