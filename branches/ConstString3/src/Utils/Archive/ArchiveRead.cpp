#	include "ArchiveRead.hpp"

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
	void ArchiveRead::readBuffer( void * _begin, std::size_t _size )
	{
		Archive::const_iterator it_begin = m_seek;
		std::advance( m_seek, _size );
		std::copy( it_begin, m_seek, (Archive::value_type *)_begin );
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveRead::readSize( std::size_t & _value )
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
		std::size_t size;
		read( size );

		_value.resize( size );
		readBuffer( &*_value.begin(), size );
	}
	//////////////////////////////////////////////////////////////////////////
	const Archive::value_type * ArchiveRead::selectBuffer( std::size_t _size )
	{
		const Archive::value_type * buff = &*m_seek;
		std::advance( m_seek, _size );
		return buff;
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveRead::seek( std::size_t _pos )
	{
		std::advance( m_seek, _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ArchiveRead::length( std::size_t _pos ) const
	{
		std::size_t reading = std::distance( m_begin, m_seek );
		std::size_t length = _pos - reading;
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