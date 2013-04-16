#	include "VideoDecoderFFMPEG.h"

#	include "Core/File.h"
#	include "Logger/Logger.h"

#	include <limits.h>
#	include <math.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	int ReadIOWrapper(void * _opaque, uint8_t *_bufer, int bufferSize)
	{
		InputStreamInterface * stream = static_cast<InputStreamInterface *>(_opaque);

        int res = stream->read( _bufer , bufferSize );
		
        return res;
	}
	//////////////////////////////////////////////////////////////////////////
	int64_t SeekIOWrapper(void *_opaque, int64_t _offset, int _whence)
	{
		InputStreamInterface * stream = static_cast<InputStreamInterface *>(_opaque);
		
		if( _whence == AVSEEK_SIZE )
		{
			size_t size = stream->size();

			return size;
		}
		
		int offset;
		
		if( _offset < 0 )
		{
			size_t pos = stream->tell();

			offset = pos + _offset;
		}
		else
		{
			offset = _offset;
		}
		
		stream->seek( offset );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderFFMPEG::VideoDecoderFFMPEG()
		: m_formatContext(NULL)
		, m_codecContext(NULL)
		, m_codec(NULL)
		, m_frame(NULL)
		, m_IOContext(NULL)
        , m_imgConvertContext(NULL)
		, m_inputFormat(NULL)
		, m_outputPixelFormat(PIX_FMT_RGBA)
		, m_videoStreamId(-1)
		, m_pts(0.0f)        
	{
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderFFMPEG::~VideoDecoderFFMPEG()
	{
		this->clear_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::_initialize()
	{
        uint8_t filebuffer[4096 + FF_INPUT_BUFFER_PADDING_SIZE];
       
        size_t stram_size = m_stream->size();
        size_t probe_size = 4096 > stram_size ? stram_size : 4096;

         memset( filebuffer + probe_size, 0, FF_INPUT_BUFFER_PADDING_SIZE );

        m_stream->read( filebuffer, probe_size );
        m_stream->seek( SEEK_SET );

        AVProbeData pd;
        pd.filename = "";
        pd.buf= (unsigned char *)filebuffer; 
        pd.buf_size = probe_size;

        m_inputFormat = av_probe_input_format( &pd, 1 );

        if( m_inputFormat == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: av_probe_input_format failed "
                );

            return false;
        }
			
        //m_bufferIO = new uint8_t[ m_probeSize + FF_INPUT_BUFFER_PADDING_SIZE ];
		memset( m_bufferIO + 4096, 0, FF_INPUT_BUFFER_PADDING_SIZE );

		m_IOContext = avio_alloc_context(
			 m_bufferIO //IO buffer
			, 4096 //size of IO buffer
			, 0 //write flag set to 0
			, m_stream //IO source - it will be send as opaque argument to IO callbacks
			, ReadIOWrapper //read callback 
			, NULL //write callback
			, SeekIOWrapper //seek callback
			);
		
		//m_IOContext.is_streamed = 0;

		
		m_formatContext = avformat_alloc_context();
		
		m_formatContext->pb = m_IOContext;

        int open_input_error = avformat_open_input( &m_formatContext, "", m_inputFormat, NULL );

		if( open_input_error < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "VideoDecoderFFMPEG:: Couldn't open stream %d"
                , open_input_error
                );

			return false;
		}
		
		int find_stream_info_error = avformat_find_stream_info( m_formatContext, NULL );

		if( find_stream_info_error < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "VideoDecoderFFMPEG:: Couldn't find stream information %d"
                , find_stream_info_error
                );

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
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: Didn't find a video stream ");

			return false; // Didn't find a video stream
		}
		// Get a pointer to the codec context for the video stream
		AVCodecID codec_id = m_formatContext->streams[m_videoStreamId]->codec->codec_id;

		// Find the decoder for the video stream
		m_codec = avcodec_find_decoder( codec_id );

        if( m_codec == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: Unsupported codec! ");

            return false;
        }

        AVCodecContext * streamCodecContext = m_formatContext->streams[m_videoStreamId]->codec;

        m_codecContext = avcodec_alloc_context3( m_codec );

        if( avcodec_copy_context( m_codecContext, streamCodecContext ) != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid copy codec context! ");

            return false;
        }

        m_codecContext->thread_count = 1;
				
		// Open codec
        int error_open = avcodec_open2( m_codecContext, m_codec, NULL );
		
        if( error_open < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: Could not open codec! %d"
                , error_open
                );

			return false; //
		}
		
		m_frame = avcodec_alloc_frame();

		if( m_frame == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: can not allocate  video frame"
                );

			return false;
		}

        //int picture_err = avpicture_alloc( &m_picture, m_codecContext->pix_fmt, m_codecContext->width, m_codecContext->height );

        //if( picture_err < 0 )
        //{
        //    LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: can not allocate  video frame"
        //        );

        //    return false;
        //}
	
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

        double fps = av_q2d( m_formatContext->streams[m_videoStreamId]->r_frame_rate );
		m_dataInfo.fps = (float)fps;

		if( int(m_dataInfo.fps) == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid Frame rate "
                );

			return false; 
		}
		
		m_dataInfo.frameTiming = 1000.f / m_dataInfo.fps;

        int64_t len = m_formatContext->duration - m_formatContext->start_time;
		m_dataInfo.duration = (len / AV_TIME_BASE) * 1000.0f;
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool VideoDecoderFFMPEG::checkVideoCorrect_()
    {
        bool video_error = false;

        if( m_dataInfo.frameWidth != m_codecContext->width || 
            m_dataInfo.frameHeight != m_codecContext->height )
        {
            LOGGER_ERROR(m_serviceProvider)("=============================================================");
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode invalid width height %d:%d - %d:%d"
                , m_dataInfo.frameWidth
                , m_dataInfo.frameHeight
                , m_codecContext->width
                , m_codecContext->height
                );

            video_error = true;
        }

        if( m_options.noSeek == false )
        {
            this->seek( m_dataInfo.frameTiming );
            this->readNextFrame( 0.f );

            if( fabsf(m_pts - m_dataInfo.frameTiming) > m_dataInfo.frameTiming )
            {
                LOGGER_ERROR(m_serviceProvider)("=============================================================");
                LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid Key Frame %f:%f (need one)"
                    , m_dataInfo.frameTiming
                    , m_pts
                    );

                video_error = true;                 
            }

            this->seek( 0.f );
        }

        if( video_error == true )
        {
            return false;
        }        

        return true;
    }
	////////////////////////////////////////////////////////////////////////// 
	unsigned int VideoDecoderFFMPEG::decode( unsigned char* _buffer, unsigned int _pitch )
	{
		int fill_error = avpicture_fill( &m_picture, _buffer, (::PixelFormat) m_outputPixelFormat,
			m_dataInfo.frameWidth, m_dataInfo.frameHeight );

        if( fill_error < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode avpicture_fill %d"
                , fill_error
                );

            return 0;
        }
        
        m_picture.linesize[0] = _pitch;

		int scale_height = sws_scale( m_imgConvertContext, m_frame->data, m_frame->linesize, 0, 
			m_codecContext->height, m_picture.data, m_picture.linesize );

        if( scale_height < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode sws_scale %d"
                , scale_height
                );

            return 0;
        }
		
		size_t decoded = _pitch * m_codecContext->height;
        	
		return decoded;
	}
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderFFMPEG::readNextFrame( float _pts )
	{		
        (void)_pts;

		AVPacket packet;
		av_init_packet(&packet);
        
        if( av_read_frame( m_formatContext, &packet ) < 0 )
        {
            av_free_packet(&packet);

            return VDRS_END_STREAM;
        }
         
        if( packet.stream_index != m_videoStreamId )
        {
            av_free_packet(&packet);

            return VDRS_SKIP;
        }

        int isGotPicture;
            
        int decode_bite = avcodec_decode_video2( m_codecContext, m_frame, &isGotPicture, &packet );
                        
        if( decode_bite < 0 ) 
        {
            av_free_packet(&packet);

            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: we don`t get a picture"
                );

            return VDRS_FAILURE;
        }

		m_pts = (float)packet.pts;
		
		av_free_packet(&packet);

        if( isGotPicture == 0 )
        {
            return VDRS_SKIP;
        }
        
		return VDRS_SUCCESS;	
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderFFMPEG::clear_()
	{
		// Free the packet that was allocated by av_read_frame
		if (m_frame != NULL)
		{
			av_free(m_frame);
			m_frame = NULL;
		}
		     
		//close codec
		if (m_codecContext != NULL)
		{
			avcodec_close(m_codecContext);
            av_free(m_codecContext);
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

		if( m_imgConvertContext != NULL )
		{
			sws_freeContext( m_imgConvertContext );
			m_imgConvertContext = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::seek( float _timing )
	{
        if( m_options.noSeek == true )
        {
            _timing = 0.f;
        }

        if( m_videoStreamId == -1 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::seek invalid m_videoStreamId [%d]"
                , m_videoStreamId
                );

            return false;
        }

        //int64_t minTime = (int64_t)(_timing - m_dataInfo.frameTiming);
        //int64_t maxTime = (int64_t)(_timing + m_dataInfo.frameTiming);
        int64_t needTime = (int64_t)(_timing);

        //if( av_seek_frame( m_formatContext, seekStreamIndex, _frame,  AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD ) < 0 )
        //int seek_err = avformat_seek_file( m_formatContext, m_videoStreamId, minTime, needTime, maxTime, AVSEEK_FLAG_ANY );
        int seek_err = avformat_seek_file( m_formatContext, m_videoStreamId, ((int64_t)_I64_MIN), needTime, ((int64_t)_I64_MAX), 0 );

        if( seek_err < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::seek %d:%f error [%d]"
                , m_videoStreamId
                , _timing
                , seek_err
                );

            return false;
        }

        avcodec_flush_buffers( m_codecContext );

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
	bool VideoDecoderFFMPEG::_invalidateOptions()
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
			LOGGER_ERROR(m_serviceProvider)("pixel format %i is not supported"
                , m_options.pixelFormat
                );
			break;
		}

		m_imgConvertContext = sws_getCachedContext(
            NULL
            , m_codecContext->width
            , m_codecContext->height
            , m_codecContext->pix_fmt
			, m_codecContext->width 
            , m_codecContext->height
            , (::PixelFormat)m_outputPixelFormat
            //, SWS_BICUBIC
            , SWS_FAST_BILINEAR
			, NULL 
            , NULL
            , NULL );

		if( m_imgConvertContext == NULL )
		{
            m_imgConvertContext = sws_getContext(
                m_codecContext->width
                , m_codecContext->height
                , m_codecContext->pix_fmt
                , m_codecContext->width 
                , m_codecContext->height
                , (::PixelFormat)m_outputPixelFormat
                //, SWS_BICUBIC
                , SWS_FAST_BILINEAR
                , NULL 
                , NULL
                , NULL );

            if( m_imgConvertContext == NULL )
            {
    			LOGGER_ERROR(m_serviceProvider)( "VideoDecoderFFMPEG::Cannot initialize the conversion context!"
                    );

                return false;
            }
		}

#   ifdef _DEBUG
        if( this->checkVideoCorrect_() == false )
        {
            return false;
        }
#   endif

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float VideoDecoderFFMPEG::getTiming() const
	{
		return m_pts;
	}
	//////////////////////////////////////////////////////////////////////////
}