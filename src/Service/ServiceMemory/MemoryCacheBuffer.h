#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Core/ServantBase.h"

#include "Factory/FactorablePtr.h"

#include "Core/FilePath.h"
#include "Core/ConstString.h"

namespace Mengine
{
	class MemoryManager;

	class MemoryCacheBuffer
		: public ServantBase<MemoryBufferInterface>
	{
	public:
		MemoryCacheBuffer();
		~MemoryCacheBuffer() override;
		
	public:
		void setMemoryManager( MemoryManager * _memoryManager );

    public:
        void setMemory( const void * _ptr, size_t _size, const char * _file, uint32_t _line ) override;
		Pointer newMemory( size_t _size, const char * _file, uint32_t _line ) override;
		
    public:
		inline Pointer getMemory() const override;

		inline size_t getSize() const override;
		inline bool empty() const override;

	protected:
		void uncache_();

	protected:
		MemoryManager * m_memoryManager;

		uint32_t m_bufferId;

		void * m_data;
		size_t m_size;
	};
	//////////////////////////////////////////////////////////////////////////	
	inline Pointer MemoryCacheBuffer::getMemory() const
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t MemoryCacheBuffer::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool MemoryCacheBuffer::empty() const
	{
		return m_size == 0;
	}
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<MemoryCacheBuffer> MemoryCacheBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}	


