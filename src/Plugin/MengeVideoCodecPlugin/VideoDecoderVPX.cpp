#	include "VideoDecoderVPX.h"
#	include "VideoI420ToRGB24.h"

#	include "Core/File.h"
#	include "Logger/Logger.h"

#   define vpx_interface (vpx_codec_vp9_dx())

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderVPX::VideoDecoderVPX()
        : m_frame(nullptr)
        , m_iterator(nullptr)
        , m_pitch(0)
        , m_pts(0.f)
    {
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderVPX::~VideoDecoderVPX()
	{
		free( m_frame );
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderVPX::_initialize()
	{        		
        int flags = 0;
        vpx_codec_err_t err_dec_init = vpx_codec_dec_init( &m_codec, vpx_interface, NULL, flags);

        if( err_dec_init != VPX_CODEC_OK )
        {
            LOGGER_ERROR(m_serviceProvider)("VideoDecoderVPX::_initialize vpx_codec_dec_init %d"
                , err_dec_init
                );
            
            return false;
        }

		m_reader = new VideoMkvReader(m_stream);

		mkvparser::EBMLHeader ebmlHeader;
		long long pos = 0;
		ebmlHeader.Parse( m_reader, pos );

		mkvparser::Segment * segment;
		mkvparser::Segment::CreateInstance( m_reader, pos, segment );

		if( segment->Load() != 0 )
		{
			return false;
		}

		const mkvparser::Tracks* tracks = segment->GetTracks();

		const unsigned long tracksCount = tracks->GetTracksCount();

		for( unsigned long i = 0; i != tracksCount; ++i )
		{
			const mkvparser::Track * track = tracks->GetTrackByIndex( i );

			const char* codecId = track->GetCodecId();

			const char* codecName_ = track->GetCodecNameAsUTF8();

			const long long trackType = track->GetType();
			if( trackType == mkvparser::Track::kVideo )
			{
				m_track = static_cast<const mkvparser::VideoTrack *>(track);
				break;
			}
		}

		if( m_track == nullptr )
		{
			return false;
		}

		m_track->GetFirst( m_blockEntry );

		if( (m_blockEntry == nullptr) || m_blockEntry->EOS() == true )
		{
			return false;
		}

		m_dataInfo.frameWidth = m_track->GetWidth();
		m_dataInfo.frameHeight = m_track->GetHeight();
		const double rate = m_track->GetFrameRate();
		m_dataInfo.frameTiming = 33.3333f;
		m_dataInfo.fps = 30;

        return true;
	}
	////////////////////////////////////////////////////////////////////////// 
	size_t VideoDecoderVPX::decode( void * _buffer, size_t _bufferSize )
	{
        vpx_codec_iter_t iter = nullptr;
        vpx_image_t * img = vpx_codec_get_frame( &m_codec, &iter );

        if( img == nullptr )
        {
            return 0;
        }

        unsigned char * vpx_buff = static_cast<unsigned char *>(_buffer);

		Utils::I420ToRGB24(
            img->planes[0], img->stride[0],
            img->planes[1], img->stride[1],
            img->planes[2], img->stride[2],
            vpx_buff, m_pitch,
            img->d_w, img->d_h);

        return m_frameSize;
    }
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderVPX::readNextFrame( float & _pts )
	{	
		const mkvparser::Block * block  = m_blockEntry->GetBlock();

		const int frameCount = block->GetFrameCount();

		for (int i = 0; i < frameCount; ++i)
		{
			const mkvparser::Block::Frame & frame = block->GetFrame( i );

			m_frameSize = frame.len;
			//const long long offset = frame.pos;

			m_frame = (unsigned char *)realloc( m_frame, m_frameSize );

			frame.Read( m_reader, m_frame );

			vpx_codec_err_t err_decode = vpx_codec_decode( &m_codec, m_frame, m_frameSize, NULL, 0);

			if( err_decode != VPX_CODEC_OK )
			{
				return VDRS_FAILURE;
			}
		}

		m_track->GetNext( m_blockEntry, m_blockEntry );
        
        m_pts += m_dataInfo.frameTiming;

        return VDRS_SUCCESS;
    }
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderVPX::seek( float _timing )
	{
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void VideoDecoderVPX::setPitch( size_t _pitch )
    {
        m_pitch = _pitch;
    }
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderVPX::eof() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	float VideoDecoderVPX::getTiming() const
	{
		return m_pts;
	}
	//////////////////////////////////////////////////////////////////////////
}