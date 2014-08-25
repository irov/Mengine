#	include "ImageDecoderPNG.h"

#	include "Interface/CacheInterface.h"

#   include "Config/Blobject.h"
#	include "Core/CacheMemoryBuffer.h"

#	include "Logger/Logger.h"

#	include "stdex/allocator.h"

#	define PNG_BYTES_TO_CHECK 8

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void s_handlerError( png_structp png_ptr, const char * _error ) 
	{
		png_voidp error_ptr = png_get_error_ptr( png_ptr );		
		ImageDecoderPNG * imageDecoderPNG = static_cast<ImageDecoderPNG*>(error_ptr);

		ServiceProviderInterface * serviceProvider = imageDecoderPNG->getServiceProvider();

		LOGGER_ERROR(serviceProvider)("ImageDecoderPNG::s_handlerError '%s'"
			, _error 
			);

		longjmp(png_jmpbuf(png_ptr), 1);
	}
	//////////////////////////////////////////////////////////////////////////
	static void PNGAPI s_handlerWarning( png_structp png_ptr, const char * _error )
	{
		png_voidp error_ptr = png_get_error_ptr( png_ptr );		
		ImageDecoderPNG * imageDecoderPNG = static_cast<ImageDecoderPNG*>(error_ptr);
		ServiceProviderInterface * serviceProvider = imageDecoderPNG->getServiceProvider();

		LOGGER_INFO(serviceProvider)("ImageDecoderPNG::s_handlerWarning '%s'"
			, _error 
			);
	}
	//////////////////////////////////////////////////////////////////////////
	static void PNGAPI s_readProc( png_structp _png_ptr, unsigned char * _data, png_size_t _size )
	{
		png_voidp io_ptr = png_get_io_ptr( _png_ptr );

		InputStreamInterface * stream = static_cast<InputStreamInterface *>( io_ptr );

		stream->read( _data, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPNG::ImageDecoderPNG()
		: m_png_ptr(nullptr)
		, m_info_ptr(nullptr)
		, m_row_bytes(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPNG::~ImageDecoderPNG()
	{			
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderPNG::_initialize()
	{
		png_const_charp png_ver = PNG_LIBPNG_VER_STRING;

		m_png_ptr = png_create_read_struct( png_ver, (png_voidp)this, &s_handlerError, &s_handlerWarning );

		if( m_png_ptr == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::initialize Can't create read structure" 
				);

			return false;
		}

		m_info_ptr = png_create_info_struct( m_png_ptr );

		if( m_info_ptr == nullptr ) 
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::initialize Can't create info structure" 
				);

			png_destroy_write_struct( &m_png_ptr, nullptr );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderPNG::_finalize()
	{
		png_destroy_read_struct( &m_png_ptr, &m_info_ptr, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderPNG::_prepareData()
	{
		unsigned char png_check[PNG_BYTES_TO_CHECK];
		m_stream->read( &png_check, PNG_BYTES_TO_CHECK );

		if( png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::initialize Bad or not PNG file" 
				);

			return false;
		}

		if( setjmp( png_jmpbuf( m_png_ptr ) ) )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::initialize" 
				);

			png_destroy_write_struct( &m_png_ptr, &m_info_ptr );

			return false;
		}

		png_set_read_fn( m_png_ptr, m_stream.get(), &s_readProc );

		png_set_sig_bytes( m_png_ptr, PNG_BYTES_TO_CHECK );

		png_read_info( m_png_ptr, m_info_ptr );

		png_uint_32 width = 0;
		png_uint_32 height = 0;
		int color_type = 0;
		int bit_depth = 0;
		int interlace_method = 0;

		png_get_IHDR( m_png_ptr, m_info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, nullptr, nullptr );

		if( interlace_method != PNG_INTERLACE_NONE )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::initialize PNG is interlacing (Engine not support to read this png format)"
				);

			png_destroy_info_struct( m_png_ptr, &m_info_ptr );

			return false;
		}

		switch( color_type )
		{
		case PNG_COLOR_TYPE_RGB:
		case PNG_COLOR_TYPE_RGB_ALPHA:
#   ifndef MENGE_RENDER_TEXTURE_RGBA
			png_set_bgr(m_png_ptr);
#   endif
			break;

		case PNG_COLOR_TYPE_GRAY:

			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:

			png_set_strip_alpha(m_png_ptr);
			break;
		};

		png_read_update_info( m_png_ptr, m_info_ptr );

		png_get_IHDR( m_png_ptr, m_info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0 );

		m_row_bytes = png_get_rowbytes( m_png_ptr, m_info_ptr );

		png_byte channels = png_get_channels( m_png_ptr, m_info_ptr );

		size_t decodedDataSize = m_row_bytes * height;

		m_dataInfo.width = width;
		m_dataInfo.height = height;
		m_dataInfo.depth = 1;
		m_dataInfo.size = decodedDataSize;
		m_dataInfo.mipmaps = 0;
		m_dataInfo.channels = channels;

		if( bit_depth != 8 )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::initialize Can't support non 8 bit depth - '%d'"
				, bit_depth
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderPNG::decode( void * _buffer, size_t _bufferSize )
	{	
		if( _bufferSize < m_options.pitch * m_dataInfo.height )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::decode invalid bufferSize %d != (%d * %d)"
				, _bufferSize
				, m_options.pitch
				, m_dataInfo.height 
				);

			return 0;
		}

		if( setjmp( png_jmpbuf( m_png_ptr ) ) )  
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::decode" );

			return 0;
		}

		if( m_options.flags & DF_READ_ALPHA_ONLY )
		{
			if( m_dataInfo.channels == 1 && m_options.channels == 1 )
			{
				png_bytep bufferCursor = (png_bytep)_buffer;
				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, bufferCursor, nullptr );

					bufferCursor += m_options.pitch;
				}
			}
			else if( m_dataInfo.channels == 4 && m_options.channels == 1 )
			{
				CacheMemoryBuffer row_buffer(m_serviceProvider, m_row_bytes, "ImageDecoderPNG_4_1");
				png_byte * row_memory = row_buffer.getMemoryT<png_byte>();

				png_bytep bufferCursor = (png_bytep)_buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, row_memory, nullptr );

					size_t row_alpha = m_row_bytes / 4;
					for( size_t j = 0; j != row_alpha; ++j )
					{
						bufferCursor[j] = row_memory[j*4+3];
					}

					bufferCursor += m_options.pitch;
				}
			}
			else
			{
				LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::decode DF_READ_ALPHA_ONLY not support chanells %d - %d"
					, m_dataInfo.channels
					, m_options.channels
					);

				return 0;
			}
		}
		else if( m_options.flags & DF_WRITE_ALPHA_ONLY )
		{				
			if( m_dataInfo.channels == 1 && m_options.channels == 4 )
			{
				CacheMemoryBuffer row_buffer(m_serviceProvider, m_row_bytes, "ImageDecoderPNG_1_4");
				png_byte * row_memory = row_buffer.getMemoryT<png_byte>();

				png_bytep bufferCursor = (png_bytep)_buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, row_memory, nullptr );

					for( size_t j = 0; j != m_row_bytes; ++j )
					{
						bufferCursor[j*4+3] = row_memory[j];
					}

					bufferCursor += m_options.pitch;
				}
			}
			else if( m_dataInfo.channels == 4 && m_options.channels == 4 )
			{
				CacheMemoryBuffer row_buffer(m_serviceProvider, m_row_bytes, "ImageDecoderPNG_4_4");
				png_byte * row_memory = row_buffer.getMemoryT<png_byte>();

				png_bytep bufferCursor = (png_bytep)_buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, row_memory, nullptr );

					for( size_t j = 0; j != m_row_bytes; ++j )
					{
						bufferCursor[j*4+3] = row_memory[j*4+3];
					}

					bufferCursor += m_options.pitch;
				}
			}
			else
			{
				LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::decode DF_WRITE_ALPHA_ONLY not support chanells %d - %d"
					, m_dataInfo.channels
					, m_options.channels
					);

				return 0;
			}
		}
		else 
		{
			if( m_dataInfo.channels == 1 && m_options.channels == 4 )
			{
				png_bytep bufferCursor = (png_bytep)_buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, bufferCursor, nullptr );

					bufferCursor += m_options.pitch;
				}

				this->sweezleAlpha1( m_dataInfo.width, m_dataInfo.height, _buffer, m_options.pitch );
			}
			else if( m_dataInfo.channels == 3 && m_options.channels == 4 )
			{           
				png_bytep bufferCursor = (png_bytep)_buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, bufferCursor, nullptr );

					bufferCursor += m_options.pitch;
				}

				this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, _buffer, m_options.pitch );
			}
			else if( m_dataInfo.channels == m_options.channels )
			{
				png_bytep bufferCursor = (png_bytep)_buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, bufferCursor, nullptr );

					bufferCursor += m_options.pitch;
				}
			}
			else
			{
				LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::decode DEFAULT not support chanells %d - %d"
					, m_dataInfo.channels
					, m_options.channels
					);

				return 0;
			}
		}

		png_read_end( m_png_ptr, m_info_ptr );

		return _bufferSize;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderPNG::_rewind()
	{
		m_stream->seek( PNG_BYTES_TO_CHECK );

		png_destroy_read_struct( &m_png_ptr, &m_info_ptr, nullptr );

		png_const_charp png_ver = PNG_LIBPNG_VER_STRING;

		m_png_ptr = png_create_read_struct( png_ver, (png_voidp)this, &s_handlerError, &s_handlerWarning );

		if( m_png_ptr == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::_rewind Can't create read structure" 
				);

			return false;
		}

		m_info_ptr = png_create_info_struct( m_png_ptr );

		if( m_info_ptr == nullptr ) 
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::_rewind Can't create info structure" 
				);

			png_destroy_write_struct( &m_png_ptr, nullptr );

			return false;
		}

		if( setjmp( png_jmpbuf( m_png_ptr ) ) )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::_rewind" 
				);

			png_destroy_write_struct( &m_png_ptr, &m_info_ptr );

			return false;
		}

		png_set_read_fn( m_png_ptr, m_stream.get(), &s_readProc );

		png_set_sig_bytes( m_png_ptr, PNG_BYTES_TO_CHECK );

		png_read_info( m_png_ptr, m_info_ptr );

		png_uint_32 width = 0;
		png_uint_32 height = 0;
		int color_type = 0;
		int bit_depth = 0;
		int interlace_method = 0;

		png_get_IHDR( m_png_ptr, m_info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, nullptr, nullptr );

		if( interlace_method != PNG_INTERLACE_NONE )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::_rewind PNG is interlacing (Engine not support to read this png format)"
				);

			png_destroy_info_struct( m_png_ptr, &m_info_ptr );

			return false;
		}

		switch( color_type )
		{
		case PNG_COLOR_TYPE_RGB:
		case PNG_COLOR_TYPE_RGB_ALPHA:
#   ifndef MENGE_RENDER_TEXTURE_RGBA
			png_set_bgr(m_png_ptr);
#   endif
			break;

		case PNG_COLOR_TYPE_GRAY:

			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:

			png_set_strip_alpha(m_png_ptr);
			break;
		};

		return true;
	}
}
