#	pragma once

#	include "Config/Typedef.h"

#   include "Utils/Factory/FactorablePtr.h"
#   include "Utils/Core/IntrusivePtr.h"

namespace Menge
{
	class InputStreamInterface        
        : public FactorablePtr
	{
	public:
		virtual size_t read( void* _buf, size_t _count ) = 0;
		virtual bool seek( size_t _pos ) = 0;
		virtual size_t tell() const = 0;
		virtual size_t size() const = 0;

	public:
		virtual bool time( time_t & _time ) const = 0;
	};

    typedef IntrusivePtr<InputStreamInterface> InputStreamInterfacePtr;

	class OutputStreamInterface
        : public FactorablePtr
	{
	public:
		virtual bool write( const void* _data, size_t _count ) = 0;
		virtual bool flush() = 0;
	};

    typedef IntrusivePtr<OutputStreamInterface> OutputStreamInterfacePtr;
}
