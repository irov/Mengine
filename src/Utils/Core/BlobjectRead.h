#	pragma once

#	include "Config/Blobject.h"

#	include "stdex/memorycopy.h"


namespace Menge
{
	class BlobjectRead
	{
	public:
		BlobjectRead( Blobject::value_type * _buffer, Blobject::size_type _size )
			: m_begin(_buffer)
			, m_end(_buffer + _size)
			, m_seek(_buffer)
		{
		}

	public:
		inline void begin()
		{
			m_seek = m_begin;
		}

	public:
		template<class T>
		inline void read( T & _t )
		{
			*this >> _t;
		}

		template<class T>
		inline void readPOD( T & _t )
		{
			Blobject::value_type * buff = reinterpret_cast<Blobject::value_type *>(&_t);
			this->readBuffer( buff, sizeof(T) );
		}

		template<class T>
		inline void readPODs( T * _t, size_t _count )
		{
			Blobject::value_type * buff = reinterpret_cast<Blobject::value_type *>(_t);
			this->readBuffer( buff, _count * sizeof(T) );
		}

		inline void readBuffer( Blobject::value_type * _begin, size_t _size )
		{
			Blobject::value_type * it_begin = m_seek;
			m_seek += _size;

			//stdex::unchecked_array_iterator<TBlobject::value_type *> chkd_begin((TBlobject::value_type *)_begin);
			//std::copy( it_begin, m_seek, chkd_begin );

			stdex::memorycopy( _begin, 0, it_begin, _size );
		}

		const Blobject::value_type * selectBuffer( size_t _size )
		{
			const Blobject::value_type * buff = m_seek;
			m_seek += _size;

			return buff;
		}

	public:
		void seek( size_t _pos )
		{
			m_seek += _pos;
		}

		size_t length( size_t _pos ) const
		{
			size_t reading = m_seek - m_begin;
			size_t length = _pos - reading;

			return length;
		}

		inline bool eof() const
		{
			return m_seek == m_end;
		}

	protected:
		Blobject::value_type * m_begin;
		Blobject::value_type * m_end;
		Blobject::value_type * m_seek;
	};

	template<class T>
	void operator >> ( BlobjectRead & ar, T & _value )
	{
		ar.readPOD( _value );
	}
}