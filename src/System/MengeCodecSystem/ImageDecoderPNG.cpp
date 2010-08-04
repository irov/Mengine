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
		MENGE_LOG_ERROR( _error );
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_readProc( png_structp _png_ptr, unsigned char* _data, png_size_t _size )
	{
		FileInputInterface* stream = static_cast<FileInputInterface*>( _png_ptr->io_ptr );
		stream->read( _data, (std::streamsize)_size );
	}
	//////////////////////////////////////////////////////////////////////////
	static void	s_flushProc( png_structp png_ptr ) 
	{
		// empty flush implementation
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPNG::ImageDecoderPNG()
		: m_png_ptr( NULL )
		, m_rowsRead( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPNG::~ImageDecoderPNG()
	{
		cleanup_();
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderPNG::_initialize()
	{
		if( m_stream != NULL )
		{
			m_valid = readHeader_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderPNG::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		if( m_valid == false )
		{
			return 0;
		}

		bool alphaOnly = (( m_options & DF_READ_ALPHA_ONLY ) != 0);

		if( !alphaOnly && (_bufferSize % m_bufferRowStride != 0) )
		{
			MENGE_LOG_ERROR( "ImageDecoderPNG::decode Error: bad buffer size" );
			return 0;
		}

		if( !alphaOnly && (m_bufferRowStride < m_rowStride) )
		{
			MENGE_LOG_ERROR( "ImageDecoderPNG::decode Error: bad buffer pitch" );
			return 0;
		}

		if( alphaOnly == true )
		{
			if( _bufferSize != m_dataInfo.width * m_dataInfo.height )
			{
				MENGE_LOG_ERROR( "ImageDecoderPNG::decode Error: bad buffer size" );
				return 0;
			}
			if( m_dataInfo.format != PF_A8R8G8B8
				&& m_dataInfo.format != PF_A8 )
			{
				std::fill( _buffer, _buffer + _bufferSize, 0xFF );
				return _bufferSize;
			}
			unsigned char* tempBuffer = new unsigned char[m_rowStride];
			while( m_rowsRead < m_dataInfo.height )
			{
				png_read_row( m_png_ptr, tempBuffer, NULL );
				++m_rowsRead;
				for( size_t i = 0; i < m_dataInfo.width; ++i )
				{
					_buffer[i] = tempBuffer[i*4+3];
				}
				_buffer += m_dataInfo.width;
			}
			delete[] tempBuffer;
		}
		// else
		{
			while( _bufferSize >= m_rowStride && m_rowsRead < m_dataInfo.height )
			{
				png_read_row( m_png_ptr, _buffer, NULL );
				m_rowsRead++;

				if( (m_options & DF_COUNT_ALPHA) != 0 )	// place data as there is alpha
				{
					// place a little magic here =)
					std::size_t bufferDataWidth = m_dataInfo.width * 4;
					for( std::size_t i = 0; i < m_dataInfo.width; i++ )
					{
						std::copy( _buffer + 3 * ( m_dataInfo.width - i - 1 ), _buffer + 3 * ( m_dataInfo.width - i ), _buffer + bufferDataWidth - 4 - i*4 );
						_buffer[bufferDataWidth-i*4-1] = 255; // alpha

					}
				}

				_buffer += m_bufferRowStride;
				_bufferSize -= m_bufferRowStride;
			}

		}
		return m_rowsRead * m_rowStride;

	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderPNG::setOptions( unsigned int _options )
	{
		ImageDecoder::setOptions( _options );

		if( ( m_options & DF_CUSTOM_PITCH ) != 0 )
		{
			m_bufferRowStride = m_options >> 16;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderPNG::readHeader_()
	{
		png_infop info_ptr = 0;
		png_uint_32 width, height;
		int color_type;
		int bit_depth, pixel_depth;		// pixel_depth = bit_depth * channels

		// check for png signature
		unsigned char png_check[PNG_BYTES_TO_CHECK];
		m_stream->read( &png_check, PNG_BYTES_TO_CHECK );

		if( png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0 )
		{
			MENGE_LOG_ERROR( "PNG codec error: Bad or not PNG file" );
			return false;
		}

		// create the chunk manage structure
		m_png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
			(png_voidp)0,
			s_errorHandler,
			s_errorHandler );

		if( m_png_ptr == 0 )
		{
			MENGE_LOG_ERROR( "PNG codec error: Can't create read structure" );
			return false;
		}

		// create the info structure
		info_ptr = png_create_info_struct( m_png_ptr );

		if( info_ptr == 0 ) 
		{
			MENGE_LOG_ERROR( "PNG codec error: Can't create info structure" );
			return false;
		}

		// init the IO
		png_set_read_fn( m_png_ptr, m_stream, s_readProc );

		if( setjmp( png_jmpbuf( m_png_ptr ) ) ) 
		{
			MENGE_LOG_ERROR( "PNG codec error" );
			png_destroy_info_struct( m_png_ptr, &info_ptr );
			return false;
		}

		// because we have already read the signature...
		png_set_sig_bytes( m_png_ptr, PNG_BYTES_TO_CHECK );

		// read the IHDR chunk
		png_read_info( m_png_ptr, info_ptr );
		png_get_IHDR( m_png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL );
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
			png_set_bgr(m_png_ptr);
			//#endif
			break;

		case PNG_COLOR_TYPE_PALETTE:
			// expand palette images to the full 8 bits from 2 bits/pixel

			if( pixel_depth == 2 )
			{
				png_set_packing( m_png_ptr );
				pixel_depth = 8;
			}					

			break;

		case PNG_COLOR_TYPE_GRAY:
			// expand grayscale images to the full 8 bits from 2 bits/pixel

			if( pixel_depth == 2 )
			{
				png_set_expand(m_png_ptr);
				pixel_depth = 8;
			}

			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			// expand 8-bit greyscale + 8-bit alpha to 32-bit

			png_set_gray_to_rgb( m_png_ptr );
			//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
			//					// flip the RGBA pixels to BGRA
			//
			png_set_bgr(m_png_ptr);
			//#endif
			pixel_depth = 32;

			break;

		default:
			MENGE_LOG_ERROR( "PNG codec error: PNG format not supported" );
			return false;
		}

		// all transformations have been registered; now update info_ptr data
		png_read_update_info( m_png_ptr, info_ptr );

		std::size_t decodedDataSize = pixel_depth * width * height / 8;

		m_dataInfo.width = width;
		m_dataInfo.height = height;
		m_dataInfo.depth = 1;
		m_dataInfo.size = decodedDataSize;
		m_dataInfo.num_mipmaps = 0;
		m_dataInfo.flags = 0;
		m_dataInfo.format = PF_UNKNOWN;
		if( bit_depth == 16 ) 
		{
			if( (pixel_depth == 16) && (color_type == PNG_COLOR_TYPE_GRAY) )
			{
				m_dataInfo.format = PF_L16;
			} 
			else if( (pixel_depth == 48) && (color_type == PNG_COLOR_TYPE_RGB) )
			{
				m_dataInfo.format = PF_SHORT_RGB;
			} 
			else if( (pixel_depth == 64) && (color_type == PNG_COLOR_TYPE_RGB_ALPHA) )
			{
				m_dataInfo.format = PF_SHORT_RGBA;
			}
		}
		if( pixel_depth == 32 )
		{
			m_dataInfo.format = PF_A8R8G8B8;
		}
		else if( pixel_depth == 24 )
		{
			m_dataInfo.format = PF_R8G8B8;
		}
		else if( pixel_depth == 8 )
		{
			m_dataInfo.format = PF_A8;
		}

		png_destroy_info_struct( m_png_ptr, &info_ptr );

		m_rowStride = m_bufferRowStride = pixel_depth * width / 8;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderPNG::cleanup_()
	{
		if( m_png_ptr != NULL )
		{
			png_destroy_read_struct( &m_png_ptr, 0, (png_infopp)0 );
			m_png_ptr = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
