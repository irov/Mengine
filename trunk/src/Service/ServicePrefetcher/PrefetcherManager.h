#	include "Interface/PrefetcherInterface.h"

#	include "ThreadTaskPrefetchImageDecoder.h"
#	include "ThreadTaskPrefetchSoundDecoder.h"
#	include "ThreadTaskPrefetchDataflow.h"

#	include "Factory/FactoryStore.h"

#	include "Core/IntrusiveDuplexTree.h"

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
		bool prefetchImageDecoder( const ConstString & _pakName, const FilePath & _fileName, const ConstString & _codec ) override;
		void unfetchImageDecoder( const ConstString& _pakName, const FilePath & _fileName ) override;

	public:
		bool getImageDecoder( const ConstString & _pakName, const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const override;

	public:
		bool prefetchSoundDecoder( const ConstString & _pakName, const FilePath & _fileName, const ConstString & _codec ) override;
		void unfetchSoundDecoder( const ConstString& _pakName, const FilePath & _fileName ) override;

	public:
		bool getSoundDecoder( const ConstString& _pakName, const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const override;

	public:
		bool prefetchData( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType ) override;
		void unfetchData( const ConstString& _pakName, const FilePath& _fileName ) override;

	public:
		bool getData( const ConstString& _pakName, const FilePath & _fileName, DataInterfacePtr & _data ) const override;

	public:
		PrefetcherDebugInfo getDebugInfo() const override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ThreadQueueInterfacePtr m_threadQueue;

		struct PrefetchImageDecoderReceiver
			: public Factorable
			, public stdex::intrusive_duplex_tree_node<PrefetchImageDecoderReceiver>
		{
			typedef ConstString key_first_type;
			typedef ConstString::less_type less_first_type;

			typedef FilePath key_second_type;
			typedef FilePath::less_type less_second_type;

			struct key_first_getter_type
			{
				const ConstString & operator()( const PrefetchImageDecoderReceiver * _node ) const
				{
					return _node->prefetcher->getPakName();
				}
			};

			struct key_second_getter_type
			{
				const FilePath & operator()( const PrefetchImageDecoderReceiver * _node ) const
				{
					return _node->prefetcher->getFilePath();
				}
			};

			uint32_t refcount;
			ThreadTaskPrefetchImageDecoderPtr prefetcher;
		};

		typedef FactoryPoolStore<ThreadTaskPrefetchImageDecoder, 16> TFactoryThreadTaskPrefetchImageDecoder;
		TFactoryThreadTaskPrefetchImageDecoder m_factoryThreadTaskPrefetchImageDecoder;
		
		typedef IntrusiveDuplexTree<PrefetchImageDecoderReceiver, 16> TMapPrefetchImageDecoderReceiver;
		TMapPrefetchImageDecoderReceiver m_prefetchImageDecoderReceiver;

		struct PrefetchSoundDecoderReceiver
			: public Factorable
			, public stdex::intrusive_duplex_tree_node<PrefetchSoundDecoderReceiver>
		{
			typedef ConstString key_first_type;
			typedef ConstString::less_type less_first_type;

			typedef FilePath key_second_type;
			typedef FilePath::less_type less_second_type;

			struct key_first_getter_type
			{
				const ConstString & operator()( const PrefetchSoundDecoderReceiver * _node ) const
				{
					return _node->prefetcher->getPakName();
				}
			};

			struct key_second_getter_type
			{
				const FilePath & operator()( const PrefetchSoundDecoderReceiver * _node ) const
				{
					return _node->prefetcher->getFilePath();
				}
			};

			uint32_t refcount;
			ThreadTaskPrefetchSoundDecoderPtr prefetcher;
		};

		typedef FactoryPoolStore<ThreadTaskPrefetchSoundDecoder, 16> TFactoryThreadTaskPrefetchSoundDecoder;
		TFactoryThreadTaskPrefetchSoundDecoder m_factoryThreadTaskPrefetchSoundDecoder;

		typedef IntrusiveDuplexTree<PrefetchSoundDecoderReceiver, 16> TMapPrefetchSoundDecoderReceiver;
		TMapPrefetchSoundDecoderReceiver m_prefetchSoundDecoderReceiver;

		struct PrefetchDataReceiver
			: public Factorable
			, public stdex::intrusive_duplex_tree_node<PrefetchDataReceiver>
		{
			typedef ConstString key_first_type;
			typedef ConstString::less_type less_first_type;

			typedef FilePath key_second_type;
			typedef FilePath::less_type less_second_type;

			struct key_first_getter_type
			{
				const ConstString & operator()( const PrefetchDataReceiver * _node ) const
				{
					return _node->prefetcher->getPakName();
				}
			};

			struct key_second_getter_type
			{
				const FilePath & operator()( const PrefetchDataReceiver * _node ) const
				{
					return _node->prefetcher->getFilePath();
				}
			};

			uint32_t refcount;
			ThreadTaskPrefetchDataflowPtr prefetcher;
		};

		typedef FactoryPoolStore<ThreadTaskPrefetchDataflow, 16> TFactoryThreadTaskPrefetchDataflow;
		TFactoryThreadTaskPrefetchDataflow m_factoryThreadTaskPrefetchDataflow;

		typedef IntrusiveDuplexTree<PrefetchDataReceiver, 16> TMapPrefetchDataReceiver;
		TMapPrefetchDataReceiver m_prefetchDataReceiver;
	};
}