#include "cURLService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/StringizeInterface.h"

#include "cURLGetMessageThreadTask.h"
#include "cURLPostMessageThreadTask.h"
#include "cURLHeaderDataThreadTask.h"
#include "cURLGetAssetThreadTask.h"

#include "Kernel/FactoryPool.h"

#include "Kernel/Logger.h"

#include "curl/curl.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( cURLService, Mengine::cURLService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLService::cURLService()
        : m_enumeratorReceivers( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLService::~cURLService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLService::_initializeService()
    {
        CURLcode code = curl_global_init( CURL_GLOBAL_ALL );

        if( code != CURLE_OK )
        {
            LOGGER_ERROR( "cURLHttpSystem::initialize invalid initialize curl %d:%s"
                , code
                , curl_easy_strerror( code )
            );

            return false;
        }

        if( THREAD_SERVICE()
            ->createThread( STRINGIZE_STRING_LOCAL( "ThreadCurlHttpSystem" ), -1, __FILE__, __LINE__ ) == false )
        {
            return false;
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
        THREAD_SERVICE()
            ->destroyThread( STRINGIZE_STRING_LOCAL( "ThreadCurlHttpSystem" ) );

        m_factoryTaskDownloadAsset = nullptr;
        m_factoryTaskPostMessage = nullptr;
        m_factoryTaskHeaderData = nullptr;
        m_factoryTaskGetMessage = nullptr;

        curl_global_cleanup();
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::getMessage( const String & _url, const cURLReceiverInterfacePtr & _receiver )
    {
        uint32_t task_id = ++m_enumeratorReceivers;

        cURLGetMessageThreadTaskPtr task = m_factoryTaskGetMessage->createObject();

        task->setRequestId( task_id );
        task->setReceiver( this );
        task->initialize( _url );

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "ThreadCurlHttpSystem" ), task ) == false )
        {
            LOGGER_ERROR( "cURLHttpSystem::getMessage url '%s' invalid add task"
                , _url.c_str()
            );

            return 0;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::postMessage( const String & _url, const MapParams & _params, const cURLReceiverInterfacePtr & _receiver )
    {
        uint32_t task_id = ++m_enumeratorReceivers;

        cURLPostMessageThreadTaskPtr task = m_factoryTaskPostMessage->createObject();

        task->setRequestId( task_id );
        task->setReceiver( this );
        task->initialize( _url, _params );

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "ThreadCurlHttpSystem" ), task ) == false )
        {
            LOGGER_ERROR( "cURLHttpSystem::postMessage url '%s' invalid add task"
                , _url.c_str()
            );

            return 0;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::headerData( const String & _url, const VectorString & _headers, const String & _data, const cURLReceiverInterfacePtr & _receiver )
    {
        uint32_t task_id = ++m_enumeratorReceivers;

        cURLHeaderDataThreadTaskPtr task = m_factoryTaskHeaderData->createObject();

        task->setRequestId( task_id );
        task->setReceiver( this );
        task->initialize( _url, _headers, _data );

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "ThreadCurlHttpSystem" ), task ) == false )
        {
            LOGGER_ERROR( "cURLHttpSystem::headerData url '%s' invalid add task"
                , _url.c_str()
            );

            return 0;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLService::downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path, const cURLReceiverInterfacePtr & _receiver )
    {
        if( _fileGroup->existFile( _path ) == true )
        {
            LOGGER_ERROR( "CurlHttpSystem::downloadAsset url '%s' category '%s' file alredy exist '%s'"
                , _url.c_str()
                , _fileGroup->getName().c_str()
                , _path.c_str()
            );

            return 0;
        }

        uint32_t task_id = ++m_enumeratorReceivers;

        cURLGetAssetThreadTaskPtr task = m_factoryTaskDownloadAsset->createObject();

        task->setRequestId( task_id );
        task->setReceiver( this );
        task->initialize( _url, _login, _password, _fileGroup, _path );

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "ThreadCurlHttpSystem" ), task ) == false )
        {
            LOGGER_ERROR( "CurlHttpSystem::downloadAsset url '%s' category '%s' path '%s' invalid add task"
                , _url.c_str()
                , _fileGroup->getName().c_str()
                , _path.c_str()
            );

            return 0;
        }

        ReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLService::cancelRequest( HttpRequestID _id )
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

            desc.task->cancel();

            return true;
        }

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

            m_receiverDescs.erase( it );

            if( receiver != nullptr )
            {
                receiver->onHttpRequestComplete( _id, _status, _error, _response, _code, _successful );
            }

            break;
        }
    }
}