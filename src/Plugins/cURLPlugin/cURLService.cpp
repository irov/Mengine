#include "cURLService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "cURLGetMessageThreadTask.h"
#include "cURLPostMessageThreadTask.h"
#include "cURLHeaderDataThreadTask.h"
#include "cURLGetAssetThreadTask.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/ProfilerHelper.h"

#include "Config/StdString.h"

#include "curl/curl.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( cURLService, Mengine::cURLService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void * stdex_curl_malloc_callback( size_t _size )
    {
        void * p = Helper::allocateMemory( _size, "curl" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_curl_free_callback( void * _ptr )
    {
        Helper::deallocateMemory( _ptr, "curl" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void * stdex_curl_realloc_callback( void * _ptr, size_t _size )
    {
        void * p = Helper::reallocateMemory( _ptr, _size, "curl" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static char * stdex_curl_strdup_callback( const char * str )
    {
        size_t len = MENGINE_STRLEN( str ) + 1;
        
        void * p = Helper::allocateMemory( len, "curl" );

        if( p == nullptr )
        {
            return nullptr;
        }

        return (char *)MENGINE_MEMCPY( p, str, len );
    }
    //////////////////////////////////////////////////////////////////////////
    static void * stdex_curl_calloc_callback( size_t _nmemb, size_t _size )
    { 
        void * p = Helper::callocateMemory( _nmemb, _size, "curl" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    cURLService::cURLService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLService::~cURLService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::_dependencyService()
    {
        SERVICE_DEPENDENCY( cURLService, ThreadServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLService::_initializeService()
    {
        CURLcode code = curl_global_init_mem( CURL_GLOBAL_ALL
            , &stdex_curl_malloc_callback
            , &stdex_curl_free_callback
            , &stdex_curl_realloc_callback
            , &stdex_curl_strdup_callback
            , &stdex_curl_calloc_callback );

        if( code != CURLE_OK )
        {
            LOGGER_ERROR( "invalid initialize curl '%s' [%u]"
                , curl_easy_strerror( code )
                , code
            );

            return false;
        }

        uint32_t cURLServiceThreadCount = CONFIG_VALUE( "cURLService", "ThreadCount", 4 );

        ThreadQueueInterfacePtr threadQueue = THREAD_SERVICE()
            ->createTaskQueue( 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadQueue, "invalid create task queue" );

        m_threadQueue = threadQueue;

        for( uint32_t index = 0; index != cURLServiceThreadCount; ++index )
        {
            Stringstream ss;
            ss << "ThreadcURLService_" << index;
            ConstString threadName = Helper::stringizeString( ss.str() );

            THREAD_SERVICE()
                ->createThread( threadName, MENGINE_THREAD_PRIORITY_BELOW_NORMAL, MENGINE_DOCUMENT_FACTORABLE );

            m_threads.emplace_back( threadName );

            m_threadQueue->addThread( threadName );
        }

        m_factoryTaskGetMessage = Helper::makeFactoryPool<cURLGetMessageThreadTask, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskPostMessage = Helper::makeFactoryPool<cURLPostMessageThreadTask, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskHeaderData = Helper::makeFactoryPool<cURLHeaderDataThreadTask, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDownloadAsset = Helper::makeFactoryPool<cURLGetAssetThreadTask, 8>( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &cURLService::notifyEnginePrepareFinalize_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDownloadAsset );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskPostMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskHeaderData );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskGetMessage );

        m_factoryTaskDownloadAsset = nullptr;
        m_factoryTaskPostMessage = nullptr;
        m_factoryTaskHeaderData = nullptr;
        m_factoryTaskGetMessage = nullptr;

#ifdef MENGINE_DEBUG
        for( const RequestListenerDesk & desc : m_networkListeners )
        {
            LOGGER_ERROR( "Forgot remove listener '%u' from cURLService (doc: %s)"
                , desc.id
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        curl_global_cleanup();
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::notifyEnginePrepareFinalize_()
    {
        for( const ReceiverDesc & desc : m_receiverDescs )
        {
            const ThreadTaskPtr & task = desc.task;

            THREAD_SERVICE()
                ->joinTask( task );
        }

        m_receiverDescs.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::_stopService()
    {
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
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::getMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLReceiverInterfacePtr & _receiver )
    {
        MENGINE_PROFILER_CATEGORY();

        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        UniqueId task_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        cURLGetMessageThreadTaskPtr task = m_factoryTaskGetMessage->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );
        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );
        
        if( task->initialize() == false )
        {
            return 0;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        m_threadQueue->addTask( task );

        for( const RequestListenerDesk & listenerDesc : m_networkListeners )
        {
            listenerDesc.listener->request( task_id, _url );
        }

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::postMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLReceiverInterfacePtr & _receiver )
    {
        MENGINE_PROFILER_CATEGORY();

        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        UniqueId task_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        cURLPostMessageThreadTaskPtr task = m_factoryTaskPostMessage->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );
        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );
        
        if( task->initialize( _params ) == false )
        {
            return 0;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        m_threadQueue->addTask( task );

        for( const RequestListenerDesk & listenerDesc : m_networkListeners )
        {
            listenerDesc.listener->request( task_id, _url );
        }

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::headerData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLReceiverInterfacePtr & _receiver )
    {
        MENGINE_PROFILER_CATEGORY();

        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        UniqueId task_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        cURLHeaderDataThreadTaskPtr task = m_factoryTaskHeaderData->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );
        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );
        
        if( task->initialize( _data ) == false )
        {
            return 0;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        m_threadQueue->addTask( task );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver )
    {
        MENGINE_PROFILER_CATEGORY();

        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        if( _fileGroup->existFile( _filePath, true ) == true )
        {
            LOGGER_ERROR( "url '%s' category '%s' file already exist '%s'"
                , _url.c_str()
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return 0;
        }

        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", _filePath.c_str() );

        UniqueId task_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        cURLGetAssetThreadTaskPtr task = m_factoryTaskDownloadAsset->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );
        
        if( task->initialize( _login, _password, _fileGroup, _filePath, filePathTmp ) == false )
        {
            LOGGER_ERROR( "url '%s' category '%s' file '%s' invalid initialize task"
                , _url.c_str()
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return 0;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        m_threadQueue->addTask( task );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLService::cancelRequest( HttpRequestID _id )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return false;
        }

        for( ReceiverDesc & desc : m_receiverDescs )
        {
            if( desc.id != _id )
            {
                continue;
            }

            desc.task->cancel();

            return true;
        }

        LOGGER_ERROR( "request [%u] not found"
            , _id
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t cURLService::addRequestListener( const cURLRequestListenerInterfacePtr & _listener, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        RequestListenerDesk desc;
        desc.id = id;
        desc.listener = _listener;

#if MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

        m_networkListeners.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::removeRequestListener( int32_t _id )
    {
        m_networkListeners.erase( std::remove_if( m_networkListeners.begin(), m_networkListeners.end(), 
            [_id]( const RequestListenerDesk  & _desc )
        {
            return _desc.id == _id;
        } ));
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful )
    {
        for( const RequestListenerDesk & listenerDesc : m_networkListeners )
        {
            listenerDesc.listener->response( _id, _response );
        }

        for( VectorReceiverDesc::iterator
            it = m_receiverDescs.begin(),
            it_end = m_receiverDescs.end();
            it != it_end;
            ++it )
        {
            ReceiverDesc & desc = *it;

            if( desc.id != _id )
            {
                continue;
            }

            cURLReceiverInterfacePtr receiver = desc.receiver;

            *it = m_receiverDescs.back();
            m_receiverDescs.pop_back();

            if( receiver != nullptr )
            {
                receiver->onHttpRequestComplete( _id, _status, _error, _headers, _response, _code, _successful );
            }

            return;
        }

        LOGGER_ERROR( "invalid request '%u' complete (status [%u] error '%.2048s' response '%.2048s' code [%u] successful [%u])"
            , _id
            , _status
            , _error.c_str()
            , _response.c_str()
            , _code
            , _successful
        );
    }
    //////////////////////////////////////////////////////////////////////////
}