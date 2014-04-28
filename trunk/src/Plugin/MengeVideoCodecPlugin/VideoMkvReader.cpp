#	include "VideoMkvReader.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoMkvReader::VideoMkvReader( const InputStreamInterfacePtr & _stream )
		: m_stream(_stream)
		, m_size(0)
		, m_pos(0)
	{
		m_size = m_stream->size();
	}
	//////////////////////////////////////////////////////////////////////////
	int VideoMkvReader::Read( long long _offset, long _len, unsigned char * _buf )
	{
		if( _offset >= m_size )
		{
			return -1;
		}

		if( m_pos != _offset )
		{
			if( m_stream->seek( _offset ) == false )
			{
				return -1;
			}
		}

		m_pos = _offset + _len;

		const size_t size = m_stream->read( _buf, _len );

		if( size != _len )
		{
			return -1;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	int VideoMkvReader::Length( long long * _total, long long * _available )
	{
		if( _total != nullptr )
		{
			*_total = m_size;
		}

		if( _available != nullptr )
		{
			*_available = m_size;
		}

		return 0;
	}
}