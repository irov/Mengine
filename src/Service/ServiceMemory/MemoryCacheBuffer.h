#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/ServantBase.h"

#include "Kernel/Factorable.h"

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"

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
        void setBuffer( const void * _ptr, size_t _size, const Char * _doc, const Char * _file, uint32_t _line ) override;
		Pointer newBuffer( size_t _size, const Char * _doc, const Char * _file, uint32_t _line ) override;
		
    public:
		inline Pointer getBuffer() const override;

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
	inline Pointer MemoryCacheBuffer::getBuffer() const
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
    typedef IntrusivePtr<MemoryCacheBuffer> MemoryCacheBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}	


