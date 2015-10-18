#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#   include "Factory/FactorablePtr.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class MemoryGetterInterface
	{ 
	public:
		virtual void * getMemory() const = 0;
		virtual size_t getSize() const = 0;

	public:
		template<class T>
		T getMemoryT() const
		{
			void * memory = this->getMemory();

			return static_cast<T>(memory);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	class MemoryCacheBufferInterface
		: public FactorablePtr
		, public MemoryGetterInterface
	{
	public:
		virtual void * cacheMemory( size_t _size, const char * _doc ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryCacheBufferInterface> MemoryCacheBufferInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryCacheInputInterface
		: public InputStreamInterface
		, public MemoryGetterInterface
	{
	public:
		virtual bool cacheMemory( size_t _size, const char * _doc ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryCacheInputInterface> MemoryCacheInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryProxyInputInterface
		: public InputStreamInterface
		, public MemoryGetterInterface
	{
	public:
		virtual void * setMemory( void * _memory, size_t _offset, size_t _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryProxyInputInterface> MemoryProxyInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryInputInterface
		: public InputStreamInterface
		, public MemoryGetterInterface
	{
	public:
		virtual void * newMemory( size_t _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryInputInterface> MemoryInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryInterface
		: public FactorablePtr
		, public MemoryGetterInterface
	{
	public:
		virtual void * newMemory( size_t _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryInterface> MemoryInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "MemoryService" )

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual MemoryCacheBufferInterfacePtr createMemoryCacheBuffer() = 0;
		virtual MemoryCacheInputInterfacePtr createMemoryCacheInput() = 0;
		virtual MemoryProxyInputInterfacePtr createMemoryProxyInput() = 0;
		virtual MemoryInputInterfacePtr createMemoryInput() = 0;
		virtual MemoryInterfacePtr createMemory() = 0;

	public:
		virtual void clearCacheBuffers() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
#   define MEMORY_SERVICE( serviceProvider )\
	((Menge::MemoryServiceInterface *)SERVICE_GET(serviceProvider, Menge::MemoryServiceInterface))
}
