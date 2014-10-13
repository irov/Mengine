#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Interface/ImageCodecInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/ParticleSystemInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

namespace Menge
{
	struct PrefetcherDebugInfo
	{
		uint32_t decoderCount;
		uint32_t dataCount;
	};

	class PrefetcherServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("PrefetcherService")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual bool prefetchImageDecoder( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec ) = 0;
		virtual void unfetchImageDecoder( const FilePath& _fileName ) = 0;

	public:
		virtual bool getImageDecoder( const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const = 0;
		
	public:
		virtual bool prefetchData( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType ) = 0;
		virtual void unfetchData( const FilePath& _fileName ) = 0;

	public:
		virtual bool getData( const FilePath & _fileName, DataInterfacePtr & _data ) const = 0;

	public:
		virtual PrefetcherDebugInfo getDebugInfo() const = 0;
	};

#   define PREFETCHER_SERVICE( serviceProvider )\
	((Menge::PrefetcherServiceInterface*)SERVICE_GET(serviceProvider, Menge::PrefetcherServiceInterface))
}

