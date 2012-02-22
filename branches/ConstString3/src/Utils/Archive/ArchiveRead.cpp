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
	void ArchiveRead::readBuffer( Archive::value_type * _begin, size_t _size )
	{
		Archive::const_iterator it_begin = m_seek;
		std::advance( m_seek, _size );
		std::copy( it_begin, m_seek, _begin );
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
}