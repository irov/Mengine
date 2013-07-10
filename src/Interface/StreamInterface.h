#	pragma once

#	include "Config/Typedef.h"

#   include "Factory/FactorablePtr.h"

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
		virtual bool time( uint64 & _time ) const = 0;
	};

    typedef stdex::intrusive_ptr<InputStreamInterface> InputStreamInterfacePtr;

	class OutputStreamInterface
        : public FactorablePtr
	{
	public:
		virtual bool write( const void* _data, size_t _count ) = 0;
		virtual bool flush() = 0;
	};

    typedef stdex::intrusive_ptr<OutputStreamInterface> OutputStreamInterfacePtr;
}
