#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class InputStreamInterface
	{
	public:
		virtual int read( void* _buf, int _count ) = 0;
		virtual void seek( int _pos ) = 0;
		virtual int tell() const = 0;
		virtual int size() const = 0;		

	public:
		virtual bool time( time_t & _time ) const = 0;
	};

	class OutputStreamInterface
	{
	public:
		virtual void write( const void* _data, int _count ) = 0;
		virtual void flush() = 0;
	};
}
