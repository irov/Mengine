#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Config/Blobject.h"

#   include "stdex/intrusive_ptr.h"

namespace Menge
{
	class CacheBufferInterface
		: public FactorablePtr
	{
	public:
		 virtual TBlobject::value_type * getMemory() const = 0;

	public:
		template<class T>
		T * getMemoryT() const
		{
			TBlobject::value_type * memory = this->getMemory();

			return reinterpret_cast<T *>(memory);
		}
	};

	typedef stdex::intrusive_ptr<CacheBufferInterface> CacheBufferInterfacePtr;

	class CacheServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "CacheService" )

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual CacheBufferInterfacePtr lockBuffer( size_t _size ) = 0;
	};

#   define CACHE_SERVICE( serviceProvider )\
	(Menge::Helper::getService<Menge::CacheServiceInterface>(serviceProvider))
}