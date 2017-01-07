#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServantInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const size_t INVALID_SIZE = (size_t)-1;
	static const size_t UNKNOWN_SIZE = (size_t)-1;
	//////////////////////////////////////////////////////////////////////////
	class InputStreamInterface        
        : public ServantInterface
	{
	public:
		virtual size_t read( void * _buf, size_t _size ) = 0;
		virtual bool seek( size_t _pos ) = 0;
		virtual bool skip( size_t _pos ) = 0;
		virtual size_t tell() const = 0;
		virtual size_t size() const = 0;
		virtual bool eof() const = 0;

	public:  
		virtual bool time( uint64_t & _time ) const = 0;

	public:
		virtual bool memory( void ** _memory, size_t * _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<InputStreamInterface> InputStreamInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class OutputStreamInterface
        : public ServantInterface
	{
	public:
		virtual bool write( const void * _data, size_t _size ) = 0;
		virtual size_t size() const = 0;

	public:
		virtual bool flush() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OutputStreamInterface> OutputStreamInterfacePtr;
}
