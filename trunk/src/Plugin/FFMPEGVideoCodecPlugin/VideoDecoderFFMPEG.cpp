#	include "VideoDecoderFFMPEG.h"

#	include "Interface/ConfigInterface.h"

#	include "Logger/Logger.h"

#	include <stdint.h>
#	include <math.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	static int s_readIOWrapper( void * _opaque, uint8_t *_bufer, int bufferSize )
	{
		InputStreamInterface * stream = static_cast<InputStreamInterface *>(_opaque);

        size_t res = stream->read( _bufer , bufferSize );
		
        return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static int64_t s_seekIOWrapper( void *_opaque, int64_t _offset, int _whence )
	{
		InputStreamInterface * stream = static_cast<InputStreamInterface *>(_opaque);
		
		switch( _whence )
		{
		case AVSEEK_SIZE:
			{
				size_t size = stream->size();
	
				return size;
			}break;

		case SEEK_SET:
			{
				if( stream->seek( (size_t)_offset ) == false )
				{
					return -1;
				}

				return _offset;
			}break;

		case SEEK_CUR:
			{
				size_t pos = stream->tell();

				size_t offset = pos + (size_t)_offset;

				if( stream->seek( offset ) == false )
				{
					return -1;
				}

				return offset;
			}break;
		
		case SEEK_END:
			{
				size_t size = stream->size();

				size_t offset = size + (size_t)_offset;

				if( stream->seek( offset ) == false )
				{
					return -1;
				}

				return offset;
			}break;
		}

		return -1;
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
	bool VideoDecoderFFMPEG::_prepareData()
	{
        //m_bufferIO = new uint8_t[ m_probeSize + FF_INPUT_BUFFER_PADDING_SIZE ];
		memset( m_bufferIO + FF_MIN_BUFFER_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE );

		m_IOContext = avio_alloc_context(
			 m_bufferIO //IO buffer
			, FF_MIN_BUFFER_SIZE //size of IO buffer
			, 0 //write flag set to 0
			, m_stream.get()//IO source - it will be send as opaque argument to IO callbacks
			, s_readIOWrapper //read callback 
			, nullptr //write callback
			, s_seekIOWrapper //seek callback
			);

		m_formatContext = avformat_alloc_context();

		m_formatContext->pb = m_IOContext;

		size_t stream_size = m_stream->size();
		size_t probe_size = FF_MIN_BUFFER_SIZE < stream_size ? FF_MIN_BUFFER_SIZE : stream_size;

		m_stream->read( m_bufferIO, probe_size );
		m_stream->seek( 0 );

		AVProbeData pd;
		pd.filename = "";
		pd.buf = m_bufferIO; 
		pd.buf_size = probe_size;

		AVInputFormat * inputFormat = av_probe_input_format( &pd, 1 );
		
		if( inputFormat == nullptr )
		{
		    LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::_prepareData av_probe_input_format failed "
		        );

		    return false;
		}

		//inputFormat->flags |= AVFMT_NOFILE;

		m_formatContext->iformat = inputFormat;
		m_formatContext->flags = AVFMT_FLAG_CUSTOM_IO;
		        
        int open_input_error = avformat_open_input( &m_formatContext, "", nullptr, nullptr );

		if( open_input_error != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::_prepareData Couldn't open stream %d"
                , open_input_error
                );

			return false;
		}

		int find_stream_info_error = avformat_find_stream_info( m_formatContext, nullptr );

		if( find_stream_info_error < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::_prepareData Couldn't find stream information %d"
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
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::_prepareData Didn't find a video stream ");

			return false; // Didn't find a video stream
		}

		// Get a pointer to the codec context for the video stream
        AVCodecContext * streamCodecContext = m_formatContext->streams[m_videoStreamId]->codec;
		AVCodecID codec_id = streamCodecContext->codec_id;

		// Find the decoder for the video stream
		AVCodec * codec = avcodec_find_decoder( codec_id );

        if( codec == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::_prepareData Unsupported codec! ");

            return false;
        }
                
        m_codecContext = streamCodecContext;
		
		// Open codec
        int error_open = avcodec_open2( m_codecContext, codec, NULL );
		
        if( error_open < 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::_prepareData Could not open codec! %d"
                , error_open
                );

			return false; //
		}        
		
		m_frame = av_frame_alloc();

		if( m_frame == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::_prepareData can not allocate  video frame"
                );

			return false;
		}

		switch( m_options.pixelFormat )
		{
		case Menge::PF_A8R8G8B8:
			{
				m_outputPixelFormat = PIX_FMT_BGRA;
			}break;
		case Menge::PF_X8R8G8B8:
			{
				m_outputPixelFormat = PIX_FMT_0RGB32;
			}break;
		case Menge::PF_R8G8B8:
			{
				m_outputPixelFormat = PIX_FMT_RGB32;
			}break;
		default:
			{
				LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::_prepareData pixel format %i is not supported"
					, m_options.pixelFormat
					);

				return false;
			}break;
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

		// Hack to correct wrong frame rates that seem to be generated by some codecs
		if( m_codecContext->time_base.num > 1000 && m_codecContext->time_base.den == 1 )
		{
		 	m_codecContext->time_base.den = 1000;
		}
		
		m_dataInfo.frameWidth = m_codecContext->width;
		m_dataInfo.frameHeight = m_codecContext->height;		

        double fps = av_q2d( m_formatContext->streams[m_videoStreamId]->r_frame_rate );

		m_dataInfo.fps = (uint32_t)fps;

		if( m_dataInfo.fps == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: invalid Frame rate "
                );

			return false; 
		}
		
        int64_t len = m_formatContext->duration - m_formatContext->start_time;

        double d_duration = double(len * 1000.0) / double(AV_TIME_BASE);
		m_dataInfo.duration = (float)d_duration;

		m_dataInfo.format = PF_UNKNOWN;
		m_dataInfo.clamp = true;

