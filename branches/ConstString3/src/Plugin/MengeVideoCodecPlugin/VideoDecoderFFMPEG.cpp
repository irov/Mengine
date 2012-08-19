#	include "Utils/Logger/Logger.h"
#	include "VideoDecoderFFMPEG.h"
#	include "Utils/Core/File.h"
#	include <limits.h>
#	include <math.h>
//#include <windows.h>
//#define MYRGB(r, g ,b)  ((int) (((int) (r) | ((int) (g) << 8)) | (((int) (int) (b)) << 16)))
/*#define COLOR_ARGB(a,r,g,b) \
	((int)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
*/

namespace Menge
{
	//FILE * Pf;
	
	//custom avcodec IO callback Read
	int ReadIOWrapper(void * _opaque, uint8_t *_bufer, int bufferSize)
	{
		InputStreamInterface * stream = (InputStreamInterface *) _opaque;
		int res = stream->read( _bufer , bufferSize );
		return res;
		//int res = fread( _bufer ,1 ,bufferSize, Pf );
		//return res;
	}
	
	//custom avcodec IO callback Seek
	int64_t SeekIOWrapper(void *_opaque, int64_t _offset, int _whence)
	{
		InputStreamInterface * stream = (InputStreamInterface *) _opaque;
		
		if( _whence == AVSEEK_SIZE )
		{
			int64_t size = stream->size();
			return size;
		}
		
		int offset;
		
		if( _offset < 0 )
		{
			int pos = stream->tell();
			offset = pos + _offset;
		}
		else
		{
			offset = _offset;
		}
		
		stream->seek( offset );
		//fseek(Pf, _offset , SEEK_SET);
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderFFMPEG::VideoDecoderFFMPEG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogServiceInterface * _logService )
		: VideoDecoder(_service, _stream)
		, m_logService(_logService)
		, m_formatContext(NULL)
		, m_codecContext(NULL)
		, m_codec(NULL)
		, m_Frame(NULL)
		, m_FrameRGBA(NULL)
		, m_IOContext(NULL)
		, m_inputFormat(NULL)
		, m_outputPixelFormat(PIX_FMT_RGBA)
		, m_bufferIO(NULL)
		, m_probeSize(4096)
		, m_videoStreamId(-1)
		//, m_timing(0)
		, m_pts(0.0f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderFFMPEG::~VideoDecoderFFMPEG()
	{
		clear_();
	}
	//////////////////////////////////////////////////////////////////////////
	const VideoCodecDataInfo* VideoDecoderFFMPEG::getCodecDataInfo() const
	{
		return &m_dataInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::initialize()
	{
		//check the input format
		if( m_probeSize > m_stream->size() )
		{
			m_probeSize = m_stream->size();
		}

		uint8_t * filebuffer = new uint8_t[ m_probeSize ];
		
		//read portion of data
		m_stream->read( filebuffer, m_probeSize );
		m_stream->seek( SEEK_SET );
		AVProbeData probeData; 
		m_bufferIO = new uint8_t[ m_probeSize + FF_INPUT_BUFFER_PADDING_SIZE ];
		memset( m_bufferIO, 0, m_probeSize + FF_INPUT_BUFFER_PADDING_SIZE );

		m_IOContext = avio_alloc_context(
			 m_bufferIO //IO buffer
			, m_probeSize //size of IO buffer
			, 0 //write flag set to 0
			, m_stream //IO source - it will be send as opaque argument to IO callbacks
			, ReadIOWrapper //read callback 
			, NULL //write callback
			, SeekIOWrapper //seek callback
			);
		
		//m_IOContext.is_streamed = 0;

		probeData.filename = "";
		probeData.buf= (unsigned char *) filebuffer; 
		probeData.buf_size = m_probeSize;
		
		m_inputFormat = av_probe_input_format(&probeData, 1);
		
		if( m_inputFormat == NULL )
		{
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: av_probe_input_format failed ");
			delete [] filebuffer;
			return false;
		}

		delete [] filebuffer;
		
		m_formatContext = avformat_alloc_context();
		
		m_formatContext->pb = m_IOContext;
		if( avformat_open_input( &m_formatContext, "", m_inputFormat ,NULL ) < 0 )
		{
			LOGGER_ERROR(m_logService)( "VideoDecoderFFMPEG:: Couldn't open stream " );
			return false;
		}
		
		
		if( avformat_find_stream_info( m_formatContext, NULL ) < 0 )
		{
			LOGGER_ERROR(m_logService)( "VideoDecoderFFMPEG:: Couldn't find stream information " );
			return false; 
		}
		//av_dump_format(m_formatContext, 0, filename, 0);
		
		// Find the first video stream
		m_videoStreamId = -1;
		for( size_t i = 0; i < m_formatContext->nb_streams; i++ )
		{
			if( m_formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO )
			{
				m_videoStreamId = i;
				break;
			}
		}	
		
		if( m_videoStreamId == -1 )
		{
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: Didn't find a video stream ");
			return false; // Didn't find a video stream
		}
		// Get a pointer to the codec context for the video stream
		m_codecContext = m_formatContext->streams[m_videoStreamId]->codec;

		// Find the decoder for the video stream
		m_codec = avcodec_find_decoder(m_codecContext->codec_id);
		if( m_codec == NULL )
		{
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: Unsupported codec! ");
			return false; // Codec not found
		}
			
		// Open codec
		if( avcodec_open2(m_codecContext, m_codec,NULL) < 0 )
		{
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: Could not open codec! ");
			return false; //
		}
		
		// Allocate video frame
		m_Frame = avcodec_alloc_frame();
		if( m_Frame == NULL )
		{
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: can not allocate  video frame");
			//error
			return false;
		}

		// Allocate an AVFrame structure
		m_FrameRGBA = avcodec_alloc_frame();
		if( m_FrameRGBA == NULL )
		{
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: can not allocate  video frame");
			//error
			return false;
		}
	
		// Hack to correct wrong frame rates that seem to be generated by some codecs
		if( m_codecContext->time_base.num > 1000 && m_codecContext->time_base.den == 1 )
		{
		 	m_codecContext->time_base.den = 1000;
		}
		
		////we must read frame because  m_codecContext->width,  m_codecContext->height can change 
		////init VideoCodec info
		//int isGotPicture;
		//AVPacket packet;

		//av_init_packet(&packet);
		//av_read_frame(m_formatContext, &packet);
		//avcodec_decode_video2( m_codecContext ,m_Frame, &isGotPicture, &packet );
		//av_free_packet(&packet);

		m_dataInfo.frameHeight = m_codecContext->height;
		m_dataInfo.frameWidth = m_codecContext->width;
		m_dataInfo.fps = av_q2d( m_formatContext->streams[m_videoStreamId]->r_frame_rate );

		if( int(m_dataInfo.fps) == 0 )
		{
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: invalid Frame rate ");
			return false; 
		}
		
		if ( this->eof() == true )
		{
			seek( 0.f );
		}

		int64_t len = m_formatContext->duration - m_formatContext->start_time;
		//m_dataInfo.duration  = (float)( len * (m_dataInfo.fps / AV_TIME_BASE) ) ;
		m_dataInfo.frameTiming = 1000.f / m_dataInfo.fps;
		m_dataInfo.duration = (len / AV_TIME_BASE) * 1000.0f;
		
		return true;
	}
	////////////////////////////////////////////////////////////////////////// 
	unsigned int VideoDecoderFFMPEG::decode( unsigned char* _buffer, unsigned int _pitch )
	{
		if( m_FrameRGBA == NULL )
		{
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: not valid RGBA Frame ");
			return 0;
		}
		
		// Convert the image from its native format to RGBA using 
        
		int fill_error = avpicture_fill( (AVPicture*) m_FrameRGBA, _buffer, (::PixelFormat) m_outputPixelFormat,
			m_dataInfo.frameWidth, m_dataInfo.frameHeight );

        m_FrameRGBA->linesize[0] = _pitch;

		int ret = sws_scale( m_imgConvertContext, m_Frame->data, m_Frame->linesize, 0, 
			m_codecContext->height, m_FrameRGBA->data, m_FrameRGBA->linesize );
		
		size_t decoded = _pitch * m_codecContext->height;
		
		return decoded;
	}
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderFFMPEG::readNextFrame()
	{	
		int isGotPicture;
		AVPacket packet;
		av_init_packet(&packet);
		//av_dup_packet(&packet);

		if( av_read_frame( m_formatContext, &packet ) < 0 )
		{
			av_free_packet(&packet);
			//printf(" can not read frame ");
			return VDRS_END_STREAM;
		}

		// Is this a packet from the video stream?
		if( packet.stream_index != m_videoStreamId )
		{
			av_free_packet(&packet);
			//printf(" packet is not video ");
			return VDRS_SKIP;
		}

		// Decode video frame
		avcodec_decode_video2( m_codecContext, m_Frame, &isGotPicture, &packet );

		// Did we get a video frame?
		if( isGotPicture <= 0 ) {
			av_free_packet(&packet);
			LOGGER_ERROR(m_logService)("VideoDecoderFFMPEG:: we don`t get a picture ");
			return VDRS_FAILURE;
		}
		
		m_pts = packet.pts;
		//printf("!!PTS %f \n",m_pts);
		
		av_free_packet(&packet);

		return VDRS_SUCCESS;	
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderFFMPEG::clear_()
	{
		// Free the packet that was allocated by av_read_frame
		if (m_Frame != NULL)
		{
			av_free(m_Frame);
			m_Frame = NULL;
		}
		
		// Free the packet that was allocated by av_read_frame
		if (m_FrameRGBA != NULL)
		{
			av_free(m_FrameRGBA);
			m_FrameRGBA = NULL;
		}
		
		//close codec
		if (m_codecContext != NULL)
		{
			avcodec_close(m_codecContext);
			m_codecContext = NULL;
		}
		// Close the video file
		if (m_formatContext != NULL)
		{
			avformat_close_input(&m_formatContext);
			m_formatContext = NULL;
		}
		
		if (m_IOContext != NULL)
		{
			av_free(m_IOContext); 
			m_IOContext = NULL;
		}
		
		if (m_bufferIO != NULL)
		{
			delete [] m_bufferIO;
			m_bufferIO = NULL;
		}

		if( m_imgConvertContext != NULL )
		{
			sws_freeContext( m_imgConvertContext );
			m_imgConvertContext = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::seek( float _timing )
	{
		int defaultStreamIndex = av_find_default_stream_index(m_formatContext);
		int seekStreamIndex = ( m_videoStreamId != -1 ) ? m_videoStreamId : defaultStreamIndex;
		
		/*AVRational av_q;
		av_q.num = 1; 
		av_q.den = AV_TIME_BASE; 
		int64_t seekTime = av_rescale_q(_timing, av_q, m_formatContext->streams[seekStreamIndex]->time_base);*/
		//int64_t ts = seekTime * AV_TIME_BASE;
		
		// seconds * TIMEBASE!!!!!
		//int64_t ts = (int64_t) ceil( (_timing / 1000.0f) * AV_TIME_BASE );
		
		//printf("SetTiming, need %4.2f real %i seekTime %4.2f\n", _timing, ts, seekTime);
		if( this->seekFrame_( _timing ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::eof()
	{
		if( Utils::eof( m_stream ) == true )
		{
			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderFFMPEG::setOptions( CodecOptions * _options )
	{
		VideoCodecOptions* options = static_cast<VideoCodecOptions*>(_options);
		m_options.pixelFormat = options->pixelFormat;
		this->_invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderFFMPEG::_invalidate()
	{
		switch(m_options.pixelFormat)
		{
		case (Menge::PF_A8R8G8B8):
			m_outputPixelFormat = PIX_FMT_BGRA;
			break;
		case (Menge::PF_R8G8B8):
			m_outputPixelFormat = PIX_FMT_RGB32;
			break;
		default:
			LOGGER_ERROR(m_logService)("pixel format %i is not supported" , m_options.pixelFormat);
			break;
		}

		m_imgConvertContext = sws_getCachedContext(NULL, m_codecContext->width, m_codecContext->height, 
			m_codecContext->pix_fmt, 
			m_codecContext->width, m_codecContext->height 
			, (::PixelFormat) m_outputPixelFormat , SWS_BICUBIC,
			NULL, NULL, NULL );

		if( m_imgConvertContext == NULL )
		{
			LOGGER_ERROR(m_logService)( "VideoDecoderFFMPEG::Cannot initialize the conversion context!\n");			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float VideoDecoderFFMPEG::getTiming() const
	{
		return m_pts;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::seekFrame_( int64_t _frame )
	{
		//printf("TIMING _ %i\n",_timing);
		
		int defaultStreamIndex = av_find_default_stream_index(m_formatContext);
		int seekStreamIndex = ( m_videoStreamId != -1 ) ? m_videoStreamId : defaultStreamIndex;
		int64_t minTime = _frame - m_dataInfo.frameTiming;
		int64_t maxTime = _frame + m_dataInfo.frameTiming;
		int64_t needTime = _frame;

		//if( av_seek_frame( m_formatContext, seekStreamIndex, _frame,  AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD ) < 0 )
		if( avformat_seek_file( m_formatContext, defaultStreamIndex, minTime, needTime, maxTime, AVSEEK_FLAG_ANY  ) < 0 )
		{
			return false;
		}

		avcodec_flush_buffers( m_codecContext );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}