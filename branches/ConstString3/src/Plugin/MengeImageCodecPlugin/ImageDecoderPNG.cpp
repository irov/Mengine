/*
 *	ImageDecoderPNG.cpp
 *
 *	Created by _Berserk_ on 18.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "ImageDecoderPNG.h"
#	include "Utils/Logger/Logger.h"
#	include "Interface/FileSystemInterface.h"

#	define PNG_BYTES_TO_CHECK 8

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void s_errorHandler( png_structp _png_ptr, const char * _error ) 
	{
		png_voidp error_ptr = png_get_error_ptr( _png_ptr );		
		LogSystemInterface * log = static_cast<ImageDecoderPNG*>(error_ptr)->getLogSystem();

		LOGGER_ERROR(log)( _error );
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_readProc( png_structp _png_ptr, unsigned char* _data, png_size_t _size )
	{
		png_voidp io_ptr = png_get_io_ptr( _png_ptr );
		FileInputStreamInterface* stream = static_cast<FileInputStreamInterface*>( io_ptr );

		stream->read( _data, (std::streamsize)_size );
	}
	//////////////////////////////////////////////////////////////////////////
	static void	s_flushProc( png_structp png_ptr ) 
	{
		// empty flush implementation
	}
	//////////////////////////////////////////////////////////////////////////
	void s_cleanup( png_structp _png_ptr )
	{
		if( _png_ptr != NULL )
		{
			png_destroy_read_struct( &_png_ptr, 0, (png_infopp)0 );
			_png_ptr = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPNG::ImageDecoderPNG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem )
		: ImageDecoder(_service, _stream)
		, m_logSystem(_logSystem)
		, m_png_ptr(NULL)
		, m_row_bytes(0)
		, m_supportA8(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPNG::~ImageDecoderPNG()
	{
		s_cleanup( m_png_ptr );
	}
	//////////////////////////////////////////////////////////////////////////
	LogSystemInterface * ImageDecoderPNG::getLogSystem()
	{
		return m_logSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderPNG::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{	
		if( (m_options.flags & DF_CUSTOM_PITCH) == false )
		{
			m_options.pitch = m_row_bytes;
		}

		if( m_options.flags & DF_READ_ALPHA_ONLY )
		{
			if( m_dataInfo.channels == 1 )
			{
				if( _bufferSize != m_options.pitch * m_dataInfo.height )
				{
					LOGGER_ERROR(m_logSystem)( "ImageDecoderPNG::decode Error: alphaMask A8 bad buffer size %i pitch %i height %i ",_bufferSize, m_options.pitch ,m_dataInfo.height );
					return 0;
				}

				png_byte **row_pointers = new png_byte * [m_dataInfo.height];

				for (unsigned int i = 0; i != m_dataInfo.height; ++i)
				{
					row_pointers[i] = _buffer + i * m_options.pitch;
				}

				// все, читаем картинку
				png_read_image(m_png_ptr, row_pointers);

				delete [] row_pointers;
			}
			else
			{
				if( _bufferSize * 4 != m_options.pitch * m_dataInfo.height )
				{
					LOGGER_ERROR(m_logSystem)( "ImageDecoderPNG::decode Error: DF_READ_ALPHA_ONLY bad buffer size" );
					return 0;
				}

				unsigned char* buff = new unsigned char[m_row_bytes];

				for( std::size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, buff, NULL );

					size_t row_alpha = m_row_bytes / 4;
					for( size_t j = 0; j < row_alpha; ++j )
					{
						_buffer[j] = buff[j*4+3];
					}

					_buffer += m_options.pitch / 4;
				}

				delete[] buff;
			}
		}
		else if( m_dataInfo.channels == 1 )
		{
			if( m_supportA8 == false )
			{
				if( _bufferSize != m_options.pitch * 4 * m_dataInfo.height )
				{
					LOGGER_ERROR(m_logSystem)( "ImageDecoderPNG::decode Error: alphaMask w/o A8 bad buffer size" );
					return 0;
				}

				unsigned char* buff = new unsigned char[m_row_bytes];

				for( std::size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, buff, NULL );

					for( size_t j = 0; j != m_row_bytes; ++j )
					{
						_buffer[j*4+0] = 255;
						_buffer[j*4+1] = 255;
						_buffer[j*4+2] = 255;
						_buffer[j*4+3] = buff[j];
					}

					_buffer += m_options.pitch * 4;
				}

				delete[] buff;
			}
			else
			{
				if( _bufferSize != m_options.pitch * m_dataInfo.height )
				{
					LOGGER_ERROR(m_logSystem)( "ImageDecoderPNG::decode Error: alphaMask A8 bad buffer size" );
					return 0;
				}

				png_byte **row_pointers = new png_byte * [m_dataInfo.height];

				for (unsigned int i = 0; i != m_dataInfo.height; ++i)
				{
					row_pointers[i] = _buffer + i * m_options.pitch;
				}

				// все, читаем картинку
				png_read_image(m_png_ptr, row_pointers);

				delete [] row_pointers;
			}
		}
		else
		{
			switch( m_dataInfo.channels )
			{
			case 3:
				{
					if( _bufferSize != m_options.pitch * m_dataInfo.height )
					{
						LOGGER_ERROR(m_logSystem)( "ImageDecoderPNG::decode Error: bad buffer size" );
						return 0;
					}

					//png_byte **row_pointers = new png_byte * [m_dataInfo.height];

					//for (unsigned int i = 0; i != m_dataInfo.height; ++i)
					//{
					//	row_pointers[i] = _buffer + i * m_options.pitch;
					//}

					//// все, читаем картинку
					//png_read_image(m_png_ptr, row_pointers);

					//delete [] row_pointers;

					////	place a little magic here =)
					//std::size_t bufferDataWidth = m_dataInfo.width * 4;
					//for( std::size_t i = 0; i < m_dataInfo.width; i++ )
					//{
					//	std::copy( 
					//		_buffer + 3 * ( m_dataInfo.width - i - 1 ), 
					//		_buffer + 3 * ( m_dataInfo.width - i ), 
					//		_buffer + bufferDataWidth - 4 - i*4 
					//		);

					//	_buffer[bufferDataWidth-i*4-1] = 255; // alpha
					//}
					unsigned char* buff = new unsigned char[m_row_bytes];

					for( std::size_t i = 0; i != m_dataInfo.height; ++i )
					{
						png_read_row( m_png_ptr, buff, NULL );

						size_t row = m_row_bytes / 3;
						for( size_t j = 0; j < row; ++j )
						{
							_buffer[j*4 + 0] = buff[j*3 + 0];
							_buffer[j*4 + 1] = buff[j*3 + 1];
							_buffer[j*4 + 2] = buff[j*3 + 2];
							_buffer[j*4 + 3] = 255;
						}

						_buffer += m_options.pitch;
					}

					delete[] buff;
				}break;
			case 4:
				{
					if( _bufferSize != m_options.pitch * m_dataInfo.height )
					{
						LOGGER_ERROR(m_logSystem)( "ImageDecoderPNG::decode Error: bad buffer size" );
						return 0;
					}

					png_byte **row_pointers = new png_byte * [m_dataInfo.height];

					for (unsigned int i = 0; i != m_dataInfo.height; ++i)
					{
						row_pointers[i] = _buffer + i * m_options.pitch;
					}

					png_read_image(m_png_ptr, row_pointers);

					delete [] row_pointers;
				}break;
			}

		}

		png_read_end(m_png_ptr, 0);

		png_destroy_read_struct(&m_png_ptr, 0, 0);

		return _bufferSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderPNG::_invalidate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderPNG::initialize()
	{
		m_supportA8 = m_service->supportA8();

		png_infop info_ptr = 0;

		// check for png signature
		unsigned char png_check[PNG_BYTES_TO_CHECK];
		m_stream->read( &png_check, PNG_BYTES_TO_CHECK );

		if( png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0 )
		{
			LOGGER_ERROR(m_logSystem)( "PNG codec error: Bad or not PNG file" );
			return false;
		}

		// create the chunk manage structure
		m_png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
			(png_voidp)this,
			s_errorHandler,
			s_errorHandler );

		if( m_png_ptr == 0 )
		{
			LOGGER_ERROR(m_logSystem)( "PNG codec error: Can't create read structure" );
			return false;
		}

		// create the info structure
		info_ptr = png_create_info_struct( m_png_ptr );

		if( info_ptr == 0 ) 
		{
			LOGGER_ERROR(m_logSystem)( "PNG codec error: Can't create info structure" );
			return false;
		}

		// init the IO
		png_set_read_fn( m_png_ptr, m_stream, s_readProc );

		if( setjmp( png_jmpbuf( m_png_ptr ) ) ) 
		{
			LOGGER_ERROR(m_logSystem)( "PNG codec error" );
			png_destroy_info_struct( m_png_ptr, &info_ptr );
			return false;
		}

		// because we have already read the signature...
		png_set_sig_bytes( m_png_ptr, PNG_BYTES_TO_CHECK );

		// read the IHDR chunk
		png_read_info( m_png_ptr, info_ptr );

		png_uint_32 width = 0;
		png_uint_32 height = 0;
		int color_type = 0;
		int bit_depth = 0;
		int interlace_method = 0;
		
		png_get_IHDR( m_png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, NULL, NULL );

		if( interlace_method != PNG_INTERLACE_NONE )
		{
			png_set_interlace_handling( m_png_ptr );
		}

		switch( color_type )
		{
		case PNG_COLOR_TYPE_RGB:
		case PNG_COLOR_TYPE_RGB_ALPHA:
			png_set_bgr(m_png_ptr);
			break;

		case PNG_COLOR_TYPE_GRAY:
			
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			
			png_set_strip_alpha(m_png_ptr);
			break;
		};
		//png_set_expand( m_png_ptr );
		//png_set_bgr(m_png_ptr);
		//// set some additional flags
		//switch( color_type )
		//{
		//case PNG_COLOR_TYPE_RGB:
		//case PNG_COLOR_TYPE_RGB_ALPHA:
		//	//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
		//	//					// flip the RGB pixels to BGR (or RGBA to BGRA)
		//	//
		//	//					if(image_type == FIT_BITMAP)
		//	png_set_bgr(m_png_ptr);
		//	//#endif
		//	break;

		//case PNG_COLOR_TYPE_PALETTE:
		//	// expand palette images to the full 8 bits from 2 bits/pixel

		//	if( pixel_depth == 2 )
		//	{
		//		png_set_packing( m_png_ptr );
		//		pixel_depth = 8;
		//	}					

		//	break;

		//case PNG_COLOR_TYPE_GRAY:
		//	// expand grayscale images to the full 8 bits from 2 bits/pixel

		//	if( pixel_depth == 2 )
		//	{
		//		png_set_expand(m_png_ptr);
		//		pixel_depth = 8;
		//	}

		//	break;

		//case PNG_COLOR_TYPE_GRAY_ALPHA:
		//	// expand 8-bit greyscale + 8-bit alpha to 32-bit

		//	png_set_gray_to_rgb( m_png_ptr );
		//	//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
		//	//					// flip the RGBA pixels to BGRA
		//	//
		//	png_set_bgr(m_png_ptr);
		//	//#endif
		//	pixel_depth = 32;

		//	break;

		//default:
		//	MENGE_LOG_ERROR( "PNG codec error: PNG format not supported" );
		//	return false;
		//}

		// all transformations have been registered; now update info_ptr data
		png_read_update_info( m_png_ptr, info_ptr );
		png_get_IHDR(m_png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);

		m_row_bytes = png_get_rowbytes(m_png_ptr, info_ptr);

		png_byte channels = png_get_channels(m_png_ptr, info_ptr);

		std::size_t decodedDataSize = m_row_bytes * height;

		m_dataInfo.width = width;
		m_dataInfo.height = height;
		m_dataInfo.depth = 1;
		m_dataInfo.size = decodedDataSize;
		m_dataInfo.num_mipmaps = 0;
		m_dataInfo.flags = 0;
		m_dataInfo.format = PF_UNKNOWN;
		m_dataInfo.channels = channels;

		//if( bit_depth == 16 ) 
		//{
		//	if( (pixel_depth == 16) && (color_type == PNG_COLOR_TYPE_GRAY) )
		//	{
		//		m_dataInfo.format = PF_L16;
		//	} 
		//	else if( (pixel_depth == 48) && (color_type == PNG_COLOR_TYPE_RGB) )
		//	{
		//		m_dataInfo.format = PF_SHORT_RGB;
		//	} 
		//	else if( (pixel_depth == 64) && (color_type == PNG_COLOR_TYPE_RGB_ALPHA) )
		//	{
		//		m_dataInfo.format = PF_SHORT_RGBA;
		//	}
		//}
		if( channels == 4 )
		{
			m_dataInfo.format = PF_A8R8G8B8;
		}
		else if( channels == 3 )
		{
			m_dataInfo.format = PF_R8G8B8;
		}
		else if( channels == 1 )
		{
			if( ( m_options.flags & DF_READ_ALPHA_ONLY ) == false )
			{
				if( m_supportA8 == false )
				{
					m_dataInfo.format = PF_A8R8G8B8;

					m_dataInfo.size = decodedDataSize * 4;
				}
				else
				{
					m_dataInfo.format = PF_A8;
				}
			}
		}

		png_destroy_info_struct( m_png_ptr, &info_ptr );

		if( bit_depth != 8 )
		{
			LOGGER_ERROR(m_logSystem)( "PNG codec error: Can't support non 8 bit depth - '%d'"
				, bit_depth
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
