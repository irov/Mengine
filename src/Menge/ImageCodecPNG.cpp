
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
		return ".png";
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
	Codec::DecodeResult ImageCodecPNG::decode( DataStreamInterface* _input ) const
	{
		/*png_structp png_ptr = NULL;
		png_infop info_ptr;
		png_uint_32 width, height;
		png_colorp png_palette;
		int color_type, palette_entries;
		int bit_depth, pixel_depth;		// pixel_depth = bit_depth * channels

		png_bytepp  row_pointers = NULL;
		int i;

		fi_ioStructure fio;
		fio.s_handle = handle;
		fio.s_io = io;

		if (handle) {
			try {		
				// check to see if the file is in fact a PNG file

				BYTE png_check[PNG_BYTES_TO_CHECK];

				io->read_proc(png_check, PNG_BYTES_TO_CHECK, 1, handle);

				if (png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0)
					return NULL;	// Bad signature

				// create the chunk manage structure

				png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, error_handler, warning_handler);

				if (!png_ptr)
					return NULL;			

				// create the info structure

				info_ptr = png_create_info_struct(png_ptr);

				if (!info_ptr) {
					png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
					return NULL;
				}

				// init the IO

				png_set_read_fn(png_ptr, &fio, _ReadProc);

				if (setjmp(png_jmpbuf(png_ptr))) {
					png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
					return NULL;
				}

				// because we have already read the signature...

				png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

				// read the IHDR chunk

				png_read_info(png_ptr, info_ptr);
				png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
				pixel_depth = info_ptr->pixel_depth;

				// get image data type (assume standard image type)

				FREE_IMAGE_TYPE image_type = FIT_BITMAP;
				if (bit_depth == 16) {
					if ((pixel_depth == 16) && (color_type == PNG_COLOR_TYPE_GRAY)) {
						image_type = FIT_UINT16;
					} 
					else if ((pixel_depth == 48) && (color_type == PNG_COLOR_TYPE_RGB)) {
						image_type = FIT_RGB16;
					} 
					else if ((pixel_depth == 64) && (color_type == PNG_COLOR_TYPE_RGB_ALPHA)) {
						image_type = FIT_RGBA16;
					} else {
						// tell libpng to strip 16 bit/color files down to 8 bits/color
						png_set_strip_16(png_ptr);
						bit_depth = 8;
					}
				}

#ifndef FREEIMAGE_BIGENDIAN
				if((image_type == FIT_UINT16) || (image_type == FIT_RGB16) || (image_type == FIT_RGBA16)) {
					// turn on 16 bit byte swapping
					png_set_swap(png_ptr);
				}
#endif						

				// set some additional flags

				switch(color_type) {
				case PNG_COLOR_TYPE_RGB:
				case PNG_COLOR_TYPE_RGB_ALPHA:
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
					// flip the RGB pixels to BGR (or RGBA to BGRA)

					if(image_type == FIT_BITMAP)
						png_set_bgr(png_ptr);
#endif
					break;

				case PNG_COLOR_TYPE_PALETTE:
					// expand palette images to the full 8 bits from 2 bits/pixel

					if (pixel_depth == 2) {
						png_set_packing(png_ptr);
						pixel_depth = 8;
					}					

					break;

				case PNG_COLOR_TYPE_GRAY:
					// expand grayscale images to the full 8 bits from 2 bits/pixel

					if (pixel_depth == 2) {
						png_set_expand(png_ptr);
						pixel_depth = 8;
					}

					break;

				case PNG_COLOR_TYPE_GRAY_ALPHA:
					// expand 8-bit greyscale + 8-bit alpha to 32-bit

					png_set_gray_to_rgb(png_ptr);
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
					// flip the RGBA pixels to BGRA

					png_set_bgr(png_ptr);
#endif
					pixel_depth = 32;

					break;

				default:
					throw "PNG format not supported";
				}

				// Get the background color to draw transparent and alpha images over.
				// Note that even if the PNG file supplies a background, you are not required to
				// use it - you should use the (solid) application background if it has one.

				png_color_16p image_background = NULL;
				RGBQUAD rgbBkColor;

				if (png_get_bKGD(png_ptr, info_ptr, &image_background)) {
					rgbBkColor.rgbRed      = (BYTE)image_background->red;
					rgbBkColor.rgbGreen    = (BYTE)image_background->green;
					rgbBkColor.rgbBlue     = (BYTE)image_background->blue;
					rgbBkColor.rgbReserved = 0;
				}

				// if this image has transparency, store the trns values

				png_bytep trans               = NULL;
				int num_trans                 = 0;
				//png_color_16p trans_values    = NULL;
				//png_uint_32 transparent_value = png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &trans_values);

				// unlike the example in the libpng documentation, we have *no* idea where
				// this file may have come from--so if it doesn't have a file gamma, don't
				// do any correction ("do no harm")

				double gamma = 0;
				double screen_gamma = 2.2;

				if (png_get_gAMA(png_ptr, info_ptr, &gamma) && ( flags & PNG_IGNOREGAMMA ) != PNG_IGNOREGAMMA)
					png_set_gamma(png_ptr, screen_gamma, gamma);

				// all transformations have been registered; now update info_ptr data

				png_read_update_info(png_ptr, info_ptr);

				// color type may have changed, due to our transformations

				color_type = png_get_color_type(png_ptr,info_ptr);

				// create a DIB and write the bitmap header
				// set up the DIB palette, if needed

				switch (color_type) {
				case PNG_COLOR_TYPE_RGB:
					png_set_invert_alpha(png_ptr);

					if(image_type == FIT_BITMAP) {
						dib = FreeImage_Allocate(width, height, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					} else {
						dib = FreeImage_AllocateT(image_type, width, height, pixel_depth);
					}
					break;

				case PNG_COLOR_TYPE_RGB_ALPHA:
					if(image_type == FIT_BITMAP) {
						dib = FreeImage_Allocate(width, height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					} else {
						dib = FreeImage_AllocateT(image_type, width, height, pixel_depth);
					}
					break;

				case PNG_COLOR_TYPE_PALETTE:
					dib = FreeImage_Allocate(width, height, pixel_depth);

					png_get_PLTE(png_ptr,info_ptr, &png_palette,&palette_entries);

					palette = FreeImage_GetPalette(dib);

					// store the palette

					for (i = 0; i < palette_entries; i++) {
						palette[i].rgbRed   = png_palette[i].red;
						palette[i].rgbGreen = png_palette[i].green;
						palette[i].rgbBlue  = png_palette[i].blue;
					}

					// store the transparency table

					if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
						FreeImage_SetTransparencyTable(dib, (BYTE *)trans, num_trans);					

					break;

				case PNG_COLOR_TYPE_GRAY:
					dib = FreeImage_AllocateT(image_type, width, height, pixel_depth);

					if(pixel_depth <= 8) {
						palette = FreeImage_GetPalette(dib);
						palette_entries = 1 << pixel_depth;

						for (i = 0; i < palette_entries; i++) {
							palette[i].rgbRed   =
								palette[i].rgbGreen =
								palette[i].rgbBlue  = (BYTE)((i * 255) / (palette_entries - 1));
						}
					}
					// store the transparency table

					if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
						FreeImage_SetTransparencyTable(dib, (BYTE *)trans, num_trans);					

					break;

				default:
					throw "PNG format not supported";
				}

				// store the background color 
				if(image_background) {
					FreeImage_SetBackgroundColor(dib, &rgbBkColor);
				}

				// get physical resolution

				if (png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs)) {
					png_uint_32 res_x, res_y;

					// we'll overload this var and use 0 to mean no phys data,
					// since if it's not in meters we can't use it anyway

					int res_unit_type = 0;

					png_get_pHYs(png_ptr,info_ptr,&res_x,&res_y,&res_unit_type);

					if (res_unit_type == 1) {
						FreeImage_SetDotsPerMeterX(dib, res_x);
						FreeImage_SetDotsPerMeterY(dib, res_y);
					}
				}

				// get possible ICC profile

				if (png_get_valid(png_ptr, info_ptr, PNG_INFO_iCCP)) {
					png_charp profile_name = NULL;
					png_charp profile_data = NULL;
					png_uint_32 profile_length = 0;
					int  compression_type;

					png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &profile_length);

					// copy ICC profile data (must be done after FreeImage_Allocate)

					FreeImage_CreateICCProfile(dib, profile_data, profile_length);
				}


				// set the individual row_pointers to point at the correct offsets

				row_pointers = (png_bytepp)malloc(height * sizeof(png_bytep));

				if (!row_pointers) {
					if (palette)
						png_free(png_ptr, palette);				

					png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

					FreeImage_Unload(dib);
					return NULL;
				}

				// read in the bitmap bits via the pointer table

				for (png_uint_32 k = 0; k < height; k++)				
					row_pointers[height - 1 - k] = FreeImage_GetScanLine(dib, k);			

				png_read_image(png_ptr, row_pointers);

				// check if the bitmap contains transparency, if so enable it in the header

				if (FreeImage_GetBPP(dib) == 32)
					if (FreeImage_GetColorType(dib) == FIC_RGBALPHA)
						FreeImage_SetTransparent(dib, TRUE);
					else
						FreeImage_SetTransparent(dib, FALSE);

				// cleanup

				if (row_pointers) {
					free(row_pointers);
					row_pointers = NULL;
				}

				// read the rest of the file, getting any additional chunks in info_ptr

				png_read_end(png_ptr, info_ptr);

				// get possible metadata (it can be located both before and after the image data)

				ReadMetadata(png_ptr, info_ptr, dib);

				if (png_ptr) {
					// clean up after the read, and free any memory allocated - REQUIRED
					png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
				}

				return dib;
			} catch (const char *text) {
				if (png_ptr)
					png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

				if (row_pointers)
					free(row_pointers);

				if (dib)
					FreeImage_Unload(dib);			

				FreeImage_OutputMessageProc(s_format_id, text);

				return NULL;
			}
		}			

		return NULL;*/

		DecodeResult res;
		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecPNG::getDataInfo( DataStreamInterface* _inputData, Codec::CodecData* _codecData ) const
	{
		png_structp png_ptr = 0;
		png_infop info_ptr = 0;
		png_uint_32 width, height;
		png_colorp png_palette;
		int color_type, palette_entries;
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
		png_colorp png_palette;
		int color_type, palette_entries;
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
		for( int i = 0; i <height; i++ )
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