/*
 *	ImageEncoderPNG.cpp
 *
 *	Created by _Berserk_ on 22.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "ImageEncoderPNG.h"
#	include "Interface/FileSystemInterface.h"
#	include "LogEngine.h"

#	include "FactorableImplement.h"
#	include "PixelFormat.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void s_errorHandler( png_structp _png_ptr, const char * _error ) 
	{
		MENGE_LOG_ERROR( _error );
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
		OutStreamInterface* outStream = static_cast<OutStreamInterface*>( png_get_io_ptr( png_ptr ) );
		outStream->flush();
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT( ImageEncoderPNG );
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderPNG::ImageEncoderPNG()
		: m_png_ptr( NULL )
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
	void ImageEncoderPNG::_initialize()
	{
		if( m_stream != NULL )
		{
			m_valid = initializeEncoder_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageEncoderPNG::destructor()
	{
		this->~ImageEncoderPNG();
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageEncoderPNG::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageEncoderPNG::encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo )
	{
		if( m_valid == false )
		{
			return 0;
		}

		const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( _bufferDataInfo );
		png_infop info_ptr;
		png_uint_32 width = (png_uint_32)dataInfo->width;
		png_uint_32 height = (png_uint_32)dataInfo->height;
		int bit_depth = 8;
		int color_type;
		int pitch = width * PixelUtil::getNumElemBytes( dataInfo->format );
		int pixel_depth = 8;
		if( (m_options & DF_CUSTOM_PITCH) != 0 )
		{
			pitch = (m_options >> 16);
		}

		// allocate/initialize the image information data.
		info_ptr = png_create_info_struct( m_png_ptr );

		if( info_ptr == NULL )  
		{
			MENGE_LOG_ERROR( "PNG encoder error: Can't create info structure" );
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

		if( dataInfo->format == PF_R8G8B8 || dataInfo->format == PF_X8R8G8B8 )
		{
			color_type = PNG_COLOR_TYPE_RGB;
		}
		else if( dataInfo->format == PF_A8R8G8B8 )
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		}
		else
		{
			MENGE_LOG_ERROR( "PNG codec error: unsupported image format" );
			png_destroy_info_struct( m_png_ptr, &info_ptr );
			return 0;
		}

		png_set_IHDR( m_png_ptr, info_ptr, width, height, pixel_depth, color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

		png_set_bgr( m_png_ptr );

		// Write the file header information.
		png_write_info( m_png_ptr, info_ptr );

		// write out the image data
		for( png_uint_32 k = 0; k < height; k++) 
		{
			png_write_row( m_png_ptr, _buffer );
			_buffer += pitch;
		}
		// It is REQUIRED to call this to finish writing the rest of the file
		// Bug with png_flush
		png_write_end( m_png_ptr, info_ptr );

		png_destroy_info_struct( m_png_ptr, &info_ptr );

		return pitch * height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageEncoderPNG::initializeEncoder_()
	{
		m_png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, (png_voidp)0, s_errorHandler, s_errorHandler );

		if( m_png_ptr == NULL )  
		{
			MENGE_LOG_ERROR( "PNG encoder error: Can't create write structure" );
			return false;
		}

		// init the IO
		png_set_write_fn( m_png_ptr, m_stream, s_writeProc, s_flushProc );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
