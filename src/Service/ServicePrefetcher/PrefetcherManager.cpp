#	include "PrefetcherManager.h"

#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PrefetcherService, Menge::PrefetcherServiceInterface, Menge::PrefetcherManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PrefetcherManager::PrefetcherManager()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PrefetcherManager::~PrefetcherManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * PrefetcherManager::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::initialize()
	{
		for( uint32_t i = 0; i != MENGINE_PREFETCHER_THREAD_COUNT; ++i )
		{
			THREAD_SERVICE(m_serviceProvider)
				->createThread( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ThreadPrefetcherManager"), -1 );
		}

		m_threadQueue = THREAD_SERVICE(m_serviceProvider)
			->runTaskQueue( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ThreadPrefetcherManager"), MENGINE_PREFETCHER_THREAD_COUNT, MENGINE_PREFETCHER_PACKET_SIZE );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		template<class T>
		class FVisitorReceiverJoinTask
		{
		public:
			FVisitorReceiverJoinTask( ThreadServiceInterface * _threadService )
				: m_threadService(_threadService)
			{
			}

		public:
			void operator()( T * _receiver )
			{
				m_threadService->joinTask( _receiver->prefetcher );				
			}

		protected:
			ThreadServiceInterface * m_threadService;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::finalize()
	{
		ThreadServiceInterface * threadService = THREAD_SERVICE(m_serviceProvider);

		FVisitorReceiverJoinTask<PrefetchImageDecoderReceiver> fvr_image(threadService);
		m_prefetchImageDecoderReceiver.foreach( fvr_image );
		m_prefetchImageDecoderReceiver.clear();

		FVisitorReceiverJoinTask<PrefetchSoundDecoderReceiver> fvr_sound(threadService);
		m_prefetchSoundDecoderReceiver.foreach( fvr_sound );
		m_prefetchSoundDecoderReceiver.clear();

		FVisitorReceiverJoinTask<PrefetchDataReceiver> fvr_data(threadService);
		m_prefetchDataReceiver.foreach( fvr_data );
		m_prefetchDataReceiver.clear();

		if( m_threadQueue != nullptr )
		{
			m_threadQueue->cancel();
			m_threadQueue = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::prefetchImageDecoder( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec )
	{
		if( m_threadQueue == nullptr )
		{
			return false;
		}

		PrefetchImageDecoderReceiver * receiver;
		if( m_prefetchImageDecoderReceiver.create_if( _pakName, _fileName, &receiver ) == true )
		{
			receiver->refcount = 1;
						
			ThreadTaskPrefetchImageDecoderPtr task = m_factoryThreadTaskPrefetchImageDecoder.createObjectT();
			task->setServiceProvider( m_serviceProvider );
			task->initialize( _pakName, _fileName, _codec );

			receiver->prefetcher = task;

			m_prefetchImageDecoderReceiver.insert( receiver, nullptr );

			m_threadQueue->addTask( task );

			return true;
		}

		++receiver->refcount;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::unfetchImageDecoder( const ConstString& _pakName, const FilePath& _fileName )
	{
		PrefetchImageDecoderReceiver * receiver;
		if( m_prefetchImageDecoderReceiver.has( _pakName, _fileName, &receiver ) == false )
		{
			return;
		}

		if( --receiver->refcount > 0 )
		{
			return;
		}

		receiver->prefetcher->cancel();

		m_prefetchImageDecoderReceiver.erase_node( receiver );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::getImageDecoder( const ConstString & _pakName, const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const
	{
		const PrefetchImageDecoderReceiver * receiver;
		if( m_prefetchImageDecoderReceiver.has( _pakName, _fileName, &receiver ) == false )
		{
			return false;
		}
				
		if( receiver->prefetcher->isComplete() == false )
		{
			return false;
		}

		if( receiver->prefetcher->isSuccessful() == false )
		{
			return false;
		}

		const ImageDecoderInterfacePtr & prefetch_decoder = receiver->prefetcher->getDecoder();

		if( prefetch_decoder == nullptr )
		{
			return false;
		}
		
		_decoder = prefetch_decoder;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::prefetchSoundDecoder( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec )
	{
		if( m_threadQueue == nullptr )
		{
			return false;
		}

		PrefetchSoundDecoderReceiver * receiver;
		if( m_prefetchSoundDecoderReceiver.create_if( _pakName, _fileName, &receiver ) == true )
		{
			receiver->refcount = 1;

			ThreadTaskPrefetchSoundDecoderPtr task = m_factoryThreadTaskPrefetchSoundDecoder.createObjectT();
			task->setServiceProvider( m_serviceProvider );
			task->initialize( _pakName, _fileName, _codec );

			receiver->prefetcher = task;

			m_prefetchSoundDecoderReceiver.insert( receiver, nullptr );

			m_threadQueue->addTask( task );

			return true;
		}
		
		++receiver->refcount;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::unfetchSoundDecoder( const ConstString& _pakName, const FilePath& _fileName )
	{
		PrefetchSoundDecoderReceiver * receiver;
		if( m_prefetchSoundDecoderReceiver.has( _pakName, _fileName, &receiver ) == false )
		{
			return;
		}

		if( --receiver->refcount > 0 )
		{
			return;
		}

		receiver->prefetcher->cancel();

		m_prefetchSoundDecoderReceiver.erase_node( receiver );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::getSoundDecoder( const ConstString& _pakName, const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const
	{
		const PrefetchSoundDecoderReceiver * receiver;
		if( m_prefetchSoundDecoderReceiver.has( _pakName, _fileName, &receiver ) == false )
		{
			return false;
		}

		if( receiver->prefetcher->isComplete() == false )
		{
			return false;
		}

		if( receiver->prefetcher->isSuccessful() == false )
		{
			return false;
		}

		const SoundDecoderInterfacePtr & prefetch_decoder = receiver->prefetcher->getDecoder();

		if( prefetch_decoder == nullptr )
		{
			return false;
		}

		_decoder = prefetch_decoder;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::prefetchData( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _dataflowType )
	{
		if( m_threadQueue == nullptr )
		{
			return false;
		}

		PrefetchDataReceiver * receiver;
		if( m_prefetchDataReceiver.create_if( _pakName, _fileName, &receiver ) == true )
		{
			receiver->refcount = 1;

			ThreadTaskPrefetchDataflowPtr task = m_factoryThreadTaskPrefetchDataflow.createObjectT();
			task->setServiceProvider( m_serviceProvider );
			task->initialize( _pakName, _fileName, _dataflowType );

			receiver->prefetcher = task;

			m_prefetchDataReceiver.insert( receiver, nullptr );

			m_threadQueue->addTask( task );

			return true;
		}

		++receiver->refcount;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::unfetchData( const ConstString& _pakName, const FilePath& _fileName )
	{
		PrefetchDataReceiver * receiver;
		if( m_prefetchDataReceiver.has( _pakName, _fileName, &receiver ) == false )
		{
			return;
		}

		if( --receiver->refcount > 0 )
		{
			return;
		}

		receiver->prefetcher->cancel();

		m_prefetchDataReceiver.erase_node( receiver );
	}
	//////////////////////////////////////////////////////////////////////////			
	bool PrefetcherManager::getData( const ConstString& _pakName, const FilePath & _fileName, DataInterfacePtr & _data ) const
	{
		const PrefetchDataReceiver * receiver;
		if( m_prefetchDataReceiver.has( _pakName, _fileName, &receiver ) == false )
		{
			return false;
		}

		if( receiver->prefetcher->isComplete() == false )
		{
			return false;
		}

		if( receiver->prefetcher->isSuccessful() == false )
		{
			return false;
		}

		const DataInterfacePtr & prefetch_data = receiver->prefetcher->getData();

		if( prefetch_data == nullptr )
		{
			return false;
		}

		_data = prefetch_data;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PrefetcherDebugInfo PrefetcherManager::getDebugInfo() const
	{
		PrefetcherDebugInfo info;

		info.decoderCount = 0;
		info.soundCount = 0;
		info.dataCount = 0;

		for( TMapPrefetchImageDecoderReceiver::const_iterator
			it = m_prefetchImageDecoderReceiver.begin(),
			it_end = m_prefetchImageDecoderReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchImageDecoderReceiver * receiver = *it;

			if( receiver->prefetcher->isComplete() == false )
			{
				continue;
			}

			++info.decoderCount;
		}

		for( TMapPrefetchSoundDecoderReceiver::const_iterator
			it = m_prefetchSoundDecoderReceiver.begin(),
			it_end = m_prefetchSoundDecoderReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchSoundDecoderReceiver * receiver = *it;

			if( receiver->prefetcher->isComplete() == false )
			{
				continue;
			}

			++info.soundCount;
		}

		for( TMapPrefetchDataReceiver::const_iterator
			it = m_prefetchDataReceiver.begin(),
			it_end = m_prefetchDataReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchDataReceiver * receiver = *it;

			if( receiver->prefetcher->isComplete() == false )
			{
				continue;
			}

			++info.dataCount;
		}

		return info;
	}
}