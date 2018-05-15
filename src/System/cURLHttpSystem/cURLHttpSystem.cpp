#	include "cURLHttpSystem.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "ThreadTaskGetMessage.h"
#	include "ThreadTaskPostMessage.h"
#	include "ThreadTaskHeaderData.h"
#	include "ThreadTaskGetAsset.h"

#	include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

#	include "curl/curl.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::cURLHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHttpSystem::cURLHttpSystem()
        : m_enumeratorReceivers( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpSystem::~cURLHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpSystem::_initializeService()
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

        m_factoryTaskGetMessage = new FactoryPool<ThreadTaskGetMessage, 8>();
        m_factoryTaskPostMessage = new FactoryPool<ThreadTaskPostMessage, 8>();
        m_factoryTaskHeaderData = new FactoryPool<ThreadTaskHeaderData, 8>();
        m_factoryTaskDownloadAsset = new FactoryPool<ThreadTaskGetAsset, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpSystem::_finalizeService()
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
    HttpRequestID cURLHttpSystem::getMessage( const String & _url, HttpReceiver * _receiver )
    {
        uint32_t task_id = ++m_enumeratorReceivers;

        ThreadTaskGetMessagePtr task = m_factoryTaskGetMessage->createObject();

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

        HttpReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLHttpSystem::postMessage( const String & _url, const TMapParams & _params, HttpReceiver * _receiver )
    {
        uint32_t task_id = ++m_enumeratorReceivers;

        ThreadTaskPostMessagePtr task = m_factoryTaskPostMessage->createObject();

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

        HttpReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLHttpSystem::headerData( const String & _url, const TVectorString & _headers, const String & _data, HttpReceiver * _receiver )
    {
        uint32_t task_id = ++m_enumeratorReceivers;

        ThreadTaskHeaderDataPtr task = m_factoryTaskHeaderData->createObject();

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

        HttpReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestID cURLHttpSystem::downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _path, HttpReceiver * _receiver )
    {
        if( FILE_SERVICE()
            ->hasFileGroup( _category, nullptr ) == false )
        {
            LOGGER_ERROR( "CurlHttpSystem::downloadAsset url '%s' not found category '%s' for filepath '%s'"
                , _url.c_str()
                , _category.c_str()
                , _path.c_str()
            );

            return 0;
        }

        if( FILE_SERVICE()
            ->existFile( _category, _path, nullptr ) == true )
        {
            LOGGER_ERROR( "CurlHttpSystem::downloadAsset url '%s' category '%s' file alredy exist '%s'"
                , _url.c_str()
                , _category.c_str()
                , _path.c_str()
            );

            return 0;
        }

        uint32_t task_id = ++m_enumeratorReceivers;

        ThreadTaskGetAssetPtr task = m_factoryTaskDownloadAsset->createObject();

        task->setRequestId( task_id );
        task->setReceiver( this );
        task->initialize( _url, _login, _password, _category, _path );

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "ThreadCurlHttpSystem" ), task ) == false )
        {
            LOGGER_ERROR( "CurlHttpSystem::downloadAsset url '%s' category '%s' path '%s' invalid add task"
                , _url.c_str()
                , _category.c_str()
                , _path.c_str()
            );

            return 0;
        }

        HttpReceiverDesc desc;
        desc.id = task_id;
        desc.task = task;
        desc.receiver = _receiver;

        m_receiverDescs.push_back( desc );

        return task_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpSystem::cancelRequest( HttpRequestID _id )
    {
        for( TVectorHttpReceiverDesc::iterator
            it = m_receiverDescs.begin(),
            it_end = m_receiverDescs.end();
            it != it_end;
            ++it )
        {
            HttpReceiverDesc & desc = *it;

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
    void cURLHttpSystem::onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _response, uint32_t _code, bool _successful )
    {
        for( TVectorHttpReceiverDesc::iterator
            it = m_receiverDescs.begin(),
            it_end = m_receiverDescs.end();
            it != it_end;
            ++it )
        {
            HttpReceiverDesc & desc = *it;

            if( desc.id != _id )
            {
                continue;
            }

            HttpReceiver * receiver = desc.receiver;

            m_receiverDescs.erase( it );

            if( receiver != nullptr )
            {
                receiver->onHttpRequestComplete( _id, _status, _response, _code, _successful );
            }

            break;
        }
    }
}