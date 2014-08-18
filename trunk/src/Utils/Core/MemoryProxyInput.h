#	pragma once

#	include "Interface/StreamInterface.h"

namespace Menge
{
	class MemoryProxyInput
		: public InputStreamInterface
	{
	public:
		MemoryProxyInput();
		~MemoryProxyInput();

    public:
        void * setMemory( void * _memory, size_t _offset, size_t _size );
        void * getMemory( size_t & _size ) const;

	public:
		size_t read( void * _buf, size_t _count ) override;
		bool seek( size_t _pos ) override;
		bool skip( size_t _pos ) override;
		size_t tell() const override;
        size_t size() const override;
		bool eof() const override;

    public:
		bool time( uint64_t & _time ) const override;

	protected:
		unsigned char * m_data;
		size_t m_size;

		unsigned char * m_pos;
		unsigned char * m_end;		
	};

	typedef stdex::intrusive_ptr<MemoryProxyInput> MemoryProxyInputPtr;
}	// namespace Menge


