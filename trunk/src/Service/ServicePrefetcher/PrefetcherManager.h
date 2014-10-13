#	include "Interface/PrefetcherInterface.h"

#	include "ThreadTaskPrefetchImageDecoder.h"
#	include "ThreadTaskPrefetchDataflow.h"
#	include "ThreadTaskPrefetchParticleEmitterContainer.h"

#	include "Factory/FactoryStore.h"

#	include "stdex/binary_vector.h"

#	ifndef MENGINE_PREFETCHER_THREAD_COUNT
#	define MENGINE_PREFETCHER_THREAD_COUNT 2
#	endif

#	ifndef MENGINE_PREFETCHER_PACKET_SIZE
#	define MENGINE_PREFETCHER_PACKET_SIZE 64
#	endif

namespace Menge
{
	class PrefetcherManager
		: public PrefetcherServiceInterface
	{
	public:
		PrefetcherManager();
		~PrefetcherManager();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		bool prefetchImageDecoder( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec ) override;
		void unfetchImageDecoder( const FilePath& _fileName ) override;

	public:
		bool getImageDecoder( const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const override;

	public:
		bool prefetchData( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType ) override;
		void unfetchData( const FilePath& _fileName ) override;

	public:
		bool getData( const FilePath & _fileName, DataInterfacePtr & _data ) const override;

	public:
		PrefetcherDebugInfo getDebugInfo() const override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ThreadQueueInterfacePtr m_threadQueue;

		struct PrefetchImageDecoderReceiver
		{
			uint32_t refcount;
			ThreadTaskPrefetchImageDecoderPtr prefetcher;
		};

		typedef FactoryPoolStore<ThreadTaskPrefetchImageDecoder, 16> TFactoryThreadTaskPrefetchImageDecoder;
		TFactoryThreadTaskPrefetchImageDecoder m_factoryThreadTaskPrefetchImageDecoder;
		
		typedef stdex::binary_vector<FilePath, PrefetchImageDecoderReceiver> TMapPrefetchImageDecoderReceiver;
		TMapPrefetchImageDecoderReceiver m_prefetchImageDecoderReceiver;

		struct PrefetchDataReceiver
		{
			uint32_t refcount;
			ThreadTaskPrefetchDataflowPtr prefetcher;
		};

		typedef FactoryPoolStore<ThreadTaskPrefetchDataflow, 16> TFactoryThreadTaskPrefetchDataflow;
		TFactoryThreadTaskPrefetchDataflow m_factoryThreadTaskPrefetchDataflow;

		typedef stdex::binary_vector<FilePath, PrefetchDataReceiver> TMapPrefetchDataReceiver;
		TMapPrefetchDataReceiver m_prefetchDataReceiver;
	};
}