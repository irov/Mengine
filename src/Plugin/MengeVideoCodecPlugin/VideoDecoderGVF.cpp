#	include "VideoDecoderGVF.h"

#	include "Core/File.h"
#	include "Core/CacheMemoryBuffer.h"

#	include "Logger/Logger.h"

#	include "inc/crn_decomp_ext.h"

#	include <stdint.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static int s_gvf_read( void * _user, void * _buffer, uint32_t _size )
	{
		InputStreamInterface * stream = (InputStreamInterface *)_user;

		if( stream->read( _buffer, _size ) != _size )
		{
			return GVF_READ_FAILED;
		}

		return GVF_READ_SUCCESSFUL;
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_gvf_seek( void * _user, uint32_t _size )
	{
		InputStreamInterface * stream = (InputStreamInterface *)_user;

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
		, m_temp_size(0)
    {
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderGVF::~VideoDecoderGVF()
	{
		gvf_decoder_destroy( m_gvf );
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderGVF::_prepareData()
	{
		gvf_error_t err = gvf_decoder_create( &m_gvf, m_stream.get(), &s_gvf_read, &s_gvf_seek );

		if( err != GVF_ERROR_SUCCESSFUL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderGVF::_initialize invalid err code %d"
				, err
				);

			return false;
		}

		m_dataInfo.frameWidth = gvf_get_width_source( m_gvf );
        m_dataInfo.frameHeight = gvf_get_height_source( m_gvf );
		m_dataInfo.frameWidthHW = gvf_get_width_frame( m_gvf );
		m_dataInfo.frameHeightHW = gvf_get_height_frame( m_gvf );

		m_dataInfo.fps = gvf_get_fps( m_gvf );
		m_dataInfo.frameTiming = 1000.f / float(m_dataInfo.fps);

		m_frames = gvf_get_frames( m_gvf );
		m_dataInfo.duration = float(m_frames) / float(m_dataInfo.fps);

		uint16_t gvf_format = gvf_get_format( m_gvf );

		if( gvf_format == GVF_FORMAT_DXT5 )
		{
			m_dataInfo.format = PF_DXT5;
		}
		else if( gvf_format == GVF_FORMAT_DXT1 )
		{
			m_dataInfo.format = PF_DXT1;
		}
		
		m_dataInfo.clamp = false;

		m_temp_size = gvf_get_temp_size( m_gvf );

		return true;
	}
	////////////////////////////////////////////////////////////////////////// 
	size_t VideoDecoderGVF::decode( void * _buffer, size_t _bufferSize )
	{
		CacheMemoryBuffer row_buffer(m_serviceProvider, m_temp_size, "VideoDecoderGVF");
		void * temp_buffer = row_buffer.getMemory();

		gvf_error_t err = gvf_decode_frame( m_gvf, m_frame, temp_buffer, m_temp_size );

		if( err != GVF_ERROR_SUCCESSFUL )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderGVF::decode invalid err code %d"
				, err
				);

			return 0;
		}

		if( crnex::decode_crn( _buffer, _bufferSize, temp_buffer, m_temp_size, m_pitch ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderGVF::decode invalid decode crn"
				);

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