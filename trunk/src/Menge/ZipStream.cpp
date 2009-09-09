/*
 *	ZipStream.cpp
 *
 *	Created by _Berserk_ on 17.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "ZipStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ZipStream::ZipStream( DataStreamInterface* _stream
						, std::streampos _begin
						, std::streamsize _size
						, std::streamsize _unzSize )
		: m_stream( _stream )
		, m_streamBegin( _begin )
		, m_streamSize( _size )
		, m_size( _unzSize )
		, m_currentStreamPos( m_streamBegin )
		, m_unzPos( 0 )
	{
		m_zStream.zalloc = (alloc_func)0;
		m_zStream.zfree = (free_func)0;
		m_zStream.opaque = (voidpf)0;

		m_zStream.next_in = Z_NULL;
		m_zStream.avail_in = 0;

		int err = inflateInit( &m_zStream );
		m_valid = ( err == Z_OK );
	}
	//////////////////////////////////////////////////////////////////////////
	ZipStream::~ZipStream()
	{
		if( m_valid == true )
		{
			inflateEnd( &m_zStream );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ZipStream::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ZipStream::read( void* _buf, std::streamsize _count )
	{
		/*if( m_valid == false || _count == 0 || _buf == NULL )
		{
			return 0;
		}

		if( m_currentStreamPos != m_stream->tell() )
		{
			m_stream->seek( m_currentStreamPos );
		}

		m_zStream.next_out = _buf;
		m_zStream.avail_out = _count;

		while( m_zStream.avail_out > 0 )
		{
			if( m_zStream.avail_in == 0 )
			{
				// fill input
			}
			inflate( &m_zStream, Z_SYNC_FLUSH );
		}*/

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ZipStream::seek( std::streamoff _pos )
	{
		if( m_valid == false )
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ZipStream::skip( std::streamoff _count )
	{
		if( m_valid == false )
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos ZipStream::tell() const 
	{
		if( m_valid == false )
		{
			return 0;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ZipStream::eof() const 
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ZipStream::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void ZipStream::readNextPortion_()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
