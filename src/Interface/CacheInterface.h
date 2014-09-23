#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Core/MemoryCacheInput.h"
#	include "Core/MemoryProxyInput.h"
#	include "Core/MemoryInput.h"

#   include "stdex/intrusive_ptr.h"

namespace Menge
{
	typedef uint32_t CacheBufferID;

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
		virtual MemoryCacheInputPtr createMemoryCacheInput() = 0;
		virtual MemoryProxyInputPtr createMemoryProxyInput() = 0;
		virtual MemoryInputPtr createMemoryInput() = 0;
	};

#   define CACHE_SERVICE( serviceProvider )\
	((Menge::CacheServiceInterface *)SERVICE_GET(serviceProvider, Menge::CacheServiceInterface))
}