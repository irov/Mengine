#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/MemoryInterface.h"

#   include "Core/ServantBase.h"

#	include "stdex/thread_guard.h"

namespace Menge
{
	class MemoryManager;

	class MemoryCacheInput
		: public ServantBase<MemoryCacheInputInterface>
	{
	public:
		MemoryCacheInput();
		~MemoryCacheInput();
		
	public:
		void setMemoryManager( MemoryManager * _memoryManager );

    public:
        Pointer cacheMemory( size_t _size, const char * _file, uint32_t _line ) override;
		
	public:
		Pointer getMemory() const override;
		size_t getSize() const override;

	public:
		size_t read( void * _buf, size_t _size ) override;
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
		void uncache_();

	protected:
		MemoryManager * m_memoryManager;

		uint32_t m_bufferId;

		unsigned char * m_data;
		size_t m_size;

		unsigned char * m_pos;
		unsigned char * m_end;		

		STDEX_THREAD_GUARD_INIT;
	};
}	// namespace Menge