#   ifdef _DEBUG
		if( this->checkVideoCorrect_() == false )
		{
			return false;
		}
#   endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderFFMPEG::_rewind()
	{
		this->clear_();

		m_stream->seek( 0 );

		memset( m_bufferIO + FF_MIN_BUFFER_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE );

		m_IOContext = avio_alloc_context(
			m_bufferIO //IO buffer
			, FF_MIN_BUFFER_SIZE //size of IO buffer
			, 0 //write flag set to 0
			, m_stream.get()//IO source - it will be send as opaque argument to IO callbacks
			, s_readIOWrapper //read callback 
			, nullptr //write callback
			, s_seekIOWrapper //seek callback
			);

		m_formatContext = avformat_alloc_context();

		m_formatContext->pb = m_IOContext;

		size_t stream_size = m_stream->size();
		size_t probe_size = FF_MIN_BUFFER_SIZE < stream_size ? FF_MIN_BUFFER_SIZE : stream_size;

		m_stream->read( m_bufferIO, probe_size );
		m_stream->seek( 0 );

		AVProbeData pd;
		pd.filename = "";
		pd.buf = m_bufferIO; 
		pd.buf_size = probe_size;

		AVInputFormat * inputFormat = av_probe_input_format( &pd, 1 );

		if( inputFormat == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: av_probe_input_format failed "
				);

			return false;
		}

		//inputFormat->flags |= AVFMT_NOFILE;

		m_formatContext->iformat = inputFormat;
		m_formatContext->flags = AVFMT_FLAG_CUSTOM_IO;

		int open_input_error = avformat_open_input( &m_formatContext, "", nullptr, nullptr );

		if( open_input_error != 0 )
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
		for( uint32_t i = 0; i < m_formatContext->nb_streams; i++ )
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

			return false;
		}        

		m_frame = av_frame_alloc();

		if( m_frame == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG:: can not allocate  video frame"
				);

			return false;
		}

		// Hack to correct wrong frame rates that seem to be generated by some codecs
		if( m_codecContext->time_base.num > 1000 && m_codecContext->time_base.den == 1 )
		{
			m_codecContext->time_base.den = 1000;
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

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool VideoDecoderFFMPEG::checkVideoCorrect_()
    {
        bool video_error = false;

        if( m_dataInfo.frameWidth != (uint32_t)m_codecContext->width || 
            m_dataInfo.frameHeight != (uint32_t)m_codecContext->height )
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

		if( m_options.noSeek == false )
		{
			if(	m_formatContext->streams[m_videoStreamId]->index_entries->min_distance != 0 ||
				m_formatContext->streams[m_videoStreamId]->index_entries->timestamp != 0 )
			{
				bool NoCheckVideoKeyFrame = CONFIG_VALUE(m_serviceProvider, "Validation", "NoCheckVideoKeyFrame", false);

				if( NoCheckVideoKeyFrame == false )
				{
					LOGGER_ERROR(m_serviceProvider)("=============================================================");
					LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode invalid min distance %d or timestamp %d"
						, m_formatContext->streams[m_videoStreamId]->index_entries->min_distance
						, m_formatContext->streams[m_videoStreamId]->index_entries->timestamp
						);

					video_error = true;
				}
				else
				{
					LOGGER_WARNING(m_serviceProvider)("=============================================================");
					LOGGER_WARNING(m_serviceProvider)("VideoDecoderFFMPEG::decode invalid min distance %d or timestamp %d"
						, m_formatContext->streams[m_videoStreamId]->index_entries->min_distance
						, m_formatContext->streams[m_videoStreamId]->index_entries->timestamp
						);
				}
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
		(void)_bufferSize;

		if( m_options.mock == true )
		{
			return 1;
		}

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

        if( scale_height <= 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::decode sws_scale %d"
                , scale_height
                );

            return 0;
        }

        return 1;
    }
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderFFMPEG::readNextFrame( float & _pts )
	{		
		if( m_options.mock == true )
		{
			float frameTiming = m_dataInfo.getFrameTiming();
			m_pts += frameTiming;

			_pts = m_pts;

			return VDRS_SUCCESS;
		}

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
            av_frame_free(&m_frame);
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

		float correct_timing = _timing;

		float frameTiming = m_dataInfo.getFrameTiming();

		if( _timing > m_dataInfo.duration - frameTiming )
		{
			correct_timing = m_dataInfo.duration - frameTiming;
		}

		if( fabsf(m_pts - correct_timing) < frameTiming * 0.5f )
		{
			return true;
		}

		if( m_options.mock == true )
		{
			m_pts = correct_timing;

			return true;
		}
		
        int64_t minTime = (int64_t)(correct_timing - frameTiming);
        int64_t maxTime = (int64_t)(correct_timing + frameTiming);
        int64_t needTime = (int64_t)(correct_timing);
				
        //if( av_seek_frame( m_formatContext, seekStreamIndex, _frame,  AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD ) < 0 )
        int seek_err = avformat_seek_file( m_formatContext, m_videoStreamId, minTime, needTime, maxTime, AVSEEK_FLAG_ANY );
        //int seek_err = av_seek_frame( m_formatContext, m_videoStreamId, needTime, AVSEEK_FLAG_ANY );

        if( seek_err < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::seek %d from %f to %f duration %f error [%d]"
                , m_videoStreamId
				, m_pts
                , correct_timing
				, m_dataInfo.duration
                , seek_err
                );

			int seek_err2 = avformat_seek_file( m_formatContext, m_videoStreamId, 0, 0, 0, 0 );

			if( seek_err2 < 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("VideoDecoderFFMPEG::seek try second to restart - %d from %f to %f duration %f error [%d]"
					, m_videoStreamId
					, m_pts
					, 0
					, m_dataInfo.duration
					, seek_err
					);

				return false;
			}

			m_pts = 0.f;
        }
		else
		{
			m_pts = correct_timing;
		}		

        avcodec_flush_buffers( m_codecContext );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void VideoDecoderFFMPEG::setPitch( size_t _pitch )
    {
        m_pitch = _pitch;
    }
	//////////////////////////////////////////////////////////////////////////
	float VideoDecoderFFMPEG::getTiming() const
	{
		return m_pts;
	}
	//////////////////////////////////////////////////////////////////////////
}