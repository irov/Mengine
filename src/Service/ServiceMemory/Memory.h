#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/MemoryInterface.h"

#   include "Factory/FactorablePtr.h"

namespace Menge
{
	class Memory
		: public MemoryInterface
	{
	public:
		Memory();
		~Memory();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void * newMemory( size_t _size ) override;

	public:
		inline void * getMemory() const override;
		inline size_t getSize() const override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		void * m_memory;
		size_t m_size;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void * Memory::getMemory() const
	{
		return m_memory;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t Memory::getSize() const
	{
		return m_size;
	}
}