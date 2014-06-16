#	include "ImageDecoderJPEG.h"

#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	define INPUT_BUF_SIZE  4096				// choose an efficiently fread'able size 

namespace Menge
{
	typedef struct tagSourceManager {
		/// public fields
		struct jpeg_source_mgr pub;

		InputStreamInterface * stream;
		/// start of buffer
		JOCTET * buffer;
		/// have we gotten any data yet ?
		boolean start_of_file;
	} SourceManager;

	typedef SourceManager *	menge_src_ptr;
    typedef DecoderJPEGErrorManager * menge_error_ptr;
	//////////////////////////////////////////////////////////////////////////
	static void	s_jpegErrorExit( j_common_ptr _cinfo ) 
	{
		menge_error_ptr mErr = (menge_error_ptr) _cinfo->err;
		// always display the message
		(mErr->pub.output_message)( _cinfo );

		// allow JPEG with a premature end of file
		if( mErr->pub.msg_parm.i[0] != 13 )
		{
			// let the memory manager delete any temp files before we die
			jpeg_destroy( _cinfo );

			// Return control to the setjmp point
			longjmp( mErr->setjmp_buffer, 1 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// Actual output of any JPEG message.  Note that this method does not know
	// how to generate a message, only where to send it.
	static void	s_jpegOutputMessage( j_common_ptr _cinfo ) 
	{
		char buffer[JMSG_LENGTH_MAX];

		// create the message
		(*_cinfo->err->format_message)(_cinfo, buffer);
		// send it to user's message proc
		
		ImageDecoderJPEG * imageDecoderJPEG = static_cast<ImageDecoderJPEG*>(_cinfo->client_data);
		
		ServiceProviderInterface * serviceProvider = imageDecoderJPEG->getServiceProvider();

		LOGGER_ERROR(serviceProvider)("ImageDecoderJPEG::OutputMessage %s"
			, buffer
			);
	}

	// ----------------------------------------------------------
	//   Source manager
	// ----------------------------------------------------------

	// Initialize source.  This is called by jpeg_read_header() before any
	// data is actually read. Unlike init_destination(), it may leave
	// bytes_in_buffer set to 0 (in which case a fill_input_buffer() call
	// will occur immediately).

	METHODDEF(void)
		init_source (j_decompress_ptr cinfo) 
	{
		menge_src_ptr src = (menge_src_ptr) cinfo->src;

		// We reset the empty-input-file flag for each image,
		// but we don't clear the input buffer.
		// This is correct behavior for reading a series of images from one source.

		src->start_of_file = TRUE;
	}

	//This is called whenever bytes_in_buffer has reached zero and more
	//data is wanted.  In typical applications, it should read fresh data
	//into the buffer (ignoring the current state of next_input_byte and
	//bytes_in_buffer), reset the pointer & count to the start of the
	//buffer, and return TRUE indicating that the buffer has been reloaded.
	//It is not necessary to fill the buffer entirely, only to obtain at
	//least one more byte.  bytes_in_buffer MUST be set to a positive value
	//if TRUE is returned.  A FALSE return should only be used when I/O
	//suspension is desired.

	METHODDEF(boolean)
		fill_input_buffer (j_decompress_ptr cinfo)
	{
		menge_src_ptr src = (menge_src_ptr) cinfo->src;

		size_t nbytes = src->stream->read( src->buffer, INPUT_BUF_SIZE );

		if( nbytes <= 0 )
		{
			//if (src->start_of_file)	/* Treat empty input file as fatal error */
			//	throw(cinfo, JERR_INPUT_EMPTY);

			WARNMS(cinfo, JWRN_JPEG_EOF);

			// Insert a fake EOI marker
			src->buffer[0] = (JOCTET) 0xFF;
			src->buffer[1] = (JOCTET) JPEG_EOI;

			nbytes = 2;
		}

		src->pub.next_input_byte = src->buffer;
		src->pub.bytes_in_buffer = nbytes;
		src->start_of_file = FALSE;

		return TRUE;
	}

	//Skip num_bytes worth of data.  The buffer pointer and count should
	//be advanced over num_bytes input bytes, refilling the buffer as
	//needed. This is used to skip over a potentially large amount of
	//uninteresting data (such as an APPn marker). In some applications
	//it may be possible to optimize away the reading of the skipped data,
	//but it's not clear that being smart is worth much trouble; large
	//skips are uncommon.  bytes_in_buffer may be zero on return.
	//A zero or negative skip count should be treated as a no-op.

	METHODDEF(void)
		skip_input_data( j_decompress_ptr cinfo, long num_bytes )
	{
		menge_src_ptr src = (menge_src_ptr) cinfo->src;

		//Just a dumb implementation for now.  Could use fseek() except
		//it doesn't work on pipes.  Not clear that being smart is worth
		//any trouble anyway --- large skips are infrequent.

		if (num_bytes > 0)
		{
			while (num_bytes > (long) src->pub.bytes_in_buffer) 
			{
				num_bytes -= (long) src->pub.bytes_in_buffer;

				(void) fill_input_buffer(cinfo);

				// note we assume that fill_input_buffer will never return FALSE,
				// so suspension need not be handled.

			}

			src->pub.next_input_byte += (size_t) num_bytes;
			src->pub.bytes_in_buffer -= (size_t) num_bytes;
		}
	}

	//Terminate source --- called by jpeg_finish_decompress
	//after all data has been read.  Often a no-op.

	//NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
	//application must deal with any cleanup that should happen even
	//for error exit.

	METHODDEF(void)
		term_source (j_decompress_ptr cinfo)
	{
        (void)cinfo;
		// no work necessary here
	}

	// ----------------------------------------------------------
	//   Source manager & Destination manager setup
	// ----------------------------------------------------------

	/**
	Prepare for input from a stdio stream.
	The caller must have already opened the stream, and is responsible
	for closing it after finishing decompression.
	*/
	GLOBAL(void)
		jpeg_menge_src ( j_decompress_ptr cinfo, InputStreamInterface * _stream ) 
	{
		menge_src_ptr src;

		// allocate memory for the buffer. is released automatically in the end

		if( cinfo->src == nullptr )
		{
			cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
				((j_common_ptr) cinfo, JPOOL_PERMANENT, SIZEOF(SourceManager));

			src = (menge_src_ptr) cinfo->src;

			src->buffer = (JOCTET *) (*cinfo->mem->alloc_small)
				((j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * SIZEOF(JOCTET));
		}

		// initialize the jpeg pointer struct with pointers to functions
		src = (menge_src_ptr) cinfo->src;
		src->pub.init_source = init_source;
		src->pub.fill_input_buffer = fill_input_buffer;
		src->pub.skip_input_data = skip_input_data;
		src->pub.resync_to_restart = jpeg_resync_to_restart; // use default method 
		src->pub.term_source = term_source;
		src->stream = _stream;
		src->pub.bytes_in_buffer = 0;		// forces fill_input_buffer on first read 
		src->pub.next_input_byte = nullptr;	// until buffer loaded 
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_getQuality( jpeg_decompress_struct * _jpegObject )
	{
		if( _jpegObject->quant_tbl_ptrs[0] == nullptr )
		{
			return 100;
		}

		UINT16 val1 = _jpegObject->quant_tbl_ptrs[0]->quantval[0];
		UINT16 val2 = _jpegObject->quant_tbl_ptrs[0]->quantval[1];

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
		: m_initialize(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderJPEG::~ImageDecoderJPEG()
	{
		if( m_initialize == true )
		{
			jpeg_destroy_decompress( &m_jpegObject );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderJPEG::_initialize()
	{
		// step 1: allocate and initialize JPEG decompression object
		m_jpegObject.err = jpeg_std_error(&m_errorMgr.pub);
		m_jpegObject.client_data = this;

		m_errorMgr.pub.error_exit = s_jpegErrorExit;
		m_errorMgr.pub.output_message = s_jpegOutputMessage;

		jpeg_create_decompress( &m_jpegObject );

		return true;			 
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderJPEG::_prepareData()
	{
		if( setjmp( m_errorMgr.setjmp_buffer ) ) 
		{
			// If we get here, the JPEG code has signaled an error.
			// We need to clean up the JPEG object and return.
			jpeg_destroy_decompress( &m_jpegObject );

			return false;
		}

		// step 2a: specify data source (eg, a handle)
		jpeg_menge_src( &m_jpegObject, m_stream.get() );

		// step 3: read handle parameters with jpeg_read_header()
		jpeg_read_header( &m_jpegObject, TRUE );

		jpeg_calc_output_dimensions( &m_jpegObject );

		m_dataInfo.depth = 1;
		m_dataInfo.mipmaps = 0;
		m_dataInfo.width = m_jpegObject.image_width;
		m_dataInfo.height = m_jpegObject.image_height;
		m_dataInfo.quality = s_getQuality( &m_jpegObject );
		
		m_dataInfo.channels = m_jpegObject.out_color_components;
		m_dataInfo.size = m_dataInfo.width * m_dataInfo.height * m_dataInfo.channels;

		m_initialize = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderJPEG::decode( void * _buffer, size_t _bufferSize )
	{
        if( _bufferSize != m_options.pitch * m_dataInfo.height )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderJPEG::decode error, invalid buffer relation pitch to size bufferSize %d - pitch %d height %d"
                , _bufferSize
                , m_options.pitch 
                , m_dataInfo.height
                );

            return 0;
        }

		if( setjmp( m_errorMgr.setjmp_buffer ) ) 
		{
			// If we get here, the JPEG code has signaled an error.
			// We need to clean up the JPEG object and return.

			return false;
		}

		jpeg_start_decompress( &m_jpegObject );

		if( m_options.flags & DF_READ_ALPHA_ONLY )
		{
            if( m_options.channels == 1 )
            {
			    memset( _buffer, 255, _bufferSize );

                return _bufferSize;
            }
            else
            {
                LOGGER_ERROR(m_serviceProvider)("ImageDecoderJPEG::decode DF_READ_ALPHA_ONLY m_options.channels %d != 4"
                    , m_options.channels
                    );

                return 0;
            }			
		}
        else
        {
            if( m_options.channels == 4 || m_options.channels == 3 )
            {
                JSAMPROW rgb_buffer = (JSAMPROW)_buffer;

                for( size_t j = 0; j != m_dataInfo.height; ++j )
                {
                    jpeg_read_scanlines( &m_jpegObject, &rgb_buffer, 1 );

                    // Assume put_scanline_someplace wants a pointer and sample count.
                    rgb_buffer += m_options.pitch;
                }

                if( m_options.channels == 4 && (m_options.flags & DF_NOT_ADD_ALPHA) == 0)
                {
                    JSAMPROW alpha_buffer = (JSAMPROW)_buffer;
                    for( size_t j = 0; j != m_dataInfo.height; ++j )
                    {
                        for( size_t i = 0; i != m_dataInfo.width; ++i )
                        {
                            alpha_buffer[i * 4 + 3] = 255; // alpha
                        }

                        alpha_buffer += m_options.pitch;
                    }
                }
            }
            else
            {
                LOGGER_ERROR(m_serviceProvider)("ImageDecoderJPEG::decode DEFAULT m_options.channels %d != 4"
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
}	// namespace Menge
