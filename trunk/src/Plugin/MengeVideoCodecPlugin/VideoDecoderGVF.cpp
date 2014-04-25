#	include "VideoDecoderGVF.h"

#	include "Core/File.h"
#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static int s_gvf_read( void * _user, void * _buffer, uint32_t _size )
	{
		InputStreamBuffer2048 * stream = (InputStreamBuffer2048 *)_user;

		if( stream->read( _buffer, _size ) != _size )
		{
			return GVF_READ_FAILED;
		}

		return GVF_READ_SUCCESSFUL;
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_gvf_seek( void * _user, uint32_t _size )
	{
		InputStreamBuffer2048 * stream = (InputStreamBuffer2048 *)_user;

		if( stream->seek( _size ) == false )
		{
			return GVF_SEEK_FAILED;
		}

		return GVF_SEEK_SUCCESSFUL;
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderGVF::VideoDecoderGVF()
		: m_gvf(nullptr)
		, m_frame(0)
		, m_frames(0)
		, m_pts(0.f)
		, m_pitch(0)
    {
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderGVF::~VideoDecoderGVF()
	{
		gvf_decoder_destroy( m_gvf );
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderGVF::_initialize()
	{
		m_streamBuffer = m_stream;

		if( gvf_decoder_create( &m_gvf, &m_streamBuffer, &s_gvf_read, &s_gvf_seek ) != GVF_ERROR_SUCCESSFUL )
		{
			return false;
		}

		m_dataInfo.frameWidth = gvf_get_width( m_gvf );
        m_dataInfo.frameHeight = gvf_get_height( m_gvf );

		m_dataInfo.fps = gvf_get_fps( m_gvf );
		m_dataInfo.frameTiming = 1000.f / float(m_dataInfo.fps);

		m_frames = gvf_get_frames( m_gvf );
		m_dataInfo.duration = float(m_frames) / float(m_dataInfo.fps);

		m_dataInfo.format = PF_DXT5;
		m_dataInfo.clamp = false;

		return true;
	}
	////////////////////////////////////////////////////////////////////////// 
	size_t VideoDecoderGVF::decode( void * _buffer, size_t _bufferSize )
	{
		if( gvf_decode_frame( m_gvf, m_frame, _buffer, _bufferSize ) != GVF_ERROR_SUCCESSFUL )
		{
			return 0;
		}

        return 1;
    }
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderGVF::readNextFrame( float & _pts )
	{	
        if( m_frame + 1 == m_frames )
        {
			_pts = m_pts;

            return VDRS_END_STREAM;
		}

		++m_frame;
		m_pts += m_dataInfo.frameTiming;

        _pts = m_pts;

        return VDRS_SUCCESS;
    }
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderGVF::seek( float _timing )
	{
        m_frame = (size_t)((_timing / m_dataInfo.frameTiming) + 0.5f);
        m_pts = _timing;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void VideoDecoderGVF::setPitch( size_t _pitch )
    {
       m_pitch = _pitch;
    }
	//////////////////////////////////////////////////////////////////////////
	float VideoDecoderGVF::getTiming() const
	{
		return m_pts;
	}
	//////////////////////////////////////////////////////////////////////////
}