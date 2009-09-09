/*
 *	BufferedFileInput.cpp
 *
 *	Created by _Berserk_ on 26.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "BufferedFileInput.h"

#	include <cassert>

namespace Menge
{
	static const int s_maxFileBufferSize = 1024 * 1024;					// 1MB
	//////////////////////////////////////////////////////////////////////////
	BufferedFileInput::BufferedFileInput()
		: m_iStream( NULL )
		, m_bufferBegin( 0 )
		, m_iStreamCursor( 0 )
		, m_bufferMaxSize( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BufferedFileInput::~BufferedFileInput()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BufferedFileInput::loadStream( InputStreamInterface* _iStream )
	{
		m_iStream = _iStream;
		assert( m_iStream != NULL );
		m_bufferBegin = 0;
		m_iStreamCursor = 0;
		m_iStreamSize = m_iStream->size();
		m_bufferMaxSize = std::min( m_iStreamSize, s_maxFileBufferSize );
		m_buffer.clear();
		m_buffer.reserve( m_bufferMaxSize );
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface* BufferedFileInput::unloadStream()
	{
		InputStreamInterface* stream = m_iStream;
		m_iStream = NULL;
		return stream;
	}
	//////////////////////////////////////////////////////////////////////////
	int BufferedFileInput::tell()
	{
		return m_iStreamCursor - m_buffer.size() + m_bufferBegin;
	}
	//////////////////////////////////////////////////////////////////////////
	int BufferedFileInput::read( void* _buf, int _count )
	{
		if( m_iStream == NULL )
		{
			return 0;
		}

		assert( _count > 0 );

		int bytesRead = 0;
		unsigned char* dstBuf = static_cast<unsigned char*>( _buf );

		// check how much data we have
		int bytesLeft = m_buffer.size() - m_bufferBegin + m_iStreamSize - m_iStreamCursor;
		if( bytesLeft < _count )
		{
			_count = bytesLeft;
		}

		while( _count > 0 )
		{
			// buffer is empty - fill it
			if( m_bufferBegin == m_buffer.size() )
			{
				int readSize = std::min( m_bufferMaxSize, m_iStreamSize - m_iStreamCursor );
				m_buffer.resize( readSize );
				int ret = m_iStream->read( &(m_buffer[0]), readSize );
				m_iStreamCursor += ret;
				m_bufferBegin = 0;
				if( ret < readSize )	// something wrong
				{
					break;
				}
			}

			// copy buffer data
			int bytesToCopy = std::min( static_cast<int>( m_buffer.size() ) - m_bufferBegin, _count );
			std::copy( &(m_buffer[0]) + m_bufferBegin
				, &(m_buffer[0]) + m_bufferBegin + bytesToCopy
				, dstBuf );
			m_bufferBegin += bytesToCopy;
			dstBuf += bytesToCopy;
			bytesRead += bytesToCopy;
			_count -= bytesToCopy;
		}

		return bytesRead;		
	}
	//////////////////////////////////////////////////////////////////////////
	void BufferedFileInput::seek( int _pos )
	{
		if( m_iStream == NULL )
		{
			return;
		}

		if( _pos > m_iStreamCursor
			|| _pos < m_iStreamCursor - static_cast<int>( m_buffer.size() ) )
		{
			m_iStream->seek( _pos );
			// set new pointer and drop buffer
			m_iStreamCursor = _pos;
			m_buffer.clear();
			m_bufferBegin = 0;
		}
		else	// we're still in buffer, just fix m_bufferBegin
		{
			m_bufferBegin = _pos - m_iStreamCursor + m_buffer.size();
		}

	}
	//////////////////////////////////////////////////////////////////////////
	int BufferedFileInput::size() const
	{
		return m_iStreamSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
