
#	include "ImageCodecJPEG.h"

extern "C" 
{
	#	define XMD_H
	#	undef FAR

	#	include <setjmp.h>
	#	include "LibJPEG/jinclude.h"
	#	include "LibJPEG/jpeglib.h"
	#	include "LibJPEG/jerror.h"
}

#	include "LogEngine.h"



#	define INPUT_BUF_SIZE  4096				// choose an efficiently fread'able size 
#	define OUTPUT_BUF_SIZE 4096				// choose an efficiently fwrite'able size

#	define EXIF_MARKER		(JPEG_APP0+1)	// EXIF marker / Adobe XMP marker
#	define ICC_MARKER		(JPEG_APP0+2)	// ICC profile marker
#	define IPTC_MARKER		(JPEG_APP0+13)	// IPTC marker / BIM marker 

#	define ICC_HEADER_SIZE 14				// size of non-profile data in APP2
#	define MAX_BYTES_IN_MARKER 65533L		// maximum data length of a JPEG marker
#	define MAX_DATA_BYTES_IN_MARKER 65519L	// maximum data length of a JPEG APP2 marker

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

		DataStreamInterface* m_stream;
		/// start of buffer
		JOCTET * buffer;
		/// have we gotten any data yet ?
		boolean start_of_file;
	} SourceManager;

	typedef struct tagDestinationManager {
		/// public fields
		struct jpeg_destination_mgr pub;

		OutStreamInterface* m_stream;
		/// start of buffer
		JOCTET * buffer;
	} DestinationManager;

	typedef SourceManager*		menge_src_ptr;
	typedef DestinationManager* menge_dst_ptr;
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
		MENGE_LOG( buffer );
	}

	// ----------------------------------------------------------
	//   Destination manager
	// ----------------------------------------------------------

	// Initialize destination.  This is called by jpeg_start_compress()
	// before any data is actually written. It must initialize
	// next_output_byte and free_in_buffer. free_in_buffer must be
	// initialized to a positive value.
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

	// This is called whenever the buffer has filled (free_in_buffer
	// reaches zero). In typical applications, it should write out the
	// *entire* buffer (use the saved start address and buffer length;
	// ignore the current state of next_output_byte and free_in_buffer).
	// Then reset the pointer & count to the start of the buffer, and
	// return TRUE indicating that the buffer has been dumped.
	// free_in_buffer must be set to a positive value when TRUE is
	// returned.  A FALSE return should only be used when I/O suspension is
	// desired.
	METHODDEF(boolean)
		empty_output_buffer (j_compress_ptr cinfo) 
	{
			menge_dst_ptr dest = (menge_dst_ptr) cinfo->dest;

			dest->m_stream->write( (char*)( dest->buffer ), OUTPUT_BUF_SIZE );

			dest->pub.next_output_byte = dest->buffer;
			dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

			return TRUE;
	}

	// Terminate destination --- called by jpeg_finish_compress() after all
	// data has been written.  In most applications, this must flush any
	// data remaining in the buffer.  Use either next_output_byte or
	// free_in_buffer to determine how much data is in the buffer.

	METHODDEF(void)
		term_destination (j_compress_ptr cinfo) 
	{
			menge_dst_ptr dest = (menge_dst_ptr) cinfo->dest;

			size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

			// write any data remaining in the buffer

			if (datacount > 0) 
			{
				dest->m_stream->write( (char*)dest->buffer, datacount );
			}
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
		jpeg_menge_src ( j_decompress_ptr cinfo, DataStreamInterface* _stream ) 
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

	//Prepare for output to a stdio stream.
	//The caller must have already opened the stream, and is responsible
	//for closing it after finishing compression.

	GLOBAL(void)
		jpeg_freeimage_dst ( j_compress_ptr cinfo, OutStreamInterface* _stream ) 
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
	ImageCodecJPEG::ImageCodecJPEG(const String & _type)
		: m_type(_type)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	ImageCodecJPEG::~ImageCodecJPEG()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecJPEG::getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const
	{
		ImageCodec::ImageData* imageData = static_cast<ImageCodec::ImageData*>( _codecData );
		// set up the jpeglib structures
		struct jpeg_decompress_struct cinfo;
		struct tagErrorManager mErrMgr;
		
		// step 1: allocate and initialize JPEG decompression object
		cinfo.err = jpeg_std_error(&mErrMgr.pub);

		mErrMgr.pub.error_exit     = s_jpegErrorExit;
		mErrMgr.pub.output_message = s_jpegOutputMessage;

		if( setjmp( mErrMgr.setjmp_buffer ) ) 
		{
			// If we get here, the JPEG code has signaled an error.
			// We need to clean up the JPEG object and return.
			jpeg_destroy_decompress(&cinfo);
			return false;
		}

		jpeg_create_decompress(&cinfo);

		// step 2a: specify data source (eg, a handle)
		jpeg_menge_src( &cinfo, _inputData );


		// step 3: read handle parameters with jpeg_read_header()
		jpeg_read_header( &cinfo, TRUE );

		imageData->depth = 1;
		imageData->num_mipmaps = 0;
		imageData->width = cinfo.image_width;
		imageData->height = cinfo.image_height;
		imageData->format = PF_UNKNOWN;
		int numComponents = cinfo.num_components;
		if( numComponents == 3 )
		{
			if( ( imageData->flags & DF_COUNT_ALPHA ) != 0 )
			{
				imageData->format = PF_A8R8G8B8;
				numComponents = 4;
			}
			else
			{
				imageData->format = PF_R8G8B8;
			}
		}
		imageData->size = imageData->width * imageData->height * numComponents;
		
		jpeg_destroy_decompress( &cinfo );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecJPEG::decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const
	{
		unsigned char* readBuffer = _buffer;
		// set up the jpeglib structures
		struct jpeg_decompress_struct cinfo;
		struct tagErrorManager mErrMgr;

		// step 1: allocate and initialize JPEG decompression object
		cinfo.err = jpeg_std_error(&mErrMgr.pub);

		mErrMgr.pub.error_exit     = s_jpegErrorExit;
		mErrMgr.pub.output_message = s_jpegOutputMessage;

		if( setjmp( mErrMgr.setjmp_buffer ) ) 
		{
			// If we get here, the JPEG code has signaled an error.
			// We need to clean up the JPEG object and return.
			jpeg_destroy_decompress(&cinfo);
			return false;
		}

		jpeg_create_decompress(&cinfo);

		// step 2a: specify data source (eg, a handle)
		jpeg_menge_src( &cinfo, _input );

		// step 3: read handle parameters with jpeg_read_header()
		jpeg_read_header( &cinfo, TRUE );

		int numComponents = cinfo.num_components;

		if( ( numComponents == 3 ) && (( _options & DF_COUNT_ALPHA ) != 0) )
		{
			numComponents = 4;
		}

		jpeg_start_decompress(&cinfo);

		int row_stride = cinfo.output_width * numComponents;

		if( numComponents < 4 )
		{
			while( cinfo.output_scanline < cinfo.output_height ) 
			{
				jpeg_read_scanlines( &cinfo, &readBuffer, 1 );
				/* Assume put_scanline_someplace wants a pointer and sample count. */
				readBuffer += row_stride;
				//put_scanline_someplace(_buffer[0], row_stride);
			}
		}
		else
		{
			std::size_t bufferSize = cinfo.output_width * cinfo.num_components;
			unsigned char* rowBuffer = new unsigned char[bufferSize];
			while( cinfo.output_scanline < cinfo.output_height )
			{
				jpeg_read_scanlines( &cinfo, &rowBuffer, 1 );
				for( std::size_t i = 0, j = 0; i < bufferSize; i += cinfo.num_components, j += numComponents )
				{
					std::copy( &(rowBuffer[i]), &(rowBuffer[i+cinfo.num_components]), &(readBuffer[j]) );
				}
				readBuffer += row_stride;
			}
			delete[] rowBuffer;
		}

		jpeg_finish_decompress(&cinfo);

		jpeg_destroy_decompress( &cinfo );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecJPEG::code( OutStreamInterface* _input, unsigned char* _buffer, CodecData* _data ) const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ImageCodecJPEG::getType() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge