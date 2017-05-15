#	include "PrefetcherManager.h"

#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Factory/FactoryPool.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PrefetcherService, Menge::PrefetcherManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PrefetcherManager::PrefetcherManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PrefetcherManager::~PrefetcherManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::_initialize()
	{
		for( uint32_t i = 0; i != MENGINE_PREFETCHER_THREAD_COUNT; ++i )
		{
			THREAD_SERVICE(m_serviceProvider)
				->createThread( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ThreadPrefetcherManager"), -1, __FILE__, __LINE__ );
		}

		m_threadQueue = THREAD_SERVICE(m_serviceProvider)
			->runTaskQueue( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ThreadPrefetcherManager"), MENGINE_PREFETCHER_THREAD_COUNT, MENGINE_PREFETCHER_PACKET_SIZE );

        m_factoryThreadTaskPrefetchImageDecoder = new FactoryPool<ThreadTaskPrefetchImageDecoder, 16>( m_serviceProvider );
        m_factoryThreadTaskPrefetchSoundDecoder = new FactoryPool<ThreadTaskPrefetchSoundDecoder, 16>( m_serviceProvider );
        m_factoryThreadTaskPrefetchDataflow = new FactoryPool<ThreadTaskPrefetchDataflow, 16>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::_finalize()
	{
		for( TMapPrefetchReceiver::const_iterator
			it = m_prefetchReceiver.begin(),
			it_end = m_prefetchReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchReceiver & receiver = it->second;

			receiver.prefetcher->cancel();
		}

		for( TMapPrefetchReceiver::const_iterator
			it = m_prefetchReceiver.begin(),
			it_end = m_prefetchReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchReceiver & receiver = it->second;

			THREAD_SERVICE( m_serviceProvider )
				->joinTask( receiver.prefetcher );
		}

		m_prefetchReceiver.clear();

		if( m_threadQueue != nullptr )
		{
			THREAD_SERVICE( m_serviceProvider )
				->cancelTaskQueue( m_threadQueue );

			m_threadQueue = nullptr;
		}

        m_factoryThreadTaskPrefetchImageDecoder = nullptr;
        m_factoryThreadTaskPrefetchSoundDecoder = nullptr;
        m_factoryThreadTaskPrefetchDataflow = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FReceiverComplete
		{
			bool operator ()( const PrefetchReceiver & _receiver ) const
			{
				if( _receiver.refcount != 0 )
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
		//TMapPrefetchReceiver::iterator it_erase = std::remove_if( m_prefetchReceiver.begin(), m_prefetchReceiver.end(), FReceiverComplete() );
		//m_prefetchReceiver.erase( it_erase, m_prefetchReceiver.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::prefetchImageDecoder( const ConstString& _pakName, const FilePath & _filePath, const ConstString & _codec )
	{
		if( m_threadQueue == nullptr )
		{
			return false;
		}

		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _filePath, &receiver ) == true )
		{
			++receiver->refcount;

			return true;
		}

		PrefetchReceiver new_receiver;

		new_receiver.refcount = 1;
	
		ThreadTaskPrefetchImageDecoderPtr task = m_factoryThreadTaskPrefetchImageDecoder->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _pakName, _filePath );
		task->setImageCodec( _codec );

		new_receiver.prefetcher = task;

		m_prefetchReceiver.insert( std::make_pair( std::make_pair( _pakName, _filePath ), new_receiver ) );

		m_threadQueue->addTask( task );

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::getImageDecoder( const ConstString & _pakName, const FilePath & _filePath, ImageDecoderInterfacePtr & _decoder ) const
	{
		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _filePath, &receiver ) == false )
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
	bool PrefetcherManager::prefetchSoundDecoder( const ConstString& _pakName, const FilePath & _filePath, const ConstString & _codec )
	{
		if( m_threadQueue == nullptr )
		{
			return false;
		}

		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _filePath, &receiver ) == true )
		{
			++receiver->refcount;

			return true;
		}

		PrefetchReceiver new_receiver;

		new_receiver.refcount = 1;

		ThreadTaskPrefetchSoundDecoderPtr task = m_factoryThreadTaskPrefetchSoundDecoder->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _pakName, _filePath );
		task->setSoundCodec( _codec );

		new_receiver.prefetcher = task;
				
		m_prefetchReceiver.insert( std::make_pair( std::make_pair( _pakName, _filePath ), new_receiver ) );

		m_threadQueue->addTask( task );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::getSoundDecoder( const ConstString& _pakName, const FilePath & _filePath, SoundDecoderInterfacePtr & _decoder ) const
	{
		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _filePath, &receiver ) == false )
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
	bool PrefetcherManager::prefetchData( const ConstString& _pakName, const FilePath & _filePath, const ConstString & _dataflowType )
	{
		if( m_threadQueue == nullptr )
		{
			return false;
		}

		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _filePath, &receiver ) == true )
		{
			++receiver->refcount;

			return true;
		}

		PrefetchReceiver new_receiver;

		new_receiver.refcount = 1;

		ThreadTaskPrefetchDataflowPtr task = m_factoryThreadTaskPrefetchDataflow->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _pakName, _filePath );
		task->setDataflowType( _dataflowType );

		new_receiver.prefetcher = task;

		m_prefetchReceiver.insert( std::make_pair( std::make_pair( _pakName, _filePath ), new_receiver ) );

		m_threadQueue->addTask( task );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////			
	bool PrefetcherManager::getData( const ConstString& _pakName, const FilePath & _filePath, DataInterfacePtr & _data ) const
	{
		PrefetchReceiver * receiver;
		if( this->hasPrefetch( _pakName, _filePath, &receiver ) == false )
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
	void PrefetcherManager::unfetch( const ConstString& _pakName, const FilePath& _filePath )
	{
		TMapPrefetchReceiver::iterator it_found = m_prefetchReceiver.find( std::make_pair( _pakName, _filePath ) );

		if( it_found == m_prefetchReceiver.end() )
		{
			return;
		}

		PrefetchReceiver & receiver = it_found->second;

		if( --receiver.refcount > 0 )
		{
			return;
		}

		receiver.prefetcher->cancel();

		m_prefetchReceiver.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	PrefetcherDebugInfo PrefetcherManager::getDebugInfo() const
	{
		PrefetcherDebugInfo info;

		info.receiverCount = 0;

		for( TMapPrefetchReceiver::const_iterator
			it = m_prefetchReceiver.begin(),
			it_end = m_prefetchReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchReceiver & receiver = it->second;

			if( receiver.prefetcher->isComplete() == false )
			{
				continue;
			}

			++info.receiverCount;
		}
		
		return info;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::hasPrefetch( const ConstString& _pakName, const FilePath & _filePath, PrefetchReceiver ** _receiver ) const
	{ 
		TMapPrefetchReceiver::const_iterator it_found = m_prefetchReceiver.find( std::make_pair( _pakName, _filePath ) );

		if( it_found == m_prefetchReceiver.end() )
		{
			return false;
		}

		const PrefetchReceiver & receiver = it_found->second;

		*_receiver = const_cast<PrefetchReceiver *>(&receiver);

		return true;
	}
}