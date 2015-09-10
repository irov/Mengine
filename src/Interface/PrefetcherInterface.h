#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Interface/ImageCodecInterface.h"
#	include "Interface/SoundCodecInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/ParticleSystemInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

namespace Menge
{
	struct PrefetcherDebugInfo
	{
		uint32_t receiverCount;
	};

	class PrefetcherServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("PrefetcherService")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual void update() = 0;

	public:
		virtual bool prefetchImageDecoder( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec ) = 0;
		virtual bool getImageDecoder( const ConstString& _pakName, const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const = 0;
		
	public:
		virtual bool prefetchSoundDecoder( const ConstString & _pakName, const FilePath & _fileName, const ConstString & _codec ) = 0;
		virtual bool getSoundDecoder( const ConstString& _pakName, const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const = 0;
		
	public:
		virtual bool prefetchData( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType ) = 0;
		virtual bool getData( const ConstString& _pakName, const FilePath & _fileName, DataInterfacePtr & _data ) const = 0;

	public:
		virtual void unfetch( const ConstString& _pakName, const FilePath& _fileName ) = 0;

	public:
		virtual PrefetcherDebugInfo getDebugInfo() const = 0;
	};

#   define PREFETCHER_SERVICE( serviceProvider )\
	((Menge::PrefetcherServiceInterface*)SERVICE_GET(serviceProvider, Menge::PrefetcherServiceInterface))
}

