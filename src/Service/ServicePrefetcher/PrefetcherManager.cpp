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
				->createThread( CONST_STRING_LOCAL(m_serviceProvider, "ThreadPrefetcherManager"), -1 );
		}

		m_threadQueue = THREAD_SERVICE(m_serviceProvider)
			->runTaskQueue( CONST_STRING_LOCAL(m_serviceProvider, "ThreadPrefetcherManager"), MENGINE_PREFETCHER_THREAD_COUNT, MENGINE_PREFETCHER_PACKET_SIZE );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::finalize()
	{
		for( TMapPrefetchImageDecoderReceiver::iterator
			it = m_prefetchImageDecoderReceiver.begin(),
			it_end = m_prefetchImageDecoderReceiver.end();
		it != it_end;
		++it )
		{
			PrefetchImageDecoderReceiver & receiver = m_prefetchImageDecoderReceiver.get_value( it );

			THREAD_SERVICE(m_serviceProvider)
				->joinTask( receiver.prefetcher );
		}

		m_prefetchImageDecoderReceiver.clear();

		for( TMapPrefetchDataReceiver::iterator
			it = m_prefetchDataReceiver.begin(),
			it_end = m_prefetchDataReceiver.end();
		it != it_end;
		++it )
		{
			PrefetchDataReceiver & receiver = m_prefetchDataReceiver.get_value( it );

			THREAD_SERVICE(m_serviceProvider)
				->joinTask( receiver.prefetcher );
		}

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

		TMapPrefetchImageDecoderReceiver::iterator it_found = m_prefetchImageDecoderReceiver.find( _fileName );

		if( it_found == m_prefetchImageDecoderReceiver.end() )
		{
			PrefetchImageDecoderReceiver receiver;
			receiver.refcount = 1;
						
			ThreadTaskPrefetchImageDecoderPtr task = m_factoryThreadTaskPrefetchImageDecoder.createObjectT();
			task->setServiceProvider( m_serviceProvider );
			task->initialize( _pakName, _fileName, _codec );

			receiver.prefetcher = task;

			m_prefetchImageDecoderReceiver.insert( _fileName, receiver );

			m_threadQueue->addTask( task );

			return true;
		}

		PrefetchImageDecoderReceiver & receiver = m_prefetchImageDecoderReceiver.get_value( it_found );

		++receiver.refcount;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::unfetchImageDecoder( const FilePath& _fileName )
	{
		TMapPrefetchImageDecoderReceiver::iterator it_found = m_prefetchImageDecoderReceiver.find( _fileName );

		if( it_found == m_prefetchImageDecoderReceiver.end() )
		{
			return;
		}

		PrefetchImageDecoderReceiver & receiver = m_prefetchImageDecoderReceiver.get_value( it_found );

		if( --receiver.refcount > 0 )
		{
			return;
		}

		receiver.prefetcher->cancel();

		m_prefetchImageDecoderReceiver.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::getImageDecoder( const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const
	{
		TMapPrefetchImageDecoderReceiver::const_iterator it_found = m_prefetchImageDecoderReceiver.find( _fileName );

		if( it_found == m_prefetchImageDecoderReceiver.end() )
		{
			return false;
		}

		const PrefetchImageDecoderReceiver & receiver = m_prefetchImageDecoderReceiver.get_value( it_found );

		if( receiver.prefetcher->isComplete() == false )
		{
			return false;
		}

		if( receiver.prefetcher->isSuccessful() == false )
		{
			return false;
		}

		const ImageDecoderInterfacePtr & prefetch_decoder = receiver.prefetcher->getDecoder();

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

		TMapPrefetchSoundDecoderReceiver::iterator it_found = m_prefetchSoundDecoderReceiver.find( _fileName );

		if( it_found == m_prefetchSoundDecoderReceiver.end() )
		{
			PrefetchSoundDecoderReceiver receiver;
			receiver.refcount = 1;

			ThreadTaskPrefetchSoundDecoderPtr task = m_factoryThreadTaskPrefetchSoundDecoder.createObjectT();
			task->setServiceProvider( m_serviceProvider );
			task->initialize( _pakName, _fileName, _codec );

			receiver.prefetcher = task;

			m_prefetchSoundDecoderReceiver.insert( _fileName, receiver );

			m_threadQueue->addTask( task );

			return true;
		}

		PrefetchSoundDecoderReceiver & receiver = m_prefetchSoundDecoderReceiver.get_value( it_found );

		++receiver.refcount;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::unfetchSoundDecoder( const FilePath& _fileName )
	{
		TMapPrefetchSoundDecoderReceiver::iterator it_found = m_prefetchSoundDecoderReceiver.find( _fileName );

		if( it_found == m_prefetchSoundDecoderReceiver.end() )
		{
			return;
		}

		PrefetchSoundDecoderReceiver & receiver = m_prefetchSoundDecoderReceiver.get_value( it_found );

		if( --receiver.refcount > 0 )
		{
			return;
		}

		receiver.prefetcher->cancel();

		m_prefetchSoundDecoderReceiver.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	bool PrefetcherManager::getSoundDecoder( const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const
	{
		TMapPrefetchSoundDecoderReceiver::const_iterator it_found = m_prefetchSoundDecoderReceiver.find( _fileName );

		if( it_found == m_prefetchSoundDecoderReceiver.end() )
		{
			return false;
		}

		const PrefetchSoundDecoderReceiver & receiver = m_prefetchSoundDecoderReceiver.get_value( it_found );

		if( receiver.prefetcher->isComplete() == false )
		{
			return false;
		}

		if( receiver.prefetcher->isSuccessful() == false )
		{
			return false;
		}

		const SoundDecoderInterfacePtr & prefetch_decoder = receiver.prefetcher->getDecoder();

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

		TMapPrefetchDataReceiver::iterator it_found = m_prefetchDataReceiver.find( _fileName );

		if( it_found == m_prefetchDataReceiver.end() )
		{
			PrefetchDataReceiver receiver;
			receiver.refcount = 1;

			ThreadTaskPrefetchDataflowPtr task = m_factoryThreadTaskPrefetchDataflow.createObjectT();
			task->setServiceProvider( m_serviceProvider );
			task->initialize( _pakName, _fileName, _dataflowType );

			receiver.prefetcher = task;

			m_prefetchDataReceiver.insert( _fileName, receiver );

			m_threadQueue->addTask( task );

			return true;
		}

		PrefetchDataReceiver & receiver = m_prefetchDataReceiver.get_value( it_found );

		++receiver.refcount;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PrefetcherManager::unfetchData( const FilePath& _fileName )
	{
		TMapPrefetchDataReceiver::iterator it_found = m_prefetchDataReceiver.find( _fileName );

		if( it_found == m_prefetchDataReceiver.end() )
		{
			return;
		}

		PrefetchDataReceiver & receiver = m_prefetchDataReceiver.get_value( it_found );

		if( --receiver.refcount > 0 )
		{
			return;
		}

		receiver.prefetcher->cancel();

		m_prefetchDataReceiver.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////			
	bool PrefetcherManager::getData( const FilePath & _fileName, DataInterfacePtr & _data ) const
	{
		TMapPrefetchDataReceiver::const_iterator it_found = m_prefetchDataReceiver.find( _fileName );

		if( it_found == m_prefetchDataReceiver.end() )
		{
			return false;
		}

		const PrefetchDataReceiver & receiver = m_prefetchDataReceiver.get_value( it_found );

		if( receiver.prefetcher->isComplete() == false )
		{
			return false;
		}

		if( receiver.prefetcher->isSuccessful() == false )
		{
			return false;
		}

		const DataInterfacePtr & prefetch_data = receiver.prefetcher->getData();

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
		info.dataCount = 0;

		for( TMapPrefetchImageDecoderReceiver::const_iterator
			it = m_prefetchImageDecoderReceiver.begin(),
			it_end = m_prefetchImageDecoderReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchImageDecoderReceiver & receiver = m_prefetchImageDecoderReceiver.get_value( it );

			if( receiver.prefetcher->isComplete() == false )
			{
				continue;
			}

			++info.decoderCount;
		}

		for( TMapPrefetchDataReceiver::const_iterator
			it = m_prefetchDataReceiver.begin(),
			it_end = m_prefetchDataReceiver.end();
		it != it_end;
		++it )
		{
			const PrefetchDataReceiver & receiver = m_prefetchDataReceiver.get_value( it );

			if( receiver.prefetcher->isComplete() == false )
			{
				continue;
			}

			++info.dataCount;
		}

		return info;
	}
}