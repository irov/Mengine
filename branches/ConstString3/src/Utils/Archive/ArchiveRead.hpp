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

		static void memorycopy( void * _dist, const void * _source, size_t _size );

		template<class T>
		void readPOD( T & _t )
		{
			size_t size = sizeof(T);

			T tmp;
			Archive::value_type * buff = reinterpret_cast<Archive::value_type *>(&tmp);
			readBuffer( buff, size );

			ArchiveRead::memorycopy( &_t, buff, size);
			//_t = tmp;
		}

		void readBuffer( Archive::value_type * _begin, size_t _size );
		void readSize( size_t & _value );
		void readString( std::string & _value );

		const Archive::value_type * selectBuffer( size_t _size );

	public:
		void seek( size_t _pos );
		size_t length( size_t _pos ) const;
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