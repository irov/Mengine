#	include "ImageDecoderPNG.h"

#	include "Logger/Logger.h"

#	include "Interface/FileSystemInterface.h"

#	define PNG_BYTES_TO_CHECK 8

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void PNGAPI s_errorHandler( png_structp png_ptr, const char * _error ) 
	{
		png_voidp error_ptr = png_get_error_ptr( png_ptr );		
		ImageDecoderPNG * imageDecoderPNG = static_cast<ImageDecoderPNG*>(error_ptr);
		ServiceProviderInterface * serviceProvider = imageDecoderPNG->getServiceProvider();

		LOGGER_ERROR(serviceProvider)( "ImageDecoderPNG error: %s"
			, _error 
			);

        // Return control to the setjmp point
        //jmp_buf * jb = png_jmpbuf( png_ptr );
        //longjmp(jb, 1);
        longjmp(png_jmpbuf(png_ptr), 1);

	}
	//////////////////////////////////////////////////////////////////////////
	static void PNGAPI s_readProc( png_structp _png_ptr, unsigned char* _data, png_size_t _size )
	{
		png_voidp io_ptr = png_get_io_ptr( _png_ptr );
		FileInputStreamInterface* stream = static_cast<FileInputStreamInterface*>( io_ptr );

		stream->read( _data, (std::streamsize)_size );
	}
	////////////////////////////////////////////////////////////////////////////
	//static void PNGAPI s_flushProc( png_structp png_ptr ) 
	//{
 //       (void)png_ptr;

	//	// empty flush implementation
	//}
	//////////////////////////////////////////////////////////////////////////
	static void PNGAPI s_cleanup( png_structp _png_ptr )
	{
		if( _png_ptr != NULL )
		{
			png_destroy_read_struct( &_png_ptr, 0, (png_infopp)0 );
			_png_ptr = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPNG::ImageDecoderPNG()
		: m_png_ptr(NULL)
		, m_row_bytes(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderPNG::~ImageDecoderPNG()
	{
		s_cleanup( m_png_ptr );
	}
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderPNG::_initialize()
    {
        // check for png signature
        unsigned char png_check[PNG_BYTES_TO_CHECK];
        m_stream->read( &png_check, PNG_BYTES_TO_CHECK );

        if( png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::initialize Bad or not PNG file" 
                );

            return false;
        }

        // create the chunk manage structure
        m_png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
            (png_voidp)this,
            s_errorHandler,
            s_errorHandler );

        if( m_png_ptr == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::initialize Can't create read structure" 
                );

            return false;
        }

        // create the info structure
        png_infop info_ptr = png_create_info_struct( m_png_ptr );

        if( info_ptr == 0 ) 
        {
            LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::initialize Can't create info structure" 
                );

            png_destroy_write_struct(&m_png_ptr, NULL);

            return false;
        }

        if (setjmp(png_jmpbuf(m_png_ptr)))  
        {
            LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::initialize" 
                );

            png_destroy_write_struct(&m_png_ptr, &info_ptr);           

            return false;
        }

        // init the IO
        png_set_read_fn( m_png_ptr, m_stream, s_readProc );

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

        png_set_interlace_handling( m_png_ptr );
        //if( interlace_method != PNG_INTERLACE_NONE )
        {
            //LOGGER_ERROR(m_serviceProvider)("ImageDecoderPNG::initialize PNG is interlacing (Engine not support to read this png format)"
            //    );



            //png_destroy_info_struct( m_png_ptr, &info_ptr );

            //png_read_image

            //return false;
        }

        switch( color_type )
        {
        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_RGB_ALPHA:
#   ifdef WIN32
            png_set_bgr(m_png_ptr);
#   endif

            break;

        case PNG_COLOR_TYPE_GRAY:

            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:

            png_set_strip_alpha(m_png_ptr);
            break;
        };

        // all transformations have been registered; now update info_ptr data
        png_read_update_info( m_png_ptr, info_ptr );

        //if( setjmp( png_jmpbuf( m_png_ptr ) ) ) 
        //{
        //    LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::initialize (png_read_update_info)" );

        //    png_destroy_info_struct( m_png_ptr, &info_ptr );

        //    return false;
        //}

        png_get_IHDR(m_png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);

        m_row_bytes = png_get_rowbytes(m_png_ptr, info_ptr);

        png_byte channels = png_get_channels(m_png_ptr, info_ptr);

        size_t decodedDataSize = m_row_bytes * height;

        m_dataInfo.width = width;
        m_dataInfo.height = height;
        m_dataInfo.depth = 1;
        m_dataInfo.size = decodedDataSize;
        m_dataInfo.num_mipmaps = 0;
        m_dataInfo.flags = 0;
        m_dataInfo.channels = channels;

        png_destroy_info_struct( m_png_ptr, &info_ptr );

        if( bit_depth != 8 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::initialize Can't support non 8 bit depth - '%d'"
                , bit_depth
                );

            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderPNG::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{	
        if( _bufferSize != m_options.pitch * m_dataInfo.height )
        {
            LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::decode invalid bufferSize %d != (%d * %d)"
                , _bufferSize
                , m_options.pitch
                , m_dataInfo.height 
                );

            return 0;
        }


        if (setjmp(png_jmpbuf(m_png_ptr)))  
        {
            LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::initialize" );

            //png_destroy_write_struct(&m_png_ptr, 0);
            //m_png_ptr = 0;

            return 0;
        }

        static TBlobject s_row_buffer;

		if( m_options.flags & DF_READ_ALPHA_ONLY )
		{
			if( m_dataInfo.channels == 1 && m_options.channels == 1 )
			{
                unsigned char * bufferCursor = _buffer;
				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, bufferCursor, NULL );

                    bufferCursor += m_options.pitch;
				}
			}
			else if( m_dataInfo.channels == 4 && m_options.channels == 1 )
			{
				s_row_buffer.resize( m_row_bytes );
                TBlobject::value_type * buff = &s_row_buffer[0];
                unsigned char * bufferCursor = _buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, buff, NULL );

					size_t row_alpha = m_row_bytes / 4;
					for( size_t j = 0; j < row_alpha; ++j )
					{
						bufferCursor[j] = buff[j*4+3];
					}

					bufferCursor += m_options.pitch;
				}
			}
            else
            {
                LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::decode DF_READ_ALPHA_ONLY not support chanells %d - %d"
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
                s_row_buffer.resize( m_row_bytes );
				TBlobject::value_type * buff = &s_row_buffer[0];
				unsigned char * bufferCursor = _buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, buff, NULL );

					for( size_t j = 0; j != m_row_bytes; ++j )
					{
						bufferCursor[j*4+3] = buff[j];
					}

					bufferCursor += m_options.pitch;
				}
			}
			else if( m_dataInfo.channels == 4 && m_options.channels == 4 )
			{
                s_row_buffer.resize( m_row_bytes );
                TBlobject::value_type * buff = &s_row_buffer[0];
                unsigned char * bufferCursor = _buffer;

				for( size_t i = 0; i != m_dataInfo.height; ++i )
				{
					png_read_row( m_png_ptr, buff, NULL );

					for( size_t j = 0; j != m_row_bytes; ++j )
					{
						bufferCursor[j*4+3] = buff[j*4+3];
					}

					bufferCursor += m_options.pitch;
				}
			}
            else
            {
                LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::decode DF_WRITE_ALPHA_ONLY not support chanells %d - %d"
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
                s_row_buffer.resize( m_row_bytes );
                TBlobject::value_type * buff = &s_row_buffer[0];
                unsigned char * bufferCursor = _buffer;

                for( size_t i = 0; i != m_dataInfo.height; ++i )
                {
                    png_read_row( m_png_ptr, buff, NULL );

                    for( size_t j = 0; j != m_row_bytes; ++j )
                    {
                        bufferCursor[j*4+0] = 255;
                        bufferCursor[j*4+1] = 255;
                        bufferCursor[j*4+2] = 255;
                        bufferCursor[j*4+3] = buff[j];
                    }

                    bufferCursor += m_options.pitch * 4;
                }
            }
    		else if( m_dataInfo.channels == 3 && m_options.channels == 4 )
			{                 
                s_row_buffer.resize( m_row_bytes );
                TBlobject::value_type * buff = &s_row_buffer[0];
                unsigned char * bufferCursor = _buffer;

                for( size_t i = 0; i != m_dataInfo.height; ++i )
                {
                    png_read_row( m_png_ptr, buff, NULL );

                    size_t row = m_row_bytes / 3;
                    for( size_t j = 0; j < row; ++j )
                    {
                        bufferCursor[j*4 + 0] = buff[j*3 + 0];
                        bufferCursor[j*4 + 1] = buff[j*3 + 1];
                        bufferCursor[j*4 + 2] = buff[j*3 + 2];
                        bufferCursor[j*4 + 3] = 255;
                    }

                    bufferCursor += m_options.pitch;
                }
            }
            else if( m_dataInfo.channels == m_options.channels )
            {
                unsigned char * bufferCursor = _buffer;
                for( size_t i = 0; i != m_dataInfo.height; ++i )
                {
                    png_read_row( m_png_ptr, bufferCursor, NULL );

                    bufferCursor += m_options.pitch;
                }
            }
            else
            {
                LOGGER_ERROR(m_serviceProvider)( "ImageDecoderPNG::decode DEFAULT not support chanells %d - %d"
                    , m_dataInfo.channels
                    , m_options.channels
                    );

                return 0;
            }
		}

		png_read_end(m_png_ptr, 0);

		png_destroy_read_struct(&m_png_ptr, 0, 0);

		return _bufferSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
