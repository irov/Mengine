#include "cURLService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "cURLGetMessageThreadTask.h"
#include "cURLPostMessageThreadTask.h"
#include "cURLHeaderDataThreadTask.h"
#include "cURLGetAssetThreadTask.h"

#include "cURLSource.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "Config/Stringstream.h"

#include "curl/curl.h"
#include "stdex/allocator_report.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( cURLService, Mengine::cURLService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void * stdex_curl_malloc_callback( size_t size )
    {
        return ::stdex_malloc( size, "curl" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_curl_free_callback( void * ptr )
    {
        ::stdex_free( ptr, "curl" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void * stdex_curl_realloc_callback( void * ptr, size_t size )
    {
        return ::stdex_realloc( ptr, size, "curl" );
    }
    //////////////////////////////////////////////////////////////////////////
    static char * stdex_curl_strdup_callback( const char * str )
    {
        size_t len = strlen( str ) + 1;
        void * m = ::stdex_malloc( len, "curl" );
        if( m == nullptr )
            return nullptr;
        return (char *)memcpy( m, str, len );
    }
    //////////////////////////////////////////////////////////////////////////
    static void * stdex_curl_calloc_callback( size_t nmemb, size_t size )
    { 
        return ::stdex_calloc( nmemb, size, "curl" );
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
            LOGGER_ERROR( "invalid initialize curl '%s' [%d]"
                , curl_easy_strerror( code )
                , code
            );

            return false;
        }

        uint32_t cURLServiceThreadCount = CONFIG_VALUE( "cURLService", "ThreadCount", 4 );

        ThreadQueueInterfacePtr threadQueue = THREAD_SERVICE()
            ->createTaskQueue( 1, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( threadQueue, false, "invalid create task queue" );

        m_threadQueue = threadQueue;

        for( uint32_t index = 0; index != cURLServiceThreadCount; ++index )
        {
            Stringstream ss;
            ss << "ThreadcURLService_" << index;
            ConstString threadName = Helper::stringizeString( ss.str() );

            THREAD_SERVICE()
                ->createThread( threadName, MENGINE_THREAD_PRIORITY_BELOW_NORMAL, MENGINE_DOCUMENT_FUNCTION );

            m_threads.emplace_back( threadName );

            m_threadQueue->addThread( threadName );
        }

        m_factorySource = Helper::makeFactoryPool<cURLSource, 8>();

        m_factoryTaskGetMessage = Helper::makeFactoryPool<cURLGetMessageThreadTask, 8>();
        m_factoryTaskPostMessage = Helper::makeFactoryPool<cURLPostMessageThreadTask, 8>();
        m_factoryTaskHeaderData = Helper::makeFactoryPool<cURLHeaderDataThreadTask, 8>();
        m_factoryTaskDownloadAsset = Helper::makeFactoryPool<cURLGetAssetThreadTask, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySource );

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDownloadAsset );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskPostMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskHeaderData );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskGetMessage );

        m_factorySource = nullptr;

        m_factoryTaskDownloadAsset = nullptr;
        m_factoryTaskPostMessage = nullptr;
        m_factoryTaskHeaderData = nullptr;
        m_factoryTaskGetMessage = nullptr;

        curl_global_cleanup();

#ifdef STDEX_ALLOCATOR_REPORT_ENABLE
        uint32_t report_count = stdex_get_allocator_report_count( "curl" );
        MENGINE_ASSERTION( report_count == 0, "cURL memleak [%d]"
            , report_count
        );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::_stopService()
    {
        for( const ReceiverDesc & desc : m_receiverDescs )
        {
            THREAD_SERVICE()
                ->joinTask( desc.task );
        }

        m_receiverDescs.clear();

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
    HttpRequestID cURLService::getMessage( const String & _url, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        uint32_t task_id = GENERATE_UNIQUE_IDENTITY();

        cURLGetMessageThreadTaskPtr task = m_factoryTaskGetMessage->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, 0 );

        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiver( Helper::makeIntrusivePtr( this ) );
        task->initialize( _url );

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        m_threadQueue->addTask( task );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::postMessage( const String & _url, const cURLPostParams & _params, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        uint32_t task_id = GENERATE_UNIQUE_IDENTITY();

        cURLPostMessageThreadTaskPtr task = m_factoryTaskPostMessage->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, 0 );

        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiver( Helper::makeIntrusivePtr( this ) );
        task->initialize( _url, _params );

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        m_threadQueue->addTask( task );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::headerData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        uint32_t task_id = GENERATE_UNIQUE_IDENTITY();

        cURLHeaderDataThreadTaskPtr task = m_factoryTaskHeaderData->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, 0 );

        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiver( Helper::makeIntrusivePtr( this ) );
        task->initialize( _url, _headers, _data );

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
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        if( _fileGroup->existFile( _filePath, true ) == true )
        {
            LOGGER_ERROR( "url '%s' category '%s' file alredy exist '%s'"
                , _url.c_str()
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return 0;
        }

        uint32_t task_id = GENERATE_UNIQUE_IDENTITY();

        cURLGetAssetThreadTaskPtr task = m_factoryTaskDownloadAsset->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, 0 );

        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiver( Helper::makeIntrusivePtr( this ) );
        task->initialize( _url, _login, _password, _fileGroup, _filePath );

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

            return 0;
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

        LOGGER_ERROR( "request '%d' not found"
            , _id
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    cURLSourceInterfacePtr cURLService::makeSource( const EngineSourcePtr & _source )
    {
        cURLSourcePtr curl_source = m_factorySource->createObject( MENGINE_DOCUMENT_FUNCTION );

        curl_source->setSource( _source );

        return curl_source;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful )
    {
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
                receiver->onHttpRequestComplete( _id, _status, _error, _response, _code, _successful );
            }

            return;
        }

        LOGGER_ERROR( "invalid request '%u' complete (status '%u' error '%s' response '%s' code '%u' successful '%d'"
            , _id
            , _status
            , _error.c_str()
            , _response.c_str()
            , _code
            , _successful
        );
    }
}