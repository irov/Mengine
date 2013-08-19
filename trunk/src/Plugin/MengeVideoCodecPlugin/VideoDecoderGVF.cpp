#	include "VideoDecoderGVF.h"

#	include "Core/File.h"
#	include "Logger/Logger.h"

#	include "webp/decode.h"
#	include "webp/mux.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderGVF::VideoDecoderGVF()
		: m_pts(0.f)        
        , m_frame(0)
        , m_pitch(0)
    {
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderGVF::~VideoDecoderGVF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderGVF::_initialize()
	{
        m_stream->read( &m_header, sizeof(m_header) );
        
        m_offsets.resize( (m_header.count + 1) );
        m_stream->read( &m_offsets[0], (m_header.count + 1) * sizeof(TVectorOffsets::value_type) );

		m_dataInfo.frameWidth = m_header.width;
        m_dataInfo.frameHeight = m_header.height;

		m_dataInfo.fps = m_header.fps;
		m_dataInfo.frameTiming = 1000.f / float(m_dataInfo.fps);

		m_dataInfo.duration = m_header.count / 30.f;

		return true;
	}
	////////////////////////////////////////////////////////////////////////// 
	size_t VideoDecoderGVF::decode( void * _buffer, size_t _bufferSize )
	{
        size_t dataOffset = m_offsets[m_frame];
        size_t dataSize = m_offsets[m_frame + 1] - dataOffset;

        static uint8_t * s_WEBPBuffer = nullptr;
        static size_t s_WEBPBufferSize = 0;

        if( s_WEBPBufferSize < dataSize )
        {
            delete [] s_WEBPBuffer;
            s_WEBPBuffer = new uint8_t[dataSize];
            s_WEBPBufferSize = dataSize;
        }
        
        m_stream->seek( dataOffset );
        m_stream->read( s_WEBPBuffer, dataSize );

        uint8_t * webp_buffer = static_cast<uint8_t *>(_buffer);

        if( WebPDecodeBGRAInto( s_WEBPBuffer, dataSize, webp_buffer, _bufferSize, m_pitch ) == nullptr )
        {
            return 0;
        }

        return dataSize;
    }
	//////////////////////////////////////////////////////////////////////////
	EVideoDecoderReadState VideoDecoderGVF::readNextFrame( float & _pts )
	{	
        ++m_frame;
        m_pts += m_dataInfo.frameTiming;

        if( m_frame == m_header.count )
        {
            m_frame = m_header.count - 1;
            //m_pts = 0;

            return VDRS_END_STREAM;
        }
               
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
	bool VideoDecoderGVF::eof()
	{
		if( Utils::eof( m_stream.get() ) == true )
		{
			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	float VideoDecoderGVF::getTiming() const
	{
		return m_pts;
	}
	//////////////////////////////////////////////////////////////////////////
}