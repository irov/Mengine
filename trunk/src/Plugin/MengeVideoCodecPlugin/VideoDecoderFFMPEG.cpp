#	include "VideoDecoderFFMPEG.h"

#	include "Core/File.h"
#	include "Logger/Logger.h"

#	include <limits.h>
#	include <math.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	static int s_readIOWrapper( void * _opaque, uint8_t *_bufer, int bufferSize )
	{
		InputStreamBuffer * stream = static_cast<InputStreamBuffer *>(_opaque);

        size_t res = stream->read( _bufer , bufferSize );
		
        return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static int64_t s_seekIOWrapper( void *_opaque, int64_t _offset, int _whence )
	{
		InputStreamBuffer * stream = static_cast<InputStreamBuffer *>(_opaque);
		
		switch( _whence )
		{
		case AVSEEK_SIZE:
			{
				size_t size = stream->size();
	
				return size;
			}break;
		case SEEK_SET:
			{
				size_t offset;

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
			}break;
		default:
			{
				return -1;
			}break;
		}
		
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderFFMPEG::VideoDecoderFFMPEG()
		: m_formatContext(nullptr)
		, m_codecContext(nullptr)
		, m_frame(nullptr)
		, m_IOContext(nullptr)
        , m_imgConvertContext(nullptr)
        , m_imgConvertContextCache(true)
		, m_outputPixelFormat(PIX_FMT_RGBA)
		, m_videoStreamId(-1)
		, m_pts(0.f)
        , m_pitch(0)
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
		m_streamBuffer.setStream( m_stream );

        const int probe_buffer_io_size = 512;

        uint8_t filebuffer[probe_buffer_io_size + FF_INPUT_BUFFER_PADDING_SIZE];
       
        size_t stream_size = m_stream->size();
        size_t probe_size = probe_buffer_io_size > stream_size ? stream_size : probe_buffer_io_size;

        memset( filebuffer, 0, probe_buffer_io_size + FF_INPUT_BUFFER_PADDING_SIZE );

        m_streamBuffer.read( filebuffer, probe_size );
        m_streamBuffer.seek( 0 );

        AVProbeData pd;
        pd.filename = "";
        pd.buf = (unsigned char *)filebuffer; 
        pd.buf_size = probe_size;

        int score = 0;
        AVInputFormat * inputFormat = av_probe_input_format2( &pd, 1, &score );
        
        if( inputFormat == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: av_probe_input_format failed "
                );

            return false;
        }

        //m_bufferIO = new uint8_t[ m_probeSize + FF_INPUT_BUFFER_PADDING_SIZE ];
		memset( m_bufferIO + VIDEO_FFMPEG_BUFFER_IO_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE );

		m_IOContext = avio_alloc_context(
			 m_bufferIO //IO buffer
			, VIDEO_FFMPEG_BUFFER_IO_SIZE //size of IO buffer
			, 0 //write flag set to 0
			, &m_streamBuffer //IO source - it will be send as opaque argument to IO callbacks
			, s_readIOWrapper //read callback 
			, nullptr //write callback
			, s_seekIOWrapper //seek callback
			);

		//m_IOContext.is_streamed = 0;
        //return true;

		
		m_formatContext = avformat_alloc_context();

		m_formatContext->pb = m_IOContext;
        
        int open_input_error = avformat_open_input( &m_formatContext, "", inputFormat, nullptr );

		if( open_input_error < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: Couldn't open stream %d"
                , open_input_error
                );

			return false;
		}

		int find_stream_info_error = avformat_find_stream_info( m_formatContext, nullptr );

		if( find_stream_info_error < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: Couldn't find stream information %d"
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
        AVCodecContext * streamCodecContext = m_formatContext->streams[m_videoStreamId]->codec;
		AVCodecID codec_id = streamCodecContext->codec_id;

		// Find the decoder for the video stream
		AVCodec * codec = avcodec_find_decoder( codec_id );

        if( codec == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: Unsupported codec! ");

            return false;
        }
                
        m_codecContext = streamCodecContext;

        //m_codecContext = avcodec_alloc_context3( codec );
                
        m_codecContext->thread_count = 1;
		
		// Open codec
        int error_open = avcodec_open2( m_codecContext, codec, NULL );
		
        if( error_open < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: Could not open codec! %d"
                , error_open
                );

			return false; //
		}        
		
		m_frame = avcodec_alloc_frame();

		if( m_frame == nullptr )
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
		m_dataInfo.fps = (size_t)fps;

		if( m_dataInfo.fps == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid Frame rate "
                );

			return false; 
		}
		
		m_dataInfo.frameTiming = 1000.f / float(m_dataInfo.fps);

        int64_t len = m_formatContext->duration - m_formatContext->start_time;

        double d_duration = (len * 1000.0) / double(AV_TIME_BASE);
		m_dataInfo.duration = (float)d_duration;

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
		
		
		if( m_codecContext->codec_id == AV_CODEC_ID_VP6A || 
			m_codecContext->codec_id == AV_CODEC_ID_VP6F ||
			m_codecContext->codec_id == AV_CODEC_ID_VP6 )
		{
			if( m_codecContext->width % 16 != 0 || 
				m_codecContext->height % 16 != 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("=============================================================");
				LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode invalid width height mod 16! %d:%d need"
					, m_codecContext->width
					, m_codecContext->height
					, m_codecContext->width / 16 * 16
					, m_codecContext->height / 16 * 16
					);

				video_error = true;
			}
		}

        if( video_error == true )
        {
            return false;
        }        

        return true;
    }
	////////////////////////////////////////////////////////////////////////// 
	size_t VideoDecoderFFMPEG::decode( void * _buffer, size_t _bufferSize )
	{
        uint8_t * ffmpeg_buffer = (uint8_t *)_buffer;

        AVPicture picture;
        int fill_error = avpicture_fill( &picture, ffmpeg_buffer, (::PixelFormat) m_outputPixelFormat,
            m_dataInfo.frameWidth, m_dataInfo.frameHeight );

        if( fill_error < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode avpicture_fill %d"
                , fill_error
                );

            return 0;
        }

        picture.linesize[0] = m_pitch;

        int scale_height = sws_scale( m_imgConvertContext, m_frame->data, m_frame->linesize, 0, 
            m_codecContext->height, picture.data, picture.linesize );

        if( scale_height < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode sws_scale %d"
                , scale_height
                );

            return 0;
        }

        return _bufferSize;
    }
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderFFMPEG::readNextFrame( float & _pts )
	{		
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

        m_pts = (float)packet.pts;

        int isGotPicture = 0;        
        int decode_bite = avcodec_decode_video2( m_codecContext, m_frame, &isGotPicture, &packet );

        if( isGotPicture == 0 )
        {
            av_free_packet(&packet);

            return VDRS_SKIP;
        }

        if( decode_bite < 0 )
        {
            av_free_packet( &packet );

            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode we don`t get a picture"
                );

            return VDRS_FAILURE;
        }          

        av_free_packet(&packet);
        
        _pts = m_pts;

        return VDRS_SUCCESS;
    }
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderFFMPEG::clear_()
	{
        if( m_imgConvertContext != nullptr )
        {
            if( m_imgConvertContextCache == true )
            {
                sws_freeContext( m_imgConvertContext );
            }

            m_imgConvertContext = nullptr;
        }

        if( m_IOContext != nullptr )
        {
            avio_flush(m_IOContext);
            av_free(m_IOContext);
            m_IOContext = nullptr;
        }

        //close codec
        if( m_codecContext != nullptr )
        {
            avcodec_close(m_codecContext);
            //av_free(m_codecContext);

            m_codecContext = nullptr;
        }

        // Free the packet that was allocated by av_read_frame
        if( m_frame != nullptr )
        {
            avcodec_free_frame(&m_frame);
            m_frame = nullptr;
        }

		// Close the video file
		if( m_formatContext != nullptr )
		{
			avformat_close_input(&m_formatContext);
			m_formatContext = nullptr;
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

		if( fabsf(m_pts - _timing) < m_dataInfo.frameTiming * 0.5f )
		{
			return true;
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

		m_pts = _timing;

        avcodec_flush_buffers( m_codecContext );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void VideoDecoderFFMPEG::setPitch( size_t _pitch )
    {
        m_pitch = _pitch;
    }
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::eof() const
	{
		if( Utils::eof( m_stream.get() ) == true )
		{
			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::_invalidateOptions()
	{            
        if( m_codecContext == nullptr )
        {
            return false;
        }

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
            nullptr
            , m_codecContext->width
            , m_codecContext->height
            , m_codecContext->pix_fmt
			, m_codecContext->width 
            , m_codecContext->height
            , (::PixelFormat)m_outputPixelFormat
            //, SWS_BICUBIC
            , SWS_FAST_BILINEAR
			, nullptr 
            , nullptr
            , nullptr );

		if( m_imgConvertContext == nullptr )
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
                , nullptr 
                , nullptr
                , nullptr );

            if( m_imgConvertContext == nullptr )
            {
    			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::Cannot initialize the conversion context!"
                    );

                return false;
            }

            m_imgConvertContextCache = false;
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