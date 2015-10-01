#	pragma once

#	include "Interface/StreamInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "stdex/thread_guard.h"

namespace Menge
{
	class MemoryProxyInput
		: public MemoryProxyInputInterface
	{
	public:
		MemoryProxyInput();
		~MemoryProxyInput();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        void * setMemory( void * _memory, size_t _offset, size_t _size ) override;
		void * getMemory( size_t & _size ) const override;

	public:
		size_t read( void * _buf, size_t _count ) override;
		bool seek( size_t _pos ) override;
		bool skip( size_t _pos ) override;
		size_t tell() const override;
        size_t size() const override;
		bool eof() const override;

    public:
		bool time( uint64_t & _time ) const override;

	public:
		bool memory( void ** _memory, size_t * _size ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		unsigned char * m_data;
		size_t m_size;

		unsigned char * m_pos;
		unsigned char * m_end;		

		STDEX_THREAD_GUARD_INIT;
	};
}	// namespace Menge


