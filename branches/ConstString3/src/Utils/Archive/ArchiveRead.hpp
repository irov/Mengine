#	pragma once

#	include "Archive.hpp"

#	include <string>

namespace Menge
{
	class ArchiveRead
	{
	public:
		ArchiveRead( Archive::const_iterator _begin, Archive::const_iterator _end );

	public:
		void begin();

	public:
		template<class T>
		void read( T & _t )
		{
			*this >> _t;
		}

		template<class T>
		inline void readPOD( T & _t )
		{
			readBuffer( (void *)&_t, sizeof(T) );
		}

		void readBuffer( void * _begin, std::size_t _size );
		void readSize( std::size_t & _value );
		void readString( std::string & _value );

		const Archive::value_type * selectBuffer( std::size_t _size );

	public:
		void seek( std::size_t _pos );
		std::size_t length( std::size_t _pos ) const;
		bool eof() const;

	protected:
		Archive::const_iterator m_begin;
		Archive::const_iterator m_end;
		Archive::const_iterator m_seek;
	};

	void operator >> ( ArchiveRead & ar, std::string & _value );

	template<class T>
	void operator >> ( ArchiveRead & ar, T & _value )
	{
		ar.readPOD( _value );
	}
}