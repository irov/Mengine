#include "PrefetcherService.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "ThreadTaskPrefetchImageDecoder.h"
#include "ThreadTaskPrefetchSoundDecoder.h"
#include "ThreadTaskPrefetchDataflow.h"
#include "ThreadTaskPrefetchStream.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Config/Stringstream.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PrefetcherService, Mengine::PrefetcherService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrefetcherService::PrefetcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrefetcherService::~PrefetcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::_availableService() const
    {
        bool available = CONFIG_VALUE( "Engine", "PrefetcherAvailable", true );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::_initializeService()
    {
        uint32_t PrefetcherServiceThreadCount = CONFIG_VALUE( "PrefetcherService", "ThreadCount", 2 );
        uint32_t PrefetcherServicePacketSize = CONFIG_VALUE( "PrefetcherService", "PacketSize", 64 );

        m_threadQueue = THREAD_SERVICE()
            ->runTaskQueue( PrefetcherServicePacketSize );

        for( uint32_t index = 0; index != PrefetcherServiceThreadCount; ++index )
        {
            Stringstream ss;
            ss << "ThreadPrefetcherService_" << index;
            ConstString threadName = Helper::stringizeString( ss.str() );

            THREAD_SERVICE()
                ->createThread( threadName, -1, MENGINE_DOCUMENT_FUNCTION );

            m_threads.emplace_back( threadName );

            m_threadQueue->addThread( threadName );
        }

        m_factoryThreadTaskPrefetchImageDecoder = new FactoryPool<ThreadTaskPrefetchImageDecoder, 16>();
        m_factoryThreadTaskPrefetchSoundDecoder = new FactoryPool<ThreadTaskPrefetchSoundDecoder, 16>();
        m_factoryThreadTaskPrefetchDataflow = new FactoryPool<ThreadTaskPrefetchDataflow, 16>();
        m_factoryThreadTaskPrefetchStream = new FactoryPool<ThreadTaskPrefetchStream, 16>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetcherService::_finalizeService()
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        for( const ConstString & threadName : m_threads )
        {
            THREAD_SERVICE()
                ->destroyThread( threadName );
        }

        m_threads.clear();

        if( m_threadQueue != nullptr )
        {
            THREAD_SERVICE()
                ->cancelTaskQueue( m_threadQueue );

            m_threadQueue = nullptr;
        }

        m_prefetchReceiver.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadTaskPrefetchImageDecoder );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadTaskPrefetchSoundDecoder );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadTaskPrefetchDataflow );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadTaskPrefetchStream );

        m_factoryThreadTaskPrefetchImageDecoder = nullptr;
        m_factoryThreadTaskPrefetchSoundDecoder = nullptr;
        m_factoryThreadTaskPrefetchDataflow = nullptr;
        m_factoryThreadTaskPrefetchStream = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetcherService::update()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::prefetchImageDecoder( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer )
    {
        if( this->isAvailableService() == false )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        if( m_threadQueue == nullptr )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        PrefetchReceiver * receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == true )
        {
            ++receiver->refcount;

            _observer->onPrefetchAlreadyExist();

            return true;
        }

        PrefetchReceiver new_receiver;

        new_receiver.refcount = 1;

		ThreadTaskPrefetchImageDecoderPtr task = m_factoryThreadTaskPrefetchImageDecoder->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, false );

        task->initialize( _fileGroup, _filePath, _observer );
        task->setImageCodec( _codecType );

        new_receiver.prefetcher = task;

        m_prefetchReceiver.emplace( std::make_pair( _fileGroup->getName(), _filePath ), new_receiver );

        m_threadQueue->addTask( task );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getImageDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ImageDecoderInterfacePtr & _decoder )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiver * receiver;
        if( this->getPrefetch_( _fileGroup, _filePath, &receiver ) == false )
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
    bool PrefetcherService::prefetchSoundDecoder( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer )
    {
        if( this->isAvailableService() == false )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        if( m_threadQueue == nullptr )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        PrefetchReceiver * receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == true )
        {
            ++receiver->refcount;

            _observer->onPrefetchAlreadyExist();

            return true;
        }

        PrefetchReceiver new_receiver;

        new_receiver.refcount = 1;

        ThreadTaskPrefetchSoundDecoderPtr task = m_factoryThreadTaskPrefetchSoundDecoder->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, false );

        task->initialize( _fileGroup, _filePath, _observer );
        task->setSoundCodec( _codecType );

        new_receiver.prefetcher = task;

        m_prefetchReceiver.emplace( std::make_pair( _fileGroup->getName(), _filePath ), new_receiver );

        m_threadQueue->addTask( task );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getSoundDecoder( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, SoundDecoderInterfacePtr & _decoder )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiver * receiver;
        if( this->getPrefetch_( _fileGroup, _filePath, &receiver ) == false )
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
    bool PrefetcherService::prefetchData( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const PrefetcherObserverInterfacePtr & _observer )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dataflow, false )("PrefetcherService::prefetchData: '%s':'%s' invalide dataflow is nullptr"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            );

        if( this->isAvailableService() == false )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        if( m_threadQueue == nullptr )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        PrefetchReceiver * receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == true )
        {
            ++receiver->refcount;

            if( _observer == nullptr )
            _observer->onPrefetchAlreadyExist();

            return true;
        }

        PrefetchReceiver new_receiver;

        new_receiver.refcount = 1;

        ThreadTaskPrefetchDataflowPtr task = m_factoryThreadTaskPrefetchDataflow->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, false );

        task->initialize( _fileGroup, _filePath, _observer );

        task->setDataflow( _dataflow );

        new_receiver.prefetcher = task;

        m_prefetchReceiver.emplace( std::make_pair( _fileGroup->getName(), _filePath ), new_receiver );

        m_threadQueue->addTask( task );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////			
    bool PrefetcherService::getData( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, DataInterfacePtr & _data )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiver * receiver;
        if( this->getPrefetch_( _fileGroup, _filePath, &receiver ) == false )
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
    bool PrefetcherService::popData( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, DataInterfacePtr & _data )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiver * receiver;
        if( this->popPrefetch_( _fileGroup, _filePath, &receiver ) == false )
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
    bool PrefetcherService::prefetchStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ArchivatorInterfacePtr & _archivator, uint32_t _magicNumber, uint32_t _magicVersion, const PrefetcherObserverInterfacePtr & _observer )
    {
        if( this->isAvailableService() == false )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        if( m_threadQueue == nullptr )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        PrefetchReceiver * receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == true )
        {
            ++receiver->refcount;

            _observer->onPrefetchAlreadyExist();

            return true;
        }

        PrefetchReceiver new_receiver;

        new_receiver.refcount = 1;

        ThreadTaskPrefetchStreamPtr task = m_factoryThreadTaskPrefetchStream->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, false );

        task->initialize( _fileGroup, _filePath, _observer );

        task->setArchivator( _archivator );
        task->setMagicNumber( _magicNumber );
        task->setMagicVersion( _magicVersion );

        new_receiver.prefetcher = task;

        m_prefetchReceiver.emplace( std::make_pair( _fileGroup->getName(), _filePath ), new_receiver );

        m_threadQueue->addTask( task );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, MemoryInterfacePtr & _memory )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiver * receiver;
        if( this->getPrefetch_( _fileGroup, _filePath, &receiver ) == false )
        {
            return false;
        }

        ThreadTaskPrefetchStreamPtr prefetcherStream = stdex::intrusive_static_cast<ThreadTaskPrefetchStreamPtr>(receiver->prefetcher);

        const MemoryInterfacePtr & prefetch_memory = prefetcherStream->getMemory();

        if( prefetch_memory == nullptr )
        {
            return false;
        }

        _memory = prefetch_memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::unfetch( const FileGroupInterfacePtr& _fileGroup, const FilePath& _filePath )
    {
        MapPrefetchReceiver::iterator it_found = m_prefetchReceiver.find( std::make_pair( _fileGroup->getName(), _filePath ) );

        if( it_found == m_prefetchReceiver.end() )
        {
            return false;
        }

        PrefetchReceiver & receiver = it_found->second;

        if( --receiver.refcount > 0 )
        {
            return true;
        }

        receiver.prefetcher->cancel();

        m_prefetchReceiver.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetcherService::visitPrefetches( const VisitorPtr & _visitor ) const
    {
        for( MapPrefetchReceiver::const_iterator
            it = m_prefetchReceiver.begin(),
            it_end = m_prefetchReceiver.end();
            it != it_end;
            ++it )
        {
            const PrefetchReceiver & receiver = it->second;

            const ThreadTaskPrefetchPtr & prefetcher = receiver.prefetcher;

            prefetcher->visit( _visitor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::hasPrefetch_( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, PrefetchReceiver ** _receiver ) const
    {
        MapPrefetchReceiver::const_iterator it_found = m_prefetchReceiver.find( std::make_pair( _fileGroup->getName(), _filePath ) );

        if( it_found == m_prefetchReceiver.end() )
        {
            return false;
        }

        const PrefetchReceiver & receiver = it_found->second;

        *_receiver = const_cast<PrefetchReceiver *>(&receiver);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getPrefetch_( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, PrefetchReceiver ** _receiver ) const
    {
        PrefetchReceiver * receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == false )
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

        *_receiver = receiver;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::popPrefetch_( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, PrefetchReceiver ** _receiver )
    {
        MapPrefetchReceiver::iterator it_found = m_prefetchReceiver.find( std::make_pair( _fileGroup->getName(), _filePath ) );

        if( it_found == m_prefetchReceiver.end() )
        {
            return false;
        }

        PrefetchReceiver & receiver = it_found->second;

        if( receiver.prefetcher->isComplete() == false ||
            receiver.prefetcher->isSuccessful() == false )
        {
            --receiver.refcount;

            receiver.prefetcher->cancel();

            m_prefetchReceiver.erase( it_found );

            return false;
        }

        *_receiver = &receiver;

        return true;
    }
}