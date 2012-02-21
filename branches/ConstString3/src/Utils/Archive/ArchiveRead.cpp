#	include "ArchiveRead.hpp"

#	include <memory.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ArchiveRead::ArchiveRead( Archive::const_iterator _begin, Archive::const_iterator _end )
		: m_begin(_begin)
		, m_end(_end)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveRead::begin()
	{
		m_seek = m_begin;
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveRead::memorycopy( void * _dist, const void * _source, size_t _size )
	{
		memcpy( _dist, _source, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveRead::readBuffer( Archive::value_type * _begin, size_t _size )
	{
		Archive::const_iterator it_begin = m_seek;
		std::advance( m_seek, _size );
		std::copy( it_begin, m_seek, _begin );
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveRead::readSize( size_t & _value )
	{
		unsigned char low_size;
		read( low_size );

		if( low_size == 255 )
		{
			read( _value );
		}
		else
		{
			_value = low_size;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveRead::readString( std::string & _value )
	{
		size_t size;
		read( size );

		if( size == 0 )
		{
			return;
		}

		char * tmp = new char[size];		
		readBuffer( reinterpret_cast<Archive::value_type *>(tmp), size );

		_value.assign(tmp, size);
		delete [] tmp;
	}
	//////////////////////////////////////////////////////////////////////////
	const Archive::value_type * ArchiveRead::selectBuffer( size_t _size )
	{
		const Archive::value_type * buff = &*m_seek;
		std::advance( m_seek, _size );
		return buff;
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveRead::seek( size_t _pos )
	{
		std::advance( m_seek, _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ArchiveRead::length( size_t _pos ) const
	{
		size_t reading = std::distance( m_begin, m_seek );
		size_t length = _pos - reading;
		return length;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ArchiveRead::eof() const
	{
		return m_seek == m_end;
	}
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, std::string & _value )
	{
		ar.readString( _value );
	}
}