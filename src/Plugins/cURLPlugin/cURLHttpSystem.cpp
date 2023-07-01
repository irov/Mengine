#include "cURLHttpSystem.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "cURLHttpRequestGetMessageThreadTask.h"
#include "cURLHttpRequestPostMessageThreadTask.h"
#include "cURLHttpRequestHeaderDataThreadTask.h"
#include "cURLHttpRequestGetAssetThreadTask.h"
#include "cURLHttpRequestDeleteMessageThreadTask.h"

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
#include "Kernel/TimestampHelper.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

#include "curl/curl.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::cURLHttpSystem );
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
    cURLHttpSystem::cURLHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpSystem::~cURLHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpSystem::_dependencyService()
    {
        SERVICE_DEPENDENCY( cURLHttpSystem, ThreadServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpSystem::_initializeService()
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

        uint32_t cURLService_ThreadCount = CONFIG_VALUE( "cURLPlugin", "ThreadCount", 8 );

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

        m_factoryResponse = Helper::makeFactoryPool<cURLHttpResponse, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskGetMessage = Helper::makeFactoryPool<cURLHttpRequestGetMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskPostMessage = Helper::makeFactoryPool<cURLHttpRequestPostMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDeleteMessage = Helper::makeFactoryPool<cURLHttpRequestDeleteMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskHeaderData = Helper::makeFactoryPool<cURLHttpRequestHeaderDataThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDownloadAsset = Helper::makeFactoryPool<cURLHttpRequestGetAssetThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &cURLHttpSystem::notifyEnginePrepareFinalize_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpSystem::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );

        m_mutex = nullptr;

        m_receiverDescs.clear();

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
    void cURLHttpSystem::notifyEnginePrepareFinalize_()
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
    void cURLHttpSystem::_stopService()
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
    HttpRequestId cURLHttpSystem::getMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLHttpRequestGetMessageThreadTaskPtr task = m_factoryTaskGetMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        cURLHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );
        
        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

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
        desc.timestamp = Helper::getTimestamp();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, task_id, _url );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId cURLHttpSystem::postMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpRequestPostParams & _params, const HttpReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLHttpRequestPostMessageThreadTaskPtr task = m_factoryTaskPostMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        cURLHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

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
        desc.timestamp = Helper::getTimestamp();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, task_id, _url );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId cURLHttpSystem::deleteMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLHttpRequestDeleteMessageThreadTaskPtr task = m_factoryTaskDeleteMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        cURLHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

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
        desc.timestamp = Helper::getTimestamp();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, task_id, _url );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId cURLHttpSystem::headerData( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const HttpReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        cURLHttpRequestHeaderDataThreadTaskPtr task = m_factoryTaskHeaderData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        cURLHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

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
        desc.timestamp = Helper::getTimestamp();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, task_id, _url );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId cURLHttpSystem::downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const HttpReceiverInterfacePtr & _receiver, const DocumentPtr & _doc )
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

        cURLHttpRequestGetAssetThreadTaskPtr task = m_factoryTaskDownloadAsset->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );

        cURLHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

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
        desc.timestamp = Helper::getTimestamp();
        desc.task = task;
        desc.receiver = _receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        m_threadQueue->addTask( task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, task_id, _url );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpSystem::cancelRequest( HttpRequestId _id )
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

            NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_CANCEL, _id );

            desc.task->cancel();

            return true;
        }

        LOGGER_ERROR( "request [%u] not found"
            , _id
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpSystem::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        if( this->isStopService() == true )
        {
            return;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_RESPONSE, _response );

        m_mutex->lock();

        for( VectorReceiverDesc::iterator
            it = m_receiverDescs.begin(),
            it_end = m_receiverDescs.end();
            it != it_end;
            ++it )
        {
            ReceiverDesc & desc = *it;

            HttpRequestId requestId = _response->getRequestId();

            if( desc.id != requestId )
            {
                continue;
            }

            HttpReceiverInterfacePtr receiver = desc.receiver;

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