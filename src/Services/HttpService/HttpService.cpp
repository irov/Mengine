#include "HttpService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/HttpSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "HttpResponse.h"
#include "HttpReceiverPing.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/HttpLogger.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpService, Mengine::HttpService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HttpService::HttpService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HttpService::~HttpService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & HttpService::requiredServices() const
    {
        static const ServiceRequiredList service = {
            SERVICE_ID( HttpSystemInterface ),
            SERVICE_ID( ThreadSystemInterface )
        };

        return service;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HttpService::_availableService() const
    {
        if( HAS_OPTION( "nohttp" ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HttpService::_initializeService()
    {
        uint32_t Http_ThreadCountLow = CONFIG_VALUE_INTEGER( "Http", "ThreadCountLow", 4 );

        ThreadQueueInterfacePtr threadQueueLow = THREAD_SERVICE()
            ->createTaskQueue( 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadQueueLow, "invalid create task queue" );

        m_threadQueueLow = threadQueueLow;

        for( uint32_t index = 0; index != Http_ThreadCountLow; ++index )
        {
            Stringstream ss;
            ss << "HttpLow_" << index;
            ConstString processorName = Helper::stringizeString( ss.str() );

            if( THREAD_SERVICE()
                ->createThreadProcessor( processorName, MENGINE_THREAD_DESCRIPTION( "MNGHttp" ), ETP_LOWEST, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }

            m_threads.emplace_back( processorName );

            m_threadQueueLow->addThread( processorName );
        }

        uint32_t Http_ThreadCountHigh = CONFIG_VALUE_INTEGER( "Http", "ThreadCountHigh", 8 );

        ThreadQueueInterfacePtr threadQueueHigh = THREAD_SERVICE()
            ->createTaskQueue( 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadQueueHigh, "invalid create task queue" );

        m_threadQueueHigh = threadQueueHigh;

        for( uint32_t index = 0; index != Http_ThreadCountHigh; ++index )
        {
            Stringstream ss;
            ss << "HttpHigh_" << index;
            ConstString processorName = Helper::stringizeString( ss.str() );

            if( THREAD_SERVICE()
                ->createThreadProcessor( processorName, MENGINE_THREAD_DESCRIPTION( "MNGHttpHigh" ), ETP_BELOW_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }

            m_threads.emplace_back( processorName );

            m_threadQueueHigh->addThread( processorName );
        }

        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        m_mutex = mutex;

        HttpHeaders applicationJSONHeaders;
        applicationJSONHeaders.push_back( "Content-Type: application/json" );

        m_applicationJSONHeaders = applicationJSONHeaders;

        m_factoryResponse = Helper::makeFactoryPoolWithMutex<HttpResponse, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryReceiverPing = Helper::makeFactoryPoolWithMutex<HttpReceiverPing, 16>( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &HttpService::notifyEnginePrepareFinalize_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );

        m_mutex = nullptr;

        m_receiverDescs.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryResponse );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryReceiverPing );

        m_factoryResponse = nullptr;
        m_factoryReceiverPing = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpService::notifyEnginePrepareFinalize_()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        VectorReceiverDesc receiverDescs = m_receiverDescs;
        m_receiverDescs.clear();

        for( const ReceiverDesc & desc : receiverDescs )
        {
            const ThreadTaskInterfacePtr & task = desc.task;

            THREAD_SERVICE()
                ->joinTask( task );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpService::_stopService()
    {
        for( const ConstString & threadName : m_threads )
        {
            THREAD_SERVICE()
                ->destroyThreadProcessor( threadName );
        }

        m_threads.clear();

        if( m_threadQueueLow != nullptr )
        {
            THREAD_SERVICE()
                ->cancelTaskQueue( m_threadQueueLow );

            m_threadQueueLow = nullptr;
        }

        if( m_threadQueueHigh != nullptr )
        {
            THREAD_SERVICE()
                ->cancelTaskQueue( m_threadQueueHigh );

            m_threadQueueHigh = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpService::setProxy( const String & _proxy )
    {
        m_proxy = _proxy;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & HttpService::getProxy() const
    {
        return m_proxy;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpHeaders & HttpService::getApplicationJSONHeaders() const
    {
        return m_applicationJSONHeaders;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpService::ping( const String & _url, int32_t _timeout, const HttpLambdaPing & _lambda, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            _lambda( false );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId requestId = Helper::generateUniqueIdentity();

        HttpRequestInterfacePtr request = HTTP_SYSTEM()
            ->createHttpRequestPing( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( request, "invalid create http request" );

        request->setURL( _url );
        request->setProxy( m_proxy );
        request->setRequestId( requestId );
        request->setTimeout( _timeout );
        request->setFlags( EHRF_LOW_PRIORITY );

        AndroidHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response, "invalid create http response" );

        response->setRequest( request );

        request->setReponse( response );

        request->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        ThreadTaskInterfacePtr task = request->getThreadTask();

        HttpReceiverPingPtr receiver = m_factoryReceiverPing->createObject( _doc );

        receiver->setLambda( _lambda );

        ReceiverDesc desc;
        desc.id = requestId;
        desc.type = ERT_PING;
        desc.timestamp = Helper::getSystemTimestamp();
        desc.task = task;
        desc.receiver = receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mutex->lock();
        m_receiverDescs.push_back( desc );
        m_mutex->unlock();

        this->addRequestToQueue( EHRF_LOW_PRIORITY, task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, requestId, _url );

        return requestId;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpService::getMessage( const String & _url, const HttpHeaders & _headers, int32_t _timeout, uint32_t _flags, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId requestId = Helper::generateUniqueIdentity();

        HttpRequestInterfacePtr request = HTTP_SYSTEM()
            ->createHttpRequestGetMessage( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( request, "invalid create http request" );

        request->setURL( _url );
        request->setProxy( m_proxy );
        request->setHeaders( _headers );
        request->setRequestId( requestId );
        request->setTimeout( _timeout );
        request->setFlags( _flags );

        AndroidHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response, "invalid create http response" );

        response->setRequest( request );

        request->setReponse( response );

        request->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        ThreadTaskInterfacePtr task = request->getThreadTask();

        ReceiverDesc desc;
        desc.id = requestId;
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

        this->addRequestToQueue( _flags, task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, requestId, _url );

        return requestId;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpService::postMessage( const String & _url, const HttpHeaders & _headers, int32_t _timeout, uint32_t _flags, const HttpRequestPostProperties & _params, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId requestId = Helper::generateUniqueIdentity();

        HttpRequestInterfacePtr request = HTTP_SYSTEM()
            ->createHttpRequestPostMessage( _params, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( request, "invalid create http request" );

        request->setURL( _url );
        request->setProxy( m_proxy );
        request->setHeaders( _headers );
        request->setRequestId( requestId );
        request->setTimeout( _timeout );
        request->setFlags( _flags );

        AndroidHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response, "invalid create http response" );

        response->setRequest( request );

        request->setReponse( response );

        request->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        ThreadTaskInterfacePtr task = request->getThreadTask();

        ReceiverDesc desc;
        desc.id = requestId;
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

        this->addRequestToQueue( _flags, task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, requestId, _url );

        return requestId;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpService::deleteMessage( const String & _url, const HttpHeaders & _headers, int32_t _timeout, uint32_t _flags, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId requestId = Helper::generateUniqueIdentity();

        HttpRequestInterfacePtr request = HTTP_SYSTEM()
            ->createHttpRequestDeleteMessage( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( request, "invalid create http request" );

        request->setURL( _url );
        request->setProxy( m_proxy );
        request->setHeaders( _headers );
        request->setRequestId( requestId );
        request->setTimeout( _timeout );
        request->setFlags( _flags );

        AndroidHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response, "invalid create http response" );

        response->setRequest( request );

        request->setReponse( response );

        request->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        ThreadTaskInterfacePtr task = request->getThreadTask();

        ReceiverDesc desc;
        desc.id = requestId;
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

        this->addRequestToQueue( _flags, task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, requestId, _url );

        return requestId;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpService::headerData( const String & _url, const HttpHeaders & _headers, const Data & _data, int32_t _timeout, uint32_t _flags, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_HTTP_ERROR( "service is stop" );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId requestId = Helper::generateUniqueIdentity();

        HttpRequestInterfacePtr request = HTTP_SYSTEM()
            ->createHttpRequestHeaderData( _data, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( request, "invalid create http request" );

        request->setURL( _url );
        request->setHeaders( _headers );
        request->setRequestId( requestId );
        request->setTimeout( _timeout );
        request->setFlags( _flags );

        AndroidHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response, "invalid create http response" );

        response->setRequest( request );

        request->setReponse( response );

        request->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        ThreadTaskInterfacePtr task = request->getThreadTask();

        ReceiverDesc desc;
        desc.id = requestId;
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

        this->addRequestToQueue( _flags, task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, requestId, _url );

        return requestId;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestId HttpService::getAsset( const String & _url, const String & _login, const String & _password, const ContentInterfacePtr & _content, int32_t _timeout, uint32_t _flags, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc )
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
                , Helper::getContentFullPath( _content ).c_str()
            );

            return MENGINE_HTTP_REQUEST_INVALID;
        }

        UniqueId requestId = Helper::generateUniqueIdentity();

        HttpRequestInterfacePtr request = HTTP_SYSTEM()
            ->createHttpRequestGetAsset( _login, _password, _content, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( request, "invalid create http request" );

        request->setURL( _url );
        request->setProxy( m_proxy );
        request->setRequestId( requestId );
        request->setTimeout( _timeout );
        request->setFlags( _flags );

        AndroidHttpResponsePtr response = m_factoryResponse->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( response, "invalid create http response" );

        response->setRequest( request );

        request->setReponse( response );

        request->setReceiver( HttpReceiverInterfacePtr::from( this ) );

        ThreadTaskInterfacePtr task = request->getThreadTask();

        ReceiverDesc desc;
        desc.id = requestId;
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

        this->addRequestToQueue( _flags, task );

        NOTIFICATION_NOTIFY( NOTIFICATOR_HTTP_REQUEST, requestId, _url );

        return requestId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HttpService::cancelRequest( HttpRequestId _id )
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
    void HttpService::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
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

            const HttpRequestInterfacePtr & request = _response->getRequest();

            HttpRequestId requestId = request->getRequestId();

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

        LOGGER_HTTP_ERROR( "invalid request '%u' %s (error '%.2048s' [%u] code [%u])"
            , _response->getRequest()->getRequestId()
            , _response->isSuccessful() ? "complete" : "failure"
            , _response->getErrorMessage().c_str()
            , _response->getErrorCode()
            , _response->getCode()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void HttpService::addRequestToQueue( uint32_t _flags, const ThreadTaskInterfacePtr & _task )
    {
        if( _flags & EHRF_LOW_PRIORITY )
        {
            m_threadQueueLow->addTask( _task );
        }
        else
        {
            m_threadQueueHigh->addTask( _task );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}