#include "cURLService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "cURLGetMessageThreadTask.h"
#include "cURLPostMessageThreadTask.h"
#include "cURLHeaderDataThreadTask.h"
#include "cURLGetAssetThreadTask.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Config/Stringstream.h"

#include "curl/curl.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( cURLService, Mengine::cURLService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
        CURLcode code = curl_global_init( CURL_GLOBAL_ALL );

        if( code != CURLE_OK )
        {
            LOGGER_ERROR( "invalid initialize curl %d:%s"
                , code
                , curl_easy_strerror( code )
            );

            return false;
        }

        uint32_t cURLServiceThreadCount = CONFIG_VALUE( "cURLService", "ThreadCount", 4 );

        m_threadQueue = THREAD_SERVICE()
            ->createTaskQueue( 1, MENGINE_DOCUMENT_FUNCTION );

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

        m_factoryTaskGetMessage = new FactoryPool<cURLGetMessageThreadTask, 8>();
        m_factoryTaskPostMessage = new FactoryPool<cURLPostMessageThreadTask, 8>();
        m_factoryTaskHeaderData = new FactoryPool<cURLHeaderDataThreadTask, 8>();
        m_factoryTaskDownloadAsset = new FactoryPool<cURLGetAssetThreadTask, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDownloadAsset );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskPostMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskHeaderData );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskGetMessage );

        m_factoryTaskDownloadAsset = nullptr;
        m_factoryTaskPostMessage = nullptr;
        m_factoryTaskHeaderData = nullptr;
        m_factoryTaskGetMessage = nullptr;

        curl_global_cleanup();
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
    HttpRequestID cURLService::postMessage( const String & _url, const MapParams & _params, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver )
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
    HttpRequestID cURLService::downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stop" );

            return 0;
        }

        if( _fileGroup->existFile( _path ) == true )
        {
            LOGGER_ERROR( "url '%s' category '%s' file alredy exist '%s'"
                , _url.c_str()
                , _fileGroup->getName().c_str()
                , _path.c_str()
            );

            return 0;
        }

        uint32_t task_id = GENERATE_UNIQUE_IDENTITY();

        cURLGetAssetThreadTaskPtr task = m_factoryTaskDownloadAsset->createObject( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( task, 0 );

        task->setRequestId( task_id );
        task->setTimeout( _timeout );
        task->setReceiver( Helper::makeIntrusivePtr( this ) );
        task->initialize( _url, _login, _password, _fileGroup, _path );

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