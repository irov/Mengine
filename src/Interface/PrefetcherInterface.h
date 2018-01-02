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

    class PrefetcherObserverInterface
        : public FactorablePtr
    {
    public:
        virtual void onPrefetchPreparation() = 0;
        virtual bool onPrefetchRun() = 0;
        virtual void onPrefetchComplete( bool _successful ) = 0;
    };

    typedef stdex::intrusive_ptr<PrefetcherObserverInterface> PrefetcherObserverInterfacePtr;

	class PrefetcherServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("PrefetcherService")

	public:
		virtual void update() = 0;

	public:
		virtual bool prefetchImageDecoder( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec, const PrefetcherObserverInterfacePtr & _observer ) = 0;
		virtual bool getImageDecoder( const ConstString& _pakName, const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const = 0;
		
	public:
		virtual bool prefetchSoundDecoder( const ConstString & _pakName, const FilePath & _fileName, const ConstString & _codec, const PrefetcherObserverInterfacePtr & _observer ) = 0;
		virtual bool getSoundDecoder( const ConstString& _pakName, const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const = 0;
		
	public:
		virtual bool prefetchData( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType, const PrefetcherObserverInterfacePtr & _observer ) = 0;
		virtual bool getData( const ConstString& _pakName, const FilePath & _fileName, DataInterfacePtr & _data ) const = 0;

	public:
		virtual void unfetch( const ConstString& _pakName, const FilePath& _fileName ) = 0;

	public:
		virtual PrefetcherDebugInfo getDebugInfo() const = 0;
	};

#   define PREFETCHER_SERVICE( serviceProvider )\
	((Menge::PrefetcherServiceInterface*)SERVICE_GET(serviceProvider, Menge::PrefetcherServiceInterface))
}

