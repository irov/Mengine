#	include "VideoDecoderGVF.h"

#   include "Interface/StringizeInterface.h"

#	include "Core/Stream.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderGVF::VideoDecoderGVF()
		: m_frame(0)
		, m_frameCount(0)
		, m_pts(0.f)
		, m_framesOffset(nullptr)
    {
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderGVF::~VideoDecoderGVF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderGVF::_initialize()
	{		
		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "lz4") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderGVF::_finalize()
	{
		delete [] m_framesOffset;
		m_framesOffset = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderGVF::_prepareData()
	{
		uint32_t magic;
		m_stream->read( &magic, sizeof(magic) );

		if( magic != 0x2EFA8000 )
		{
			return false;
		}

		uint32_t version;
		m_stream->read( &version, sizeof(version) );

		if( version != 1 )
		{
			return false;
		}

		uint32_t width;
		m_stream->read( &width, sizeof(width) );

		uint32_t height;
		m_stream->read( &height, sizeof(height) );

		uint32_t format;
		m_stream->read( &format, sizeof(format) );

		uint32_t duration_ms;
		m_stream->read( &duration_ms, sizeof(duration_ms) );

		uint32_t framerate;
		m_stream->read( &framerate, sizeof(framerate) );

		uint32_t framecount;
		m_stream->read( &framecount, sizeof(framecount) );
		
		m_dataInfo.frameWidth = width;
        m_dataInfo.frameHeight = height;

		m_dataInfo.fps = framerate;

		m_dataInfo.duration = float(duration_ms);
		
		if( format == 0 )
		{
			m_dataInfo.format = PF_DXT1;
		}
		else if( format == 1 )
		{
			m_dataInfo.format = PF_ETC1;
		}
		else if( format == 2 )
		{
			m_dataInfo.format = PF_PVRTC4_RGB;
		}
		else
		{
			return false;
		}

		m_dataInfo.clamp = false;

		m_frameCount = framecount;

		m_framesOffset = new size_t [framecount];

		for( uint32_t i = 0; i != framecount; ++i )
		{
			uint32_t offset;
			m_stream->read( &offset, sizeof(offset) );

			m_framesOffset[i] = (size_t)offset;
		}

		return true;
	}
	////////////////////////////////////////////////////////////////////////// 
	size_t VideoDecoderGVF::decode( void * _buffer, size_t _bufferSize )
	{
		size_t frameOffset = m_framesOffset[m_frame];

		m_stream->seek( frameOffset );
		
		if( Helper::loadStreamArchiveInplace( m_serviceProvider, m_stream, m_archivator, _buffer, _bufferSize ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("VideoDecoderGVF::decode invalid load"
				);

			return 0;
		}

        return 1;
    }
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderGVF::readNextFrame( float & _pts )
	{	
        if( m_frame + 1 == m_frameCount )
        {
			m_pts = m_dataInfo.duration;
			_pts = m_pts;

            return VDRS_END_STREAM;
		}

		++m_frame;

		m_pts += m_dataInfo.getFrameTiming();
        _pts = m_pts;

        return VDRS_SUCCESS;
    }
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderGVF::seek( float _timing )
	{
		float frameTiming = m_dataInfo.getFrameTiming();
        m_frame = (uint32_t)((_timing / frameTiming) + 0.5f);
        m_pts = _timing;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void VideoDecoderGVF::setPitch( size_t _pitch )
    {
		(void)_pitch;
       //Empty
    }
	//////////////////////////////////////////////////////////////////////////
	float VideoDecoderGVF::getTiming() const
	{
		return m_pts;
	}
	//////////////////////////////////////////////////////////////////////////
}