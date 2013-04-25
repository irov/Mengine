#	pragma once

#	include "Config/Typedef.h"

#   include "Utils/Factory/Factorable.h"
#   include "Utils/Core/IntrusivePtr.h"

namespace Menge
{
	class InputStreamInterface
        //: public IntrusivePtr<InputStreamInterface>
        : public Factorable
	{
	public:
		virtual size_t read( void* _buf, size_t _count ) = 0;
		virtual void seek( size_t _pos ) = 0;
		virtual size_t tell() const = 0;
		virtual size_t size() const = 0;

	public:
		virtual bool time( time_t & _time ) const = 0;
	};

	class OutputStreamInterface
        : public Factorable
	{
	public:
		virtual bool write( const void* _data, size_t _count ) = 0;
		virtual void flush() = 0;
	};
}
