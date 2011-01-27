/*
 *	ImageDecoderJPEG.cpp
 *
 *	Created by _Berserk_ on 22.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "ImageDecoderJPEG.h"

extern "C" 
{
	#	define XMD_H
	#	undef FAR

	#	include <setjmp.h>
	#	include "libJPEG/jinclude.h"
	#	include "libJPEG/jpeglib.h"
	#	include "libJPEG/jerror.h"
}

#	include "Interface/FileSystemInterface.h"
#	include "Utils/Logger/Logger.h"

#	define INPUT_BUF_SIZE  4096				// choose an efficiently fread'able size 

namespace Menge
{
	typedef struct tagErrorManager {
		/// "public" fields
		struct jpeg_error_mgr pub;
		/// for return to caller
		jmp_buf setjmp_buffer;
	} ErrorManager;

	typedef struct tagSourceManager {
		/// public fields
		struct jpeg_source_mgr pub;

		InputStreamInterface * m_stream;
		/// start of buffer
		JOCTET * buffer;
		/// have we gotten any data yet ?
		boolean start_of_file;
	} SourceManager;

	typedef SourceManager*		menge_src_ptr;
	typedef ErrorManager*		menge_error_ptr;

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
		MENGE_LOG_ERROR( buffer );
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

		size_t nbytes = src->m_stream->read( src->buffer, INPUT_BUF_SIZE );

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

		if (cinfo->src == NULL) 
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
		src->m_stream = _stream;
		src->pub.bytes_in_buffer = 0;		// forces fill_input_buffer on first read 
		src->pub.next_input_byte = NULL;	// until buffer loaded 
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderJPEG::ImageDecoderJPEG( CodecServiceInterface * _service, InputStreamInterface * _stream )
		: ImageDecoder(_service, _stream)
		, m_jpegObject(NULL)
		, m_rowStride(0)
		, m_bufferRowStride(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderJPEG::~ImageDecoderJPEG()
	{
		if( m_jpegObject != NULL )
		{
			// skip all rows
			m_jpegObject->output_scanline = m_jpegObject->output_height;
			jpeg_finish_decompress( m_jpegObject );
			jpeg_destroy_decompress( m_jpegObject );
			delete m_jpegObject;
			m_jpegObject = NULL;
		}

		if( m_errorMgr != NULL )
		{
			delete m_errorMgr;
			m_errorMgr = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderJPEG::initialize()
	{
		m_errorMgr = new tagErrorManager;
		m_jpegObject = new jpeg_decompress_struct;

		// step 1: allocate and initialize JPEG decompression object
		m_jpegObject->err = jpeg_std_error(&m_errorMgr->pub);

		m_errorMgr->pub.error_exit     = s_jpegErrorExit;
		m_errorMgr->pub.output_message = s_jpegOutputMessage;

		if( setjmp( m_errorMgr->setjmp_buffer ) ) 
		{
			// If we get here, the JPEG code has signaled an error.
			// We need to clean up the JPEG object and return.
			jpeg_destroy_decompress( m_jpegObject );
			delete m_jpegObject;
			m_jpegObject = NULL;
			return false;
		}

		jpeg_create_decompress( m_jpegObject );

		// step 2a: specify data source (eg, a handle)
		jpeg_menge_src( m_jpegObject, m_stream );

		// step 3: read handle parameters with jpeg_read_header()
		jpeg_read_header( m_jpegObject, TRUE );

		m_dataInfo.depth = 1;
		m_dataInfo.num_mipmaps = 0;
		m_dataInfo.width = m_jpegObject->image_width;
		m_dataInfo.height = m_jpegObject->image_height;
		m_dataInfo.format = PF_UNKNOWN;
		m_dataInfo.quality = getQuality( m_jpegObject );
		int numComponents = m_jpegObject->num_components;
		if( numComponents == 3 )
		{
			if( ( m_options.flags & DF_COUNT_ALPHA ) != 0 )
			{
				m_dataInfo.format = PF_A8R8G8B8;
			}
			else
			{
				m_dataInfo.format = PF_R8G8B8;
			}
		}
		m_dataInfo.size = m_dataInfo.width * m_dataInfo.height * numComponents;
		m_rowStride = m_bufferRowStride = m_dataInfo.width * numComponents;

		jpeg_start_decompress( m_jpegObject );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageDecoderJPEG::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		if( (m_bufferRowStride < m_rowStride) || ((_bufferSize % m_bufferRowStride) != 0) )
		{
			MENGE_LOG_ERROR( "ImageDecoderJPEG::decode error, invalid buffer pitch or size" );
			return 0;
		}

		int read = 0;
		//MENGE_LOG_INFO( "ImageDecoderJPEG::decode 1" );

		while( (m_jpegObject->output_scanline < m_jpegObject->output_height) && (_bufferSize >= m_bufferRowStride) ) 
		{
			jpeg_read_scanlines( m_jpegObject, &_buffer, 1 );
			read++;
			
			if( (m_options.flags & DF_COUNT_ALPHA) != 0 )	// place data as there is alpha
			{
				// place a little magic here =)
				std::size_t bufferDataWidth = m_dataInfo.width * 4;
				for( std::size_t i = 0; i < m_dataInfo.width; i++ )
				{
					std::copy( _buffer + 3 * ( m_dataInfo.width - i - 1 ), _buffer + 3 * ( m_dataInfo.width - i ), _buffer + bufferDataWidth - 4 - i*4 );
					_buffer[bufferDataWidth-i*4-1] = 255; // alpha
					
				}
			}

			// Assume put_scanline_someplace wants a pointer and sample count.
			_buffer += m_bufferRowStride;
			_bufferSize -= m_bufferRowStride;
		}
		//MENGE_LOG_INFO( "ImageDecoderJPEG::decode 2" );

		return read * m_rowStride;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageDecoderJPEG::_invalidate()
	{
		if( ( m_options.flags & DF_CUSTOM_PITCH ) != 0 )
		{
			m_bufferRowStride = m_options.pitch;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int ImageDecoderJPEG::getQuality( jpeg_decompress_struct* _jpegObject )
	{
		if( _jpegObject->quant_tbl_ptrs[0] == NULL )
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
}	// namespace Menge
