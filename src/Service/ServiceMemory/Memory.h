#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/MemoryInterface.h"

#   include "Core/ServantBase.h"

#   include "Factory/FactorablePtr.h"

namespace Menge
{
	class Memory
		: public ServantBase<MemoryInterface>
	{
	public:
		Memory();
		~Memory();

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
	inline Pointer Memory::getMemory() const
	{
		return m_memory;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t Memory::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Memory::empty() const
	{
		return m_size == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Memory> MemoryPtr;
}