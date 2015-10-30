#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#   include "Factory/FactorablePtr.h"

#	include "Core/Pointer.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class MemoryGetterBufferInterface
		: public FactorablePtr
	{ 
	public:
		virtual Pointer getMemory() const = 0;
		virtual size_t getSize() const = 0;
		virtual bool empty() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class MemoryCacheBufferInterface		
		: public MemoryGetterBufferInterface
	{
	public:
		virtual Pointer cacheMemory( size_t _size, const char * _doc ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryCacheBufferInterface> MemoryCacheBufferInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryInterface
		: public MemoryGetterBufferInterface
	{
	public:
		virtual Pointer newMemory( size_t _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryInterface> MemoryInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryGetterStreamInterface
		: public InputStreamInterface
	{
	public:
		virtual Pointer getMemory() const = 0;
		virtual size_t getSize() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class MemoryCacheInputInterface
		: public MemoryGetterStreamInterface
	{
	public:
		virtual Pointer cacheMemory( size_t _size, const char * _doc ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryCacheInputInterface> MemoryCacheInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryProxyInputInterface
		: public MemoryGetterStreamInterface
	{
	public:
		virtual Pointer setMemory( void * _memory, size_t _offset, size_t _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryProxyInputInterface> MemoryProxyInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryInputInterface
		: public MemoryGetterStreamInterface
	{
	public:
		virtual Pointer newMemory( size_t _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryInputInterface> MemoryInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "MemoryService" )

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
