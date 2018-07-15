#include "ImageDecoderJPEG.h"

#include "Kernel/Logger.h"

#define INPUT_BUF_SIZE  4096				// choose an efficiently fread'able size 

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	struct DecoderJPEGSourceManager
		: public jpeg_source_mgr
	{
		InputStreamInterface * stream;
		JOCTET * buffer;
		boolean start_of_file;
	};
	//////////////////////////////////////////////////////////////////////////
    static noreturn_t s_jpegErrorExit( j_common_ptr _cinfo )
	{
		{
            char buffer[JMSG_LENGTH_MAX] = { 0 };

			(*_cinfo->err->format_message)(_cinfo, buffer);

			LOGGER_ERROR("ImageDecoderJPEG::ErrorMessage %s"
				, buffer
				);
		}

		if( _cinfo->err->msg_parm.i[0] != 13 )
		{
			jpeg_destroy( _cinfo );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static noreturn_t s_jpegOutputMessage( j_common_ptr _cinfo )
	{
        char buffer[JMSG_LENGTH_MAX] = { 0 };

		(*_cinfo->err->format_message)(_cinfo, buffer);
				
		LOGGER_ERROR("ImageDecoderJPEG::OutputMessage %s"
			, buffer
			);
	}
	//////////////////////////////////////////////////////////////////////////
	METHODDEF(void)	s_init_source( j_decompress_ptr cinfo ) 
	{
		DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

		src->start_of_file = TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	METHODDEF(boolean) s_fill_input_buffer( j_decompress_ptr cinfo )
	{
		DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

		size_t nbytes = src->stream->read( src->buffer, INPUT_BUF_SIZE );

		if( nbytes <= 0 )
		{
			WARNMS(cinfo, JWRN_JPEG_EOF);

			src->buffer[0] = (JOCTET) 0xFF;
			src->buffer[1] = (JOCTET) JPEG_EOI;

			nbytes = 2;
		}

		src->next_input_byte = src->buffer;
		src->bytes_in_buffer = nbytes;
		src->start_of_file = FALSE;

		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	METHODDEF(void)	s_skip_input_data( j_decompress_ptr cinfo, long num_bytes )
	{
		DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

		if( num_bytes > 0 )
		{
			while( num_bytes > (long)src->bytes_in_buffer )
			{
				num_bytes -= (long) src->bytes_in_buffer;

				(void) s_fill_input_buffer(cinfo);
			}

			src->next_input_byte += (size_t) num_bytes;
			src->bytes_in_buffer -= (size_t) num_bytes;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	METHODDEF(void)	s_term_source( j_decompress_ptr cinfo )
	{
        (void)cinfo;		
	}
	//////////////////////////////////////////////////////////////////////////
	GLOBAL(void) s_jpeg_mengine_src( j_decompress_ptr cinfo, InputStreamInterface * _stream ) 
	{
		if( cinfo->src == nullptr )
		{
			DecoderJPEGSourceManager * manager = (DecoderJPEGSourceManager *)(*cinfo->mem->alloc_small)
				((j_common_ptr) cinfo, JPOOL_PERMANENT, SIZEOF(DecoderJPEGSourceManager));

			manager->buffer = (JOCTET *) (*cinfo->mem->alloc_small)
				((j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * SIZEOF(JOCTET));

			cinfo->src = (struct jpeg_source_mgr *)manager;
		}

		DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;
				
		src->init_source = s_init_source;
		src->fill_input_buffer = s_fill_input_buffer;
		src->skip_input_data = s_skip_input_data;
		src->resync_to_restart = jpeg_resync_to_restart; // use default method 
		src->term_source = s_term_source;
		src->stream = _stream;
		src->bytes_in_buffer = 0;		// forces fill_input_buffer on first read 
		src->next_input_byte = nullptr;	// until buffer loaded 
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_getQuality( const jpeg_decompress_struct * _jpegObject )
	{
        const JQUANT_TBL * tbl_ptrs = _jpegObject->quant_tbl_ptrs[0];

        if( tbl_ptrs == nullptr )
		{
			return 100;
		}

		UINT16 val1 = tbl_ptrs->quantval[0];
		UINT16 val2 = tbl_ptrs->quantval[1];

		val1 = ( val1 * 100 - 50 ) / 16;
		val2 = ( val2 * 100 - 50 ) / 11;

		if( val1 > 100 )
		{
			val1 = 5000 / val1;
		}
		else
		{
			val1 = (200 - val1)/2;
		}

		if( val2 > 100 )
		{
			val2 = 5000 / val2;
		}
		else
		{
			val2 = (200 - val2)/2;
		}

		return (val1 + val2)/2;
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderJPEG::ImageDecoderJPEG()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderJPEG::~ImageDecoderJPEG()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderJPEG::_initialize()
	{
		// step 1: allocate and initialize JPEG decompression object
		m_jpegObject.err = jpeg_std_error(&m_errorMgr);
		m_jpegObject.client_data = this;

		m_errorMgr.error_exit = &s_jpegErrorExit;
		m_errorMgr.output_message = &s_jpegOutputMessage;

		jpeg_create_decompress( &m_jpegObject );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderJPEG::_finalize()
	{
		jpeg_destroy_decompress( &m_jpegObject );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderJPEG::_prepareData()
	{
		// step 2a: specify data source (eg, a handle)
		s_jpeg_mengine_src( &m_jpegObject, m_stream.get() );

		// step 3: read handle parameters with jpeg_read_header()
		jpeg_read_header( &m_jpegObject, TRUE );

		jpeg_calc_output_dimensions( &m_jpegObject );

		m_dataInfo.depth = 1;
		m_dataInfo.mipmaps = 1;
		m_dataInfo.width = m_jpegObject.image_width;
		m_dataInfo.height = m_jpegObject.image_height;
		m_dataInfo.quality = s_getQuality( &m_jpegObject );

		//if( m_jpegObject.jpeg_color_space != JCS_RGB )
		//{
		//	const char * jcs_err[] =
		//	{
		//		"JCS_UNKNOWN",		/* error/unspecified */
		//		"JCS_GRAYSCALE",		/* monochrome */
		//		"JCS_RGB",		/* red/green/blue, standard RGB (sRGB) */
		//		"JCS_YCbCr",		/* Y/Cb/Cr (also known as YUV), standard YCC */
		//		"JCS_CMYK",		/* C/M/Y/K */
		//		"JCS_YCCK",		/* Y/Cb/Cr/K */
		//		"JCS_BG_RGB",		/* big gamut red/green/blue, bg- sRGB */
		//		"JCS_BG_YCC"		/* big gamut Y/Cb/Cr, bg-sYCC */
		//	};

		//	LOGGER_ERROR("ImageDecoderJPEG::_prepareData unsupport %s (%d) (only RGB)"
		//		, jcs_err[m_jpegObject.jpeg_color_space]
		//		, m_jpegObject.jpeg_color_space				
		//		);

		//	return false;
		//}
		//
		//if( m_jpegObject.num_components != 3 )
		//{
		//	LOGGER_ERROR("ImageDecoderJPEG::_prepareData invalid color components %d"
		//		, m_jpegObject.num_components
		//		);

		//	return false;
		//}

		m_dataInfo.channels = RGB_PIXELSIZE;
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderJPEG::_decode( void * _buffer, size_t _bufferSize )
	{
        if( _bufferSize < m_options.pitch * m_dataInfo.height )
        {
            LOGGER_ERROR("ImageDecoderJPEG::decode error, invalid buffer relation pitch to size bufferSize %d - pitch %d height %d"
                , _bufferSize
                , m_options.pitch 
                , m_dataInfo.height
                );

            return 0;
        }

		jpeg_start_decompress( &m_jpegObject );
		
        if( m_options.flags == 0 )
        {
            if( m_options.channels == m_dataInfo.channels )
            {
                JSAMPROW rgb_buffer = (JSAMPROW)_buffer;

                for( uint32_t j = 0; j != m_dataInfo.height; ++j )
                {
                    jpeg_read_scanlines( &m_jpegObject, &rgb_buffer, 1 );

                    // Assume put_scanline_someplace wants a pointer and sample count.
                    rgb_buffer += m_options.pitch;
                }

#if RGB_PIXELSIZE == 4
                if( (m_options.flags & DF_NOT_ADD_ALPHA) == 0)
                {
                    JSAMPROW alpha_buffer = (JSAMPROW)_buffer;
                    for( uint32_t j = 0; j != m_dataInfo.height; ++j )
                    {
                        for( uint32_t i = 0; i != m_dataInfo.width; ++i )
                        {
                            alpha_buffer[i * 4 + 3] = 255; // alpha
                        }

                        alpha_buffer += m_options.pitch;
                    }
                }
#endif
            }
            else
            {
                LOGGER_ERROR("ImageDecoderJPEG::decode DEFAULT m_options.channels %d != %d"
                    , m_options.channels
					, m_dataInfo.channels
                    );

                return 0;
            }
        }
		else if( m_options.flags & DF_READ_ALPHA_ONLY )
		{
			if( m_options.channels == 1 )
			{
				memset( _buffer, 255, _bufferSize );

				return _bufferSize;
			}
			else
			{
				LOGGER_ERROR("ImageDecoderJPEG::decode DF_READ_ALPHA_ONLY m_options.channels %d != 4"
					, m_options.channels
					);

				return 0;
			}			
		}

		m_jpegObject.output_scanline = m_jpegObject.output_height;
		jpeg_finish_decompress( &m_jpegObject );
		
        size_t readSize = m_options.pitch * m_dataInfo.height;
		
		return readSize;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderJPEG::_rewind()
	{
		jpeg_destroy_decompress( &m_jpegObject );

		m_jpegObject.err = jpeg_std_error(&m_errorMgr);
		m_jpegObject.client_data = this;

		m_errorMgr.error_exit = &s_jpegErrorExit;
		m_errorMgr.output_message = &s_jpegOutputMessage;

		jpeg_create_decompress( &m_jpegObject );

		if( m_stream->seek( 0 ) == false )
		{
			return false;
		}

		if( this->_prepareData() == false )
		{
			return false;
		}

		return true;
	}
}
