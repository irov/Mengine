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
	void PrefetcherManager::finalize()
	{
		for( TVectorPrefetchReceiver::const_iterator
			it = m_prefetchReceiver.begin(),
			it_end = m_prefetchReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchReceiver & receiver = *it;

			receiver.prefetcher->cancel();
		}

		for( TVectorPrefetchReceiver::const_iterator
			it = m_prefetchReceiver.begin(),
			it_end = m_prefetchReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchReceiver & receiver = *it;

			THREAD_SERVICE( m_serviceProvider )
				->joinTask( receiver.prefetcher );
		}

		m_prefetchReceiver.clear();

		if( m_threadQueue != nullptr )
		{
			m_threadQueue->cancel();
			m_threadQueue = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FReceiverComplete
		{
			bool operator ()( const PrefetchReceiver & _receiver ) const
			{
				if( _receiver.prefetcher->isComplete() == false )
				{
					return false;
				}

				if( _receiver.prefetcher->isSuccessful() == false )
				{
					return false;
				}

				return true;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::update()
	{
		TVectorPrefetchReceiver::iterator it_erase = std::remove_if( m_prefetchReceiver.begin(), m_prefetchReceiver.end(), FReceiverComplete() );
		m_prefetchReceiver.erase( it_erase, m_prefetchReceiver.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::prefetchImageDecoder( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec )
	{
		if( m_threadQueue == nullptr )
		{
			return false;
		}

		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _fileName, &receiver ) == true )
		{
			++receiver->refcount;

			return true;
		}

		PrefetchReceiver new_receiver;

		new_receiver.refcount = 1;
	
		ThreadTaskPrefetchImageDecoderPtr task = m_factoryThreadTaskPrefetchImageDecoder.createObjectT();
		task->setServiceProvider( m_serviceProvider );
		task->initialize( _pakName, _fileName );
		task->setImageCodec( _codec );

		new_receiver.prefetcher = task;

		m_prefetchReceiver.push_back( new_receiver );

		m_threadQueue->addTask( task );

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::getImageDecoder( const ConstString & _pakName, const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const
	{
		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _fileName, &receiver ) == false )
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

		ThreadTaskPrefetchImageDecoderPtr prefetcherImageDecoder = stdex::intrusive_static_cast<ThreadTaskPrefetchImageDecoderPtr>(receiver->prefetcher);

		const ImageDecoderInterfacePtr & prefetch_decoder = prefetcherImageDecoder->getDecoder();

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

		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _fileName, &receiver ) == true )
		{
			++receiver->refcount;

			return true;
		}

		PrefetchReceiver new_receiver;

		new_receiver.refcount = 1;

		ThreadTaskPrefetchSoundDecoderPtr task = m_factoryThreadTaskPrefetchSoundDecoder.createObjectT();
		task->setServiceProvider( m_serviceProvider );
		task->initialize( _pakName, _fileName );
		task->setSoundCodec( _codec );

		new_receiver.prefetcher = task;

		m_prefetchReceiver.push_back( new_receiver );

		m_threadQueue->addTask( task );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::getSoundDecoder( const ConstString& _pakName, const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const
	{
		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _fileName, &receiver ) == false )
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

		ThreadTaskPrefetchSoundDecoderPtr prefetcherSoundDecoder = stdex::intrusive_static_cast<ThreadTaskPrefetchSoundDecoderPtr>(receiver->prefetcher);

		const SoundDecoderInterfacePtr & prefetch_decoder = prefetcherSoundDecoder->getDecoder();

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

		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _fileName, &receiver ) == true )
		{
			++receiver->refcount;

			return true;
		}

		PrefetchReceiver new_receiver;

		new_receiver.refcount = 1;

		ThreadTaskPrefetchDataflowPtr task = m_factoryThreadTaskPrefetchDataflow.createObjectT();
		task->setServiceProvider( m_serviceProvider );
		task->initialize( _pakName, _fileName );
		task->setDataflowType( _dataflowType );

		new_receiver.prefetcher = task;

		m_prefetchReceiver.push_back( new_receiver );

		m_threadQueue->addTask( task );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////			
	bool PrefetcherManager::getData( const ConstString& _pakName, const FilePath & _fileName, DataInterfacePtr & _data ) const
	{
		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _fileName, &receiver ) == false )
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

		ThreadTaskPrefetchDataflowPtr prefetcherDataflow = stdex::intrusive_static_cast<ThreadTaskPrefetchDataflowPtr>(receiver->prefetcher);

		const DataInterfacePtr & prefetch_data = prefetcherDataflow->getData();

		if( prefetch_data == nullptr )
		{
			return false;
		}

		_data = prefetch_data;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::unfetch( const ConstString& _pakName, const FilePath& _fileName )
	{
		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _fileName, &receiver ) == false )
		{
			return;
		}

		if( --receiver->refcount > 0 )
		{
			return;
		}

		receiver->prefetcher->cancel();
	}
	//////////////////////////////////////////////////////////////////////////
	PrefetcherDebugInfo PrefetcherManager::getDebugInfo() const
	{
		PrefetcherDebugInfo info;

		info.receiverCount = 0;

		for( TVectorPrefetchReceiver::const_iterator
			it = m_prefetchReceiver.begin(),
			it_end = m_prefetchReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchReceiver & receiver = *it;

			if( receiver.prefetcher->isComplete() == false )
			{
				continue;
			}

			++info.receiverCount;
		}
		
		return info;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::hasPrefetch( const ConstString& _pakName, const FilePath & _fileName, PrefetchReceiver ** _receiver ) const
	{ 
		for( TVectorPrefetchReceiver::const_iterator
			it = m_prefetchReceiver.begin(),
			it_end = m_prefetchReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchReceiver & receiver = *it;

			const ConstString & pakName = receiver.prefetcher->getPakName();

			if( pakName != _pakName )
			{
				continue;
			}

			const FilePath & filePath = receiver.prefetcher->getFilePath();

			if( filePath != _fileName )
			{
				continue;
			}

			*_receiver = const_cast<PrefetchReceiver *>(&receiver);

			return true;
		}
		
		return false;
	}
}