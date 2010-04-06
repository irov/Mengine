 /*
 *	ImageEncoderJPEG.cpp
 *
 *	Created by _Berserk_ on 16.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "ImageEncoderJPEG.h"
#	include "Utils/Core/PixelFormat.h"


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

#	define OUTPUT_BUF_SIZE 4096				// choose an efficiently fwrite'able size

namespace Menge
{
	typedef struct tagErrorManager 
	{
		/// "public" fields
		struct jpeg_error_mgr pub;
		/// for return to caller
		jmp_buf setjmp_buffer;
	} ErrorManager;

	typedef struct tagDestinationManager 
	{
		/// public fields
		struct jpeg_destination_mgr pub;

		FileOutputInterface * m_stream;
		/// start of buffer
		JOCTET * buffer;
	} DestinationManager;

	typedef DestinationManager*		menge_dst_ptr;
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
		//FreeImage_OutputMessageProc(s_format_id, buffer);
		MENGE_LOG_ERROR( buffer );
	}

	//Initialize destination.  This is called by jpeg_start_compress()
	//before any data is actually written. It must initialize
	//next_output_byte and free_in_buffer. free_in_buffer must be
	//initialized to a positive value.

	METHODDEF(void)
		init_destination (j_compress_ptr cinfo) 
	{
			menge_dst_ptr dest = (menge_dst_ptr) cinfo->dest;

			dest->buffer = (JOCTET *)
				(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

			dest->pub.next_output_byte = dest->buffer;
			dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
	}

	//This is called whenever the buffer has filled (free_in_buffer
	//reaches zero). In typical applications, it should write out the
	//*entire* buffer (use the saved start address and buffer length;
	//ignore the current state of next_output_byte and free_in_buffer).
	//Then reset the pointer & count to the start of the buffer, and
	//return TRUE indicating that the buffer has been dumped.
	//free_in_buffer must be set to a positive value when TRUE is
	//returned.  A FALSE return should only be used when I/O suspension is
	//desired.
	METHODDEF(boolean)
		empty_output_buffer (j_compress_ptr cinfo) 
	{
			menge_dst_ptr dest = (menge_dst_ptr) cinfo->dest;

			dest->m_stream->write( dest->buffer, OUTPUT_BUF_SIZE );

			dest->pub.next_output_byte = dest->buffer;
			dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

			return TRUE;
	}

	//Terminate destination --- called by jpeg_finish_compress() after all
	//data has been written.  In most applications, this must flush any
	//data remaining in the buffer.  Use either next_output_byte or
	//free_in_buffer to determine how much data is in the buffer.
	METHODDEF(void)
		term_destination (j_compress_ptr cinfo) 
	{
			menge_dst_ptr dest = (menge_dst_ptr) cinfo->dest;

			size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

			// write any data remaining in the buffer

			if (datacount > 0) 
			{
				dest->m_stream->write(dest->buffer, (unsigned int)datacount );
			}
	}


	//Prepare for output to a stdio stream.
	//The caller must have already opened the stream, and is responsible
	//for closing it after finishing compression.
	GLOBAL(void)
		jpeg_menge_dst (j_compress_ptr cinfo, FileOutputInterface* _stream ) 
	{
			menge_dst_ptr dest;

			if (cinfo->dest == NULL)
			{
				cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small)
					((j_common_ptr) cinfo, JPOOL_PERMANENT, SIZEOF(DestinationManager));
			}

			dest = (menge_dst_ptr) cinfo->dest;
			dest->pub.init_destination = init_destination;
			dest->pub.empty_output_buffer = empty_output_buffer;
			dest->pub.term_destination = term_destination;
			dest->m_stream = _stream;
	}


	//////////////////////////////////////////////////////////////////////////
	ImageEncoderJPEG::ImageEncoderJPEG()
		: m_errorMgr( NULL )
		, m_jpegObject( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderJPEG::~ImageEncoderJPEG()
	{
		if( m_jpegObject != NULL )
		{
			jpeg_destroy_compress( m_jpegObject );
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
	void ImageEncoderJPEG::_initialize()
	{
		if( m_stream != NULL )
		{
			m_valid = initializeEncoder_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ImageEncoderJPEG::encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo )
	{
		if( m_valid == false )
		{
			return 0;
		}

		const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( _bufferDataInfo );
		if( dataInfo->format != PF_R8G8B8 )
		{
			MENGE_LOG_ERROR( "JPEG encoder error: unsupported data format" );
			return 0;
		}
		m_jpegObject->image_width = (JDIMENSION)dataInfo->width;
		m_jpegObject->image_height = (JDIMENSION)dataInfo->height;
		m_jpegObject->input_components = 3;
		m_jpegObject->in_color_space = JCS_RGB;
		int pitch = dataInfo->width * PixelUtil::getNumElemBytes( dataInfo->format );
		int pixel_depth = 8;
		if( (m_options & DF_CUSTOM_PITCH) != 0 )
		{
			pitch = (m_options >> 16);
		}

		jpeg_set_defaults( m_jpegObject );
		jpeg_set_quality( m_jpegObject, dataInfo->quality, TRUE /* limit to baseline-JPEG values */);

		jpeg_start_compress( m_jpegObject, TRUE);

		JSAMPROW row_pointer[1];	// pointer to JSAMPLE row[s]
		while (m_jpegObject->next_scanline < m_jpegObject->image_height) 
		{
			// jpeg_write_scanlines expects an array of pointers to scanlines.
			// Here the array is only one element long, but you could pass
			// more than one scanline at a time if that's more convenient.
			row_pointer[0] = & _buffer[m_jpegObject->next_scanline * pitch];
			(void) jpeg_write_scanlines( m_jpegObject, row_pointer, 1);
		}

		// Step 6: Finish compression

		jpeg_finish_compress(m_jpegObject);


		return pitch * dataInfo->height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageEncoderJPEG::initializeEncoder_()
	{
		m_errorMgr = new tagErrorManager;
		m_jpegObject = new jpeg_compress_struct;

		// step 1: allocate and initialize JPEG compression object
		m_jpegObject->err = jpeg_std_error(&m_errorMgr->pub);

		m_errorMgr->pub.error_exit     = s_jpegErrorExit;
		m_errorMgr->pub.output_message = s_jpegOutputMessage;

		if( setjmp( m_errorMgr->setjmp_buffer ) ) 
		{
			// If we get here, the JPEG code has signaled an error.
			// We need to clean up the JPEG object and return.
			jpeg_destroy_compress( m_jpegObject );
			delete m_jpegObject;
			m_jpegObject = NULL;
			return false;
		}

		jpeg_create_compress( m_jpegObject );

		jpeg_menge_dst( m_jpegObject, m_stream );

		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
