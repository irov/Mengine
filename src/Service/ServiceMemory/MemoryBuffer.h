#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#   include "Core/ServantBase.h"

#   include "Factory/FactorablePtr.h"

namespace Mengine
{
	class MemoryBuffer
		: public ServantBase<MemoryBufferInterface>
	{
	public:
		MemoryBuffer();
		~MemoryBuffer();

    public:
		void setMemory( const void * _ptr, size_t _size, const char * _file, uint32_t _line ) override;
		Pointer newMemory( size_t _size, const char * _file, uint32_t _line ) override;

	public:
		inline Pointer getMemory() const override;
		inline size_t getSize() const override;
		inline bool empty() const override;

	protected:
		void * m_memory;
		size_t m_size;
        const char * m_file;
        uint32_t m_line;
	};
	//////////////////////////////////////////////////////////////////////////
	inline Pointer MemoryBuffer::getMemory() const
	{
		return m_memory;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t MemoryBuffer::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool MemoryBuffer::empty() const
	{
		return m_size == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryBuffer> MemoryPtr;
}