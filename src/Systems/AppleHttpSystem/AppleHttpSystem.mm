#include "AppleHttpSystem.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "AppleHttpRequestGetMessageThreadTask.h"
#include "AppleHttpRequestPostMessageThreadTask.h"
#include "AppleHttpRequestHeaderDataThreadTask.h"
#include "AppleHttpRequestGetAssetThreadTask.h"
#include "AppleHttpRequestDeleteMessageThreadTask.h"

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
#include "Kernel/ContentHelper.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::AppleHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpSystem::AppleHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpSystem::~AppleHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpSystem::_dependencyService()
    {
        SERVICE_DEPENDENCY( AppleHttpSystem, ThreadServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpSystem::_initializeService()
    {
        uint32_t cURLService_ThreadCount = CONFIG_VALUE( "Http", "ThreadCount", 8 );

        ThreadQueueInterfacePtr threadQueue = THREAD_SERVICE()
            ->createTaskQueue( 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadQueue, "invalid create task queue" );

        m_threadQueue = threadQueue;

        for( uint32_t index = 0; index != cURLService_ThreadCount; ++index )
        {
            Stringstream ss;
            ss << "ThreadHttp_" << index;
            ConstString threadName = Helper::stringizeString( ss.str() );

            if( THREAD_SERVICE()
                ->createThreadProcessor( threadName, ETP_BELOW_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }

            m_threads.emplace_back( threadName );

            m_threadQueue->addThread( threadName );
        }

        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        HttpRequestHeaders applicationJSONHeaders;
        applicationJSONHeaders.push_back( "Content-Type:application/json" );

        m_applicationJSONHeaders = applicationJSONHeaders;

        m_factoryResponse = Helper::makeFactoryPool<AppleHttpResponse, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskGetMessage = Helper::makeFactoryPool<AppleHttpRequestGetMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskPostMessage = Helper::makeFactoryPool<AppleHttpRequestPostMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDeleteMessage = Helper::makeFactoryPool<AppleHttpRequestDeleteMessageThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskHeaderData = Helper::makeFactoryPool<AppleHttpRequestHeaderDataThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDownloadAsset = Helper::makeFactoryPool<AppleHttpRequestGetAssetThreadTask, 16>( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &AppleHttpSystem::notifyEnginePrepareFinalize_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpSystem::_finalizeService()
    {
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
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpSystem::notifyEnginePrepareFinalize_()
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
    void AppleHttpSystem::_stopService()
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
    void AppleHttpSystem::setProxy( const String & _proxy )
    {
        m_proxy = _proxy;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AppleHttpSystem::getProxy() const
    {
        return m_proxy;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestHeaders & AppleHttpSystem::getApplicationJSONHeaders() const
    {
        return m_applicationJSONHeaders;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId AppleHttpSystem::getMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        AppleHttpRequestGetMessageThreadTaskPtr task = m_factoryTaskGetMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setProxy( m_proxy );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        AppleHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );
        
        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        if( task->initialize() == false )
        {
            LOGGER_HTTP_ERROR( "url '%s' invalid initialize task"
                , _url.c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_GET_MESSAGE;
        desc.timestamp = Helper::getSystemTimestamp();
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
    HttpRequestId AppleHttpSystem::postMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpRequestPostProperties & _params, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        AppleHttpRequestPostMessageThreadTaskPtr task = m_factoryTaskPostMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setProxy( m_proxy );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        AppleHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        if( task->initialize( _params ) == false )
        {
            LOGGER_HTTP_ERROR( "url '%s' invalid initialize task"
                , _url.c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_POST_MESSAGE;
        desc.timestamp = Helper::getSystemTimestamp();
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
    HttpRequestId AppleHttpSystem::deleteMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        AppleHttpRequestDeleteMessageThreadTaskPtr task = m_factoryTaskDeleteMessage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setProxy( m_proxy );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        AppleHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        if( task->initialize() == false )
        {
            LOGGER_HTTP_ERROR( "url '%s' invalid initialize task"
                , _url.c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_POST_MESSAGE;
        desc.timestamp = Helper::getSystemTimestamp();
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
    HttpRequestId AppleHttpSystem::headerData( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const Data & _data, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId task_id = Helper::generateUniqueIdentity();

        AppleHttpRequestHeaderDataThreadTaskPtr task = m_factoryTaskHeaderData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setHeaders( _headers );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiveHeaders( _receiveHeaders );

        AppleHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        if( task->initialize( _data ) == false )
        {
            LOGGER_HTTP_ERROR( "url '%s' invalid initialize task"
                , _url.c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_HEADER_DATA;
        desc.timestamp = Helper::getSystemTimestamp();
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
    HttpRequestId AppleHttpSystem::downloadAsset( const String & _url, const String & _login, const String & _password, const ContentInterfacePtr & _content, int32_t _timeout, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return 0;
        }

        if( _content->exist( true ) == true )
        {
            LOGGER_HTTP_ERROR( "url '%s' file already exist '%s'"
                , _url.c_str()
                , Helper::getContentFullPath( _content )
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        const FilePath & filePath = _content->getFilePath();

        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", filePath.c_str() );

        UniqueId task_id = Helper::generateUniqueIdentity();

        AppleHttpRequestGetAssetThreadTaskPtr task = m_factoryTaskDownloadAsset->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->setURL( _url );
        task->setProxy( m_proxy );
        task->setRequestId( task_id );
        task->setTimeout( _timeout );

        AppleHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response );

        response->setRequestId( task_id );

        task->setReponse( response );

        task->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        if( task->initialize( _login, _password, _content, filePathTmp ) == false )
        {
            LOGGER_HTTP_ERROR( "url '%s' file '%s' invalid initialize task"
                , _url.c_str()
                , Helper::getContentFullPath( _content )
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.type = ERT_DOWNLOAD_ASSET;
        desc.timestamp = Helper::getSystemTimestamp();
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
    bool AppleHttpSystem::cancelRequest( HttpRequestId _id )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

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

        LOGGER_HTTP_ERROR( "request [%u] not found"
            , _id
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpSystem::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
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

        LOGGER_HTTP_ERROR( "invalid request '%u' %s (error '%.2048s' response '%.2048s' code [%u])"
            , _response->getRequestId()
            , _response->isSuccessful() ? "complete" : "failure"
            , _response->getError().c_str()
            , _response->getData().c_str()
            , _response->getCode()            
        );
    }
    //////////////////////////////////////////////////////////////////////////
}