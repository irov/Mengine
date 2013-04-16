#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class InputStreamInterface
	{
	public:
		virtual size_t read( void* _buf, size_t _count ) = 0;
		virtual void seek( size_t _pos ) = 0;
		virtual size_t tell() const = 0;
		virtual size_t size() const = 0;		

	public:
		virtual bool time( time_t & _time ) const = 0;

    public:
        virtual void destroy() = 0;
	};

	class OutputStreamInterface
	{
	public:
		virtual bool write( const void* _data, size_t _count ) = 0;
		virtual void flush() = 0;

    public:
        virtual void destroy() = 0;
	};
}
