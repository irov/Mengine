#pragma once

#include "Archive.hpp"

namespace Mengine
{
	class ArchiveWrite
	{
	public:
		ArchiveWrite( Archive & _arhive )
			: m_arhive(_arhive)
		{
		}

	protected:
		ArchiveWrite & operator = ( const ArchiveWrite & )
		{
			return *this;
		}

	public:
		template<class T>
		inline void write( const T & _t )
		{
			*this >> _t;
		}

		template<class T>
		inline void writePOD( const T & _t )
		{
			const Archive::value_type * buff = reinterpret_cast<const Archive::value_type *>(&_t);
			this->writeBuffer( buff, sizeof(T) );
		}

		template<class T>
		inline void writePODs( const T * _t, size_t _count )
		{
			const Archive::value_type * buff = reinterpret_cast<const Archive::value_type *>(_t);
			this->writeBuffer( buff, _count * sizeof(T) );
		}

		inline void writeBuffer( const Archive::value_type * _begin, size_t _size )
		{
			m_arhive.insert( m_arhive.end(), _begin, _begin + _size );
		}

	protected:
		Archive & m_arhive;
	};

	template<class T>
	void operator << ( ArchiveWrite & aw, const T & _value )
	{
		aw.writePOD( _value );
	}
}