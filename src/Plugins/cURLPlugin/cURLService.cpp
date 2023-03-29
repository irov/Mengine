#include "cURLService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "cURLGetMessageThreadTask.h"
#include "cURLPostMessageThreadTask.h"
#include "cURLHeaderDataThreadTask.h"
#include "cURLGetAssetThreadTask.h"
#include "cURLDeleteMessageThreadTask.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
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
#include "Kernel/FileGroupHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/TimeHelper.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

#include "curl/curl.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( cURLService, Mengine::cURLService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * curl_malloc( size_t _size )
        {
            void * p = Helper::allocateMemory( _size, "curl" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void curl_free( void * _ptr )
        {
            Helper::deallocateMemory( _ptr, "curl" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void * curl_realloc( void * _ptr, size_t _size )
        {
            void * p = Helper::reallocateMemory( _ptr, _size, "curl" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static char * curl_strdup( const char * str )
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
        static void * curl_calloc( size_t _nmemb, size_t _size )
        {
            void * p = Helper::callocateMemory( _nmemb, _size, "curl" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
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
            , &Detail::curl_malloc
            , &Detail::curl_free
            , &Detail::curl_realloc
            , &Detail::curl_strdup
            , &Detail::curl_calloc );

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

        LOGGER_INFO( "curl", "cURL version: %s host: %s ssl: %s"
            , info->version
            , info->host
            , info->ssl_version
        );

        uint32_t cURLService_ThreadCount = CONFIG_VALUE( "cURLPlugin", "ThreadCount", 4 );

        ThreadQueueInterfacePtr threadQueue = THREAD_SERVICE()
            ->createTaskQueue( 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadQueue, "invalid create task queue" );

        m_threadQueue = threadQueue;

        for( uint32_t index = 0; index != cURLService_ThreadCount; ++index )
        {
            Stringstream ss;
            ss << "ThreadcURLService_" << index;
            ConstString threadName = Helper::stringizeString( ss.str() );

            if( THREAD_SERVICE()
                ->createThreadProcessor( threadName, ETP_BELOW_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }

            m_threads.emplace_back( threadName );

            m_threadQueue->addThread( threadName );
        }

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        m_factoryResponse = Helper::makeFactoryPool<cURLResponse, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskGetMessage = Helper::makeFactoryPool<cURLGetMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskPostMessage = Helper::makeFactoryPool<cURLPostMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDeleteMessage = Helper::makeFactoryPool<cURLDeleteMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskHeaderData = Helper::makeFactoryPool<cURLHeaderDataThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDownloadAsset = Helper::makeFactoryPool<cURLGetAssetThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &cURLService::notifyEnginePrepareFinalize_, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_USE_SCRIPT_SERVICE)
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
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );

#if defined(MENGINE_DEBUG)
        for( const RequestListenerDesk & desc : m_networkListeners )
        {
            LOGGER_ERROR( "Forgot remove listener '%u' from cURLService (doc: %s)"
                , desc.id
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_mutex = nullptr;

        m_receiverDescs.clear();
        m_networkListeners.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryResponse );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDownloadAsset );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskPostMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDeleteMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskHeaderData );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskGetMessage );

        m_factoryResponse = nullptr;
        m_factoryTaskDownloadAsset = nullptr;
        m_factoryTaskPostMessage = nullptr;
        m_factoryTaskDeleteMessage = nullptr;
        m_factoryTaskHeaderData = nullptr;
        m_factoryTaskGetMessage = nullptr;

        curl_global_cleanup();
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::notifyEnginePrepareFinalize_()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ReceiverDesc & desc : m_receiverDescs )
        {
            const ThreadTaskInterfacePtr & task = desc.task;

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
                ->destroyThreadProcessor( threadName );
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
    HttpRequestID cURLService::getMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLGetMessageThreadTaskPtr task = m_factoryTaskGetMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        cURLResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );
        
        task->setReponse( response );

        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );

        if( task->initialize() == false )
        {
            LOGGER_ERROR( "url '%s' invalid initialize task"
                , _url.c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_GET_MESSAGE;
        desc.timestamp = Helper::getTimeMilliseconds();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        for( const RequestListenerDesk & listenerDesc : m_networkListeners )
        {
            listenerDesc.listener->onHttpRequest( task_id, _url );
        }

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::postMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLPostMessageThreadTaskPtr task = m_factoryTaskPostMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        cURLResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );

        if( task->initialize( _params ) == false )
        {
            LOGGER_ERROR( "url '%s' invalid initialize task"
                , _url.c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_POST_MESSAGE;
        desc.timestamp = Helper::getTimeMilliseconds();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        for( const RequestListenerDesk & listenerDesc : m_networkListeners )
        {
            listenerDesc.listener->onHttpRequest( task_id, _url );
        }

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::deleteMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLDeleteMessageThreadTaskPtr task = m_factoryTaskDeleteMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        cURLResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );

        if( task->initialize() == false )
        {
            LOGGER_ERROR( "url '%s' invalid initialize task"
                , _url.c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_POST_MESSAGE;
        desc.timestamp = Helper::getTimeMilliseconds();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        for( const RequestListenerDesk & listenerDesc : m_networkListeners )
        {
            listenerDesc.listener->onHttpRequest( task_id, _url );
        }

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::headerData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLHeaderDataThreadTaskPtr task = m_factoryTaskHeaderData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        cURLResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );

        if( task->initialize( _data ) == false )
        {
            LOGGER_ERROR( "url '%s' invalid initialize task"
                , _url.c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_HEADER_DATA;
        desc.timestamp = Helper::getTimeMilliseconds();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
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

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", _filePath.c_str() );

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLGetAssetThreadTaskPtr task = m_factoryTaskDownloadAsset->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );

        cURLResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( cURLReceiverInterfacePtr::from( this ) );

        if( task->initialize( _login, _password, _fileGroup, _filePath, filePathTmp ) == false )
        {
            LOGGER_ERROR( "url '%s' file '%s' invalid initialize task"
                , _url.c_str()
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_DOWNLOAD_ASSET;
        desc.timestamp = Helper::getTimeMilliseconds();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

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

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

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

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_networkListeners.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::removeRequestListener( int32_t _id )
    {
        m_networkListeners.erase( Algorithm::remove_if( m_networkListeners.begin(), m_networkListeners.end(),
            [_id]( const RequestListenerDesk & _desc )
        {
            return _desc.id == _id;
        } ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::onHttpRequestComplete( const cURLResponseInterfacePtr & _response )
    {
        if( this->isStopService() == true )
        {
            return;
        }

        for( const RequestListenerDesk & listenerDesc : m_networkListeners )
        {
            listenerDesc.listener->onHttpResponse( _response );
        }

        m_mutex->lock();

        for( VectorReceiverDesc::iterator
            it = m_receiverDescs.begin(),
            it_end = m_receiverDescs.end();
            it != it_end;
            ++it )
        {
            ReceiverDesc & desc = *it;

            HttpRequestID requestId = _response->getRequestId();

            if( desc.id != requestId )
            {
                continue;
            }

            cURLReceiverInterfacePtr receiver = desc.receiver;

            *it = m_receiverDescs.back();
            m_receiverDescs.pop_back();

            m_mutex->unlock();

            if( receiver != nullptr )
            {
                receiver->onHttpRequestComplete( _response );
            }

            return;
        }

        m_mutex->unlock();

        LOGGER_ERROR( "invalid request '%u' %s (status [%u] error '%.2048s' response '%.2048s' code [%u])"
            , _response->getRequestId()
            , _response->isSuccessful() ? "complete" : "failure"
            , _response->getStatus()
            , _response->getError().c_str()
            , _response->getData().c_str()
            , _response->getCode()            
        );
    }
    //////////////////////////////////////////////////////////////////////////
}