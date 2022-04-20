#include "cURLService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "cURLGetMessageThreadTask.h"
#include "cURLPostMessageThreadTask.h"
#include "cURLHeaderDataThreadTask.h"
#include "cURLGetAssetThreadTask.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"
#   include "cURLScriptEmbedding.h"
#endif

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ConfigHelper.h"
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
#include "Kernel/FileGroupHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

#include "curl/curl.h"

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

        curl_version_info_data * info = curl_version_info( CURLVERSION_NOW );

        MENGINE_UNUSED( info );

        LOGGER_MESSAGE( "cURL version: %s host: %s ssl: %s"
            , info->version
            , info->host
            , info->ssl_version
        );

        uint32_t cURLService_ThreadCount = CONFIG_VALUE( "cURLService", "ThreadCount", 4 );

        ThreadQueueInterfacePtr threadQueue = THREAD_SERVICE()
            ->createTaskQueue( 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadQueue, "invalid create task queue" );

        m_threadQueue = threadQueue;

        for( uint32_t index = 0; index != cURLService_ThreadCount; ++index )
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

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "cURLScriptEmbedding" ), Helper::makeFactorableUnique<cURLScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "cURLScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::_finalizeService()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

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

        UniqueId task_id = Helper::generateUniqueIdentity();

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

        UniqueId task_id = Helper::generateUniqueIdentity();

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

        UniqueId task_id = Helper::generateUniqueIdentity();

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
            LOGGER_ERROR( "url '%s' file already exist '%s'"
                , _url.c_str()
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            return 0;
        }

        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", _filePath.c_str() );

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLGetAssetThreadTaskPtr task = m_factoryTaskDownloadAsset->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );
        
        if( task->initialize( _login, _password, _fileGroup, _filePath, filePathTmp ) == false )
        {
            LOGGER_ERROR( "url '%s' file '%s' invalid initialize task"
                , _url.c_str()
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
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

        UniqueId id = Helper::generateUniqueIdentity();

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
        m_networkListeners.erase( Algorithm::remove_if( m_networkListeners.begin(), m_networkListeners.end(),
            [_id]( const RequestListenerDesk  & _desc )
        {
            return _desc.id == _id;
        } ));
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::onHttpRequestComplete( const cURLResponseData & _response )
    {
        if( this->isStopService() == true )
        {
            return;
        }

        for( const RequestListenerDesk & listenerDesc : m_networkListeners )
        {
            listenerDesc.listener->response( _response.id, _response.data );
        }

        for( VectorReceiverDesc::iterator
            it = m_receiverDescs.begin(),
            it_end = m_receiverDescs.end();
            it != it_end;
            ++it )
        {
            ReceiverDesc & desc = *it;

            if( desc.id != _response.id )
            {
                continue;
            }

            cURLReceiverInterfacePtr receiver = desc.receiver;

            *it = m_receiverDescs.back();
            m_receiverDescs.pop_back();

            if( receiver != nullptr )
            {
                receiver->onHttpRequestComplete( _response );
            }

            return;
        }

        LOGGER_ERROR( "invalid request '%u' complete (status [%u] error '%.2048s' response '%.2048s' code [%u] successful [%u])"
            , _response.id
            , _response.status
            , _response.error.c_str()
            , _response.data.c_str()
            , _response.code
            , _response.successful
        );
    }
    //////////////////////////////////////////////////////////////////////////
}