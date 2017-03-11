#	include "Interface/PrefetcherInterface.h"

#	include "ThreadTaskPrefetchImageDecoder.h"
#	include "ThreadTaskPrefetchSoundDecoder.h"
#	include "ThreadTaskPrefetchDataflow.h"

#   include "Core/ServiceBase.h"

#	include "Factory/Factory.h"

#	include "stdex/stl_map.h"

#	ifndef MENGINE_PREFETCHER_THREAD_COUNT
#	define MENGINE_PREFETCHER_THREAD_COUNT 2
#	endif

#	ifndef MENGINE_PREFETCHER_PACKET_SIZE
#	define MENGINE_PREFETCHER_PACKET_SIZE 64
#	endif

namespace Menge
{
	struct PrefetchReceiver
	{
		uint32_t refcount;
		ThreadTaskPrefetchPtr prefetcher;
	};

	class PrefetcherManager
		: public ServiceBase<PrefetcherServiceInterface>
	{
	public:
		PrefetcherManager();
		~PrefetcherManager();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		void update() override;

	public:
		bool prefetchImageDecoder( const ConstString & _pakName, const FilePath & _fileName, const ConstString & _codec ) override;		
		bool getImageDecoder( const ConstString & _pakName, const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const override;

	public:
		bool prefetchSoundDecoder( const ConstString & _pakName, const FilePath & _fileName, const ConstString & _codec ) override;		
		bool getSoundDecoder( const ConstString& _pakName, const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const override;

	public:
		bool prefetchData( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType ) override;		
		bool getData( const ConstString& _pakName, const FilePath & _fileName, DataInterfacePtr & _data ) const override;

	public:
		void unfetch( const ConstString& _pakName, const FilePath & _fileName ) override;

	public:
		PrefetcherDebugInfo getDebugInfo() const override;

	protected:
		bool hasPrefetch( const ConstString& _pakName, const FilePath & _fileName, PrefetchReceiver ** _receiver ) const;

	protected:
		ThreadQueueInterfacePtr m_threadQueue;

		FactoryPtr m_factoryThreadTaskPrefetchImageDecoder;
        FactoryPtr m_factoryThreadTaskPrefetchSoundDecoder;
        FactoryPtr m_factoryThreadTaskPrefetchDataflow;
		
		typedef std::pair<ConstString, FilePath> TKeyPrefetchReceiver;
		typedef stdex::map<TKeyPrefetchReceiver, PrefetchReceiver> TMapPrefetchReceiver;
		TMapPrefetchReceiver m_prefetchReceiver;
	};
}