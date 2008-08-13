
#	include "Config/Config.h"

#	include "ImageCodecPNG.h"

#	include "png.h"

#	include "LogEngine.h"

#	define PNG_BYTES_TO_CHECK 8

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void s_errorHandler( png_structp _png_ptr, const char* _error ) 
	{
		MENGE_LOG( _error );
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_readProc( png_structp _png_ptr, unsigned char* _data, png_size_t _size )
	{
		DataStreamInterface* stream = static_cast<DataStreamInterface*>( _png_ptr->io_ptr );
		stream->read( _data, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	ImageCodecPNG::ImageCodecPNG()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	String ImageCodecPNG::getType() const
	{
		return "png";
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* ImageCodecPNG::code( DataStreamInterface* input, CodecData* _data ) const
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecPNG::codeToFile( DataStreamInterface* _input, const String& _outFileName, CodecData* _pData ) const
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPNG::getDataInfo( DataStreamInterface* _inputData, Codec::CodecData* _codecData ) const
	{
		png_structp png_ptr = 0;
		png_infop info_ptr = 0;
		png_uint_32 width, height;
		//png_colorp png_palette;
		int color_type/*, palette_entries*/;
		int bit_depth, pixel_depth;		// pixel_depth = bit_depth * channels
		ImageData* codecData = static_cast<ImageData*>( _codecData );

		if( _inputData == 0 )
		{
			MENGE_LOG( "PNG codec error: PNG codec can't decode stream" );
			return false;
		}

		// check for png signature
		unsigned char png_check[PNG_BYTES_TO_CHECK];
		_inputData->read( &png_check, PNG_BYTES_TO_CHECK );

		if (png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0)
		{
			MENGE_LOG( "PNG codec error: Bad or not PNG file" );
			return false;
		}

		// create the chunk manage structure
		png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
											(png_voidp)0,
											s_errorHandler,
											s_errorHandler );

		if( png_ptr == 0 )
		{
			MENGE_LOG( "PNG codec error: Can't create read structure" );
			return false;
		}

		// create the info structure
		info_ptr = png_create_info_struct(png_ptr);

		if( info_ptr == 0 ) 
		{
			MENGE_LOG( "PNG codec error: Can't create info structure" );
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
		//png_colorp png_palette;
		int color_type/*, palette_entries*/;
		int bit_depth, pixel_depth;		// pixel_depth = bit_depth * channels

		if( _inputData == 0 )
		{
			MENGE_LOG( "PNG codec error: PNG codec can't decode stream" );
			return 0;
		}

		// check for png signature
		unsigned char png_check[PNG_BYTES_TO_CHECK];
		_inputData->read( &png_check, PNG_BYTES_TO_CHECK );

		if (png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0)
		{
			MENGE_LOG( "PNG codec error: Bad or not PNG file" );
			return 0;
		}

		// create the chunk manage structure
		png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
			(png_voidp)0,
			s_errorHandler,
			s_errorHandler );

		if( png_ptr == 0 )
		{
			MENGE_LOG( "PNG codec error: Can't create read structure" );
			return 0;
		}

		// create the info structure
		info_ptr = png_create_info_struct(png_ptr);

		if( info_ptr == 0 ) 
		{
			MENGE_LOG( "PNG codec error: Can't create info structure" );
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
					MENGE_LOG("PNG codec error: PNG format not supported" );
					return false;
		}
/*
		// unlike the example in the libpng documentation, we have *no* idea where
		// this file may have come from--so if it doesn't have a file gamma, don't
		// do any correction ("do no harm")

		double gamma = 0;
		double screen_gamma = 2.2;

		if (png_get_gAMA(png_ptr, info_ptr, &gamma) && ( flags & PNG_IGNOREGAMMA ) != PNG_IGNOREGAMMA)
		{
			png_set_gamma(png_ptr, screen_gamma, gamma);
		}
*/
		// all transformations have been registered; now update info_ptr data
		png_read_update_info(png_ptr, info_ptr);

			// read in the bitmap bits via the pointer table
		//png_read_image( png_ptr, &_buffer );
		for( png_uint_32 i = 0; i <height; i++ )
		{
			png_read_row( png_ptr, _buffer, png_bytep_NULL );
			_buffer += width * pixel_depth / 8;
		}

		// read the rest of the file, getting any additional chunks in info_ptr
		//png_read_end( png_ptr, info_ptr );

		// get possible metadata (it can be located both before and after the image data)
		//ReadMetadata( png_ptr, info_ptr, dib );

		if( png_ptr )
		{
			// clean up after the read, and free any memory allocated - REQUIRED
			png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)0 );
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge