/*
 *	WrapStream.cpp
 *
 *	Created by _Berserk_ on 18.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "WrapStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	WrapStream::WrapStream( DataStreamInterface* _stream, std::streampos _begin, std::streamsize _size )
		: m_stream( _stream )
		, m_streamBegin( _begin )
		, m_size( _size )
		, m_currentPos( _begin )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	WrapStream::~WrapStream()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void WrapStream::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize WrapStream::read( void* _buf, std::streamsize _count )
	{
		if( m_currentPos != m_stream->tell() )
		{
			m_stream->seek( m_currentPos );
		}

		if( m_currentPos + _count > m_streamBegin + m_size )
		{
			_count = m_streamBegin + m_size - m_currentPos;
		}

		std::streampos bitesRead = m_stream->read( _buf, _count );
		m_currentPos += bitesRead;
		return bitesRead;
	}
	//////////////////////////////////////////////////////////////////////////
	void WrapStream::seek( std::streamoff _pos )
	{
		if( _pos < 0 )
		{
			_pos = 0;
		}
		else if( _pos > m_size )
		{
			_pos = m_size;
		}

		m_currentPos = m_streamBegin + _pos;
		m_stream->seek( m_currentPos );
	}
	//////////////////////////////////////////////////////////////////////////
	void WrapStream::skip( std::streamoff _count )
	{
		m_currentPos += _count;
		if( m_currentPos < m_streamBegin )
		{
			m_currentPos = m_streamBegin;
		}
		else if( m_currentPos > m_streamBegin + m_size )
		{
			m_currentPos = m_streamBegin + m_size;
		}
		m_stream->seek( m_currentPos );
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos WrapStream::tell() const 
	{
		return (m_currentPos - m_streamBegin);
	}
	//////////////////////////////////////////////////////////////////////////
	bool WrapStream::eof() const 
	{
		return ( m_currentPos >= (m_streamBegin + m_size) );
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize WrapStream::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
