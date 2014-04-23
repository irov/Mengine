#	pragma once

#	include "Interface/StreamInterface.h"

#	include "stdex/memorycopy.h"

namespace Menge
{
	//const size_t STREAM_BUFFER_SIZE = 1024; //1kb

	template<size_t STREAM_BUFFER_SIZE>
	class InputStreamBuffer
	{
	public:
		InputStreamBuffer()
			: m_carriage(0)
			, m_capacity(0)
			, m_reading(0)
		{
		}

		InputStreamBuffer( const InputStreamInterfacePtr & _stream )
			: m_stream(_stream)
			, m_carriage(0)
			, m_capacity(0)
			, m_reading(0)
		{
		}

	public:
		void setStream( const InputStreamInterfacePtr & _stream )
		{
			m_stream = _stream;
		}

	public:
		size_t read( void * _buf, size_t _count )
		{
			if( _count > STREAM_BUFFER_SIZE )
			{            
				size_t tail = m_capacity - m_carriage;

				if( tail != 0 )
				{
					stdex::memorycopy( _buf, m_buffer + m_carriage, tail );
				}

				size_t toRead = _count - tail;
				size_t bytesRead = m_stream->read( (unsigned char *)_buf + tail, toRead );

				m_carriage = 0;
				m_capacity = 0;

				m_reading += bytesRead;

				return bytesRead + tail;
			}

			if( m_carriage + _count <= m_capacity )
			{
				stdex::memorycopy( _buf, m_buffer + m_carriage, _count );

				m_carriage += _count;

				return _count;
			}

			size_t tail = m_capacity - m_carriage;

			if( tail != 0 )
			{
				stdex::memorycopy( _buf, m_buffer + m_carriage, tail );
			}

			size_t bytesRead = m_stream->read( m_buffer, STREAM_BUFFER_SIZE );

			size_t readSize = (std::min)( (_count - tail), bytesRead );

			unsigned char * read_buf = (unsigned char *)_buf + tail;
			stdex::memorycopy( read_buf, m_buffer, readSize );

			m_carriage = readSize;
			m_capacity = bytesRead;

			m_reading += bytesRead;

			return readSize + tail;
		}

		bool seek( size_t _pos )
		{
			if( _pos >= m_reading - m_capacity && _pos < m_reading )
			{
				m_carriage = m_capacity - (m_reading - _pos);

				return true;
			}

			m_carriage = 0;
			m_capacity = 0;

			m_reading = _pos;

			bool result = m_stream->seek( _pos );

			return result;
		}

		size_t tell() const
		{
			size_t current = m_reading - m_capacity + m_carriage;

			return current;
		}

		size_t size() const
		{
			size_t size = m_stream->size();

			return size;
		}

		void skip( size_t _count )
		{
			size_t tell = this->tell();

			this->seek( tell + _count );
		}

	protected:
		InputStreamInterfacePtr m_stream;

		size_t m_carriage;
		size_t m_capacity;
		size_t m_reading;

		char m_buffer[STREAM_BUFFER_SIZE];
	};

	typedef InputStreamBuffer<1024> InputStreamBuffer1024;
	typedef InputStreamBuffer<4096> InputStreamBuffer4096;
	typedef InputStreamBuffer<8196> InputStreamBuffer8196;
}