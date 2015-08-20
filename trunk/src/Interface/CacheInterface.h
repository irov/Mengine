#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Core/MemoryCacheBuffer.h"
#	include "Core/MemoryCacheInput.h"
#	include "Core/MemoryProxyInput.h"
#	include "Core/MemoryInput.h"
#	include "Core/Memory.h"

#   include "stdex/intrusive_ptr.h"

namespace Menge
{
	typedef uint32_t CacheBufferID;

	static const CacheBufferID INVALID_CACHE_BUFFER_ID = 0;

	class CacheServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "CacheService" )

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual CacheBufferID lockBuffer( size_t _size, void ** _memory, const char * _doc ) = 0;
		virtual void unlockBuffer( CacheBufferID _bufferId ) = 0;

	public:
		virtual void clearBuffers() = 0;

	public:
		virtual MemoryCacheBufferPtr createMemoryCacheBuffer() = 0;
		virtual MemoryCacheInputPtr createMemoryCacheInput() = 0;
		virtual MemoryProxyInputPtr createMemoryProxyInput() = 0;
		virtual MemoryInputPtr createMemoryInput() = 0;
		virtual MemoryPtr createMemory() = 0;
	};

#   define CACHE_SERVICE( serviceProvider )\
	((Menge::CacheServiceInterface *)SERVICE_GET(serviceProvider, Menge::CacheServiceInterface))
}