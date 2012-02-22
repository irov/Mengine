#	pragma once

#	include "Archive.hpp"

#	include <string>

#   include "../Menge/LogEngine.h"

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
		void readPOD( T & _t )
		{
			Archive::value_type * buff = reinterpret_cast<Archive::value_type *>(&_t);
			this->readBuffer( buff, sizeof(T) );
		}

		void readBuffer( Archive::value_type * _begin, size_t _size );

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

	template<class T>
	void operator >> ( ArchiveRead & ar, T & _value )
	{
		ar.readPOD( _value );
	}
}