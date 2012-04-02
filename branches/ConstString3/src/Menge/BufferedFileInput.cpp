/*
 *	BufferedFileInput.cpp
 *
 *	Created by _Berserk_ on 26.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "BufferedFileInput.h"
#	include "FileBufferProvider.h"

#	include "FileSystem.h"

namespace Menge
{
	static const int s_maxFileBufferSize = 1024 * 1024;					// 1MB
	//////////////////////////////////////////////////////////////////////////
	BufferedFileInput::BufferedFileInput()
		: m_iStream(NULL)
		, m_buffer(NULL)
		, m_bufferBegin(0)
		, m_iStreamCursor(0)
		, m_iStreamSize(0)
		, m_bufferMaxSize(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BufferedFileInput::~BufferedFileInput()
	{
		if( m_buffer )
		{
			FileBufferProvider * bufferProvider = m_fileSystem->getBufferProvider();

			bufferProvider->releaseBuffer( m_buffer );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void BufferedFileInput::loadStream( FileInputStreamInterface* _iStream )
	{
		if( _iStream == NULL )
		{
			return;
		}

		m_iStream = _iStream;
		
		m_bufferBegin = 0;
		m_iStreamCursor = 0;
		m_iStreamSize = m_iStream->size();
		m_bufferMaxSize = std::min( m_iStreamSize, s_maxFileBufferSize );

		FileBufferProvider * bufferProvider = m_fileSystem->getBufferProvider();
		
		m_buffer = bufferProvider->getBuffer();
		m_buffer->reserve( m_bufferMaxSize );
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputStreamInterface* BufferedFileInput::unloadStream()
	{
		FileInputStreamInterface* stream = m_iStream;

		m_iStream = NULL;

		return stream;
	}
	//////////////////////////////////////////////////////////////////////////
	int BufferedFileInput::tell() const
	{
		return m_iStreamCursor - m_buffer->size() + m_bufferBegin;
	}
	//////////////////////////////////////////////////////////////////////////
	int BufferedFileInput::read( void * _buf, int _count )
	{
		if( m_iStream == NULL )
		{
			return 0;
		}

		//assert( _count > 0 );

		int bytesRead = 0;
		TBlobject::value_type * dstBuf = static_cast<TBlobject::value_type*>( _buf );

		// check how much data we have
		int bytesLeft = m_buffer->size() - m_bufferBegin + m_iStreamSize - m_iStreamCursor;
		if( bytesLeft < _count )
		{
			_count = bytesLeft;
		}

		while( _count > 0 )
		{
			// buffer is empty - fill it
			if( m_bufferBegin == m_buffer->size() )
			{
				int readSize = std::min( m_bufferMaxSize, m_iStreamSize - m_iStreamCursor );
				m_buffer->resize( readSize );
				int ret = m_iStream->read( &m_buffer->front(), readSize );
				m_iStreamCursor += ret;
				m_bufferBegin = 0;
				if( ret < readSize )	// something wrong
				{
					break;
				}
			}

			// copy buffer data
			int bytesToCopy = std::min( static_cast<int>( m_buffer->size() ) - m_bufferBegin, _count );
			std::copy( &m_buffer->front() + m_bufferBegin
				, &m_buffer->front() + m_bufferBegin + bytesToCopy
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
			|| _pos < m_iStreamCursor - static_cast<int>( m_buffer->size() ) )
		{
			m_iStream->seek( _pos );
			// set new pointer and drop buffer
			m_iStreamCursor = _pos;
			m_buffer->clear();
			m_bufferBegin = 0;
		}
		else	// we're still in buffer, just fix m_bufferBegin
		{
			m_bufferBegin = _pos - m_iStreamCursor + m_buffer->size();
		}

	}
	//////////////////////////////////////////////////////////////////////////
	int BufferedFileInput::size() const
	{
		return m_iStreamSize;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BufferedFileInput::time( time_t & _time ) const
	{
		if( m_iStream == NULL )
		{
			return false;
		}

		return m_iStream->time( _time );
	}
}	// namespace Menge
