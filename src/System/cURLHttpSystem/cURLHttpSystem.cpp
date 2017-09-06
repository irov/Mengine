#	include "cURLHttpSystem.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "ThreadTaskGetMessage.h"
#	include "ThreadTaskPostMessage.h"
#	include "ThreadTaskHeaderData.h"
#	include "ThreadTaskGetAsset.h"

#	include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

#	include "curl/curl.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Menge::cURLHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	cURLHttpSystem::cURLHttpSystem()
		: m_enumeratorReceivers(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool cURLHttpSystem::_initialize()
	{
		CURLcode code = curl_global_init(CURL_GLOBAL_ALL);

		if( code != CURLE_OK )
		{
			LOGGER_ERROR(m_serviceProvider)("cURLHttpSystem::initialize invalid initialize curl %d:%s"
				, code
				, curl_easy_strerror(code)
				);

			return false;
		}

		if( THREAD_SERVICE( m_serviceProvider )
			->createThread( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadCurlHttpSystem" ), -1, __FILE__, __LINE__ ) == false )
		{
			return false;
		}

		m_factoryTaskGetMessage = new FactoryPool<ThreadTaskGetMessage, 8>( m_serviceProvider );
		m_factoryTaskPostMessage = new FactoryPool<ThreadTaskPostMessage, 8>( m_serviceProvider );
        m_factoryTaskHeaderData = new FactoryPool<ThreadTaskHeaderData, 8>( m_serviceProvider );
		m_factoryTaskDownloadAsset = new FactoryPool<ThreadTaskGetAsset, 8>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void cURLHttpSystem::_finalize()
	{
		THREAD_SERVICE( m_serviceProvider )
			->destroyThread( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadCurlHttpSystem" ) );
		
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

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _url, task_id, this );

		if( THREAD_SERVICE( m_serviceProvider )
			->addTask( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadCurlHttpSystem" ), task ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("cURLHttpSystem::getMessage url '%s' invalid add task"
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

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _url, _params, task_id, this );

		if( THREAD_SERVICE( m_serviceProvider )
			->addTask( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadCurlHttpSystem" ), task ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("cURLHttpSystem::postMessage url '%s' invalid add task"
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

        task->setServiceProvider( m_serviceProvider );
        task->initialize( _url, _headers, _data, task_id, this );

        if( THREAD_SERVICE( m_serviceProvider )
            ->addTask( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadCurlHttpSystem" ), task ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("cURLHttpSystem::headerData url '%s' invalid add task"
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
		if( FILE_SERVICE( m_serviceProvider )
			->hasFileGroup( _category, nullptr ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("CurlHttpSystem::downloadAsset url '%s' not found category '%s' for filepath '%s'"
				, _url.c_str()
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		if( FILE_SERVICE( m_serviceProvider )
			->existFile( _category, _path, nullptr ) == true )
		{
			LOGGER_ERROR( m_serviceProvider )("CurlHttpSystem::downloadAsset url '%s' category '%s' file alredy exist '%s'"
				, _url.c_str()
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		uint32_t task_id = ++m_enumeratorReceivers;
		
		ThreadTaskGetAssetPtr task = m_factoryTaskDownloadAsset->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _url, _login, _password, _category, _path, task_id, this );
		
		if( THREAD_SERVICE(m_serviceProvider)
			->addTask( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ThreadCurlHttpSystem"), task ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("CurlHttpSystem::downloadAsset url '%s' category '%s' path '%s' invalid add task"
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
	void cURLHttpSystem::onHttpRequestComplete( HttpRequestID _id, const String & _response, uint32_t _code, bool _successful )
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
				receiver->onHttpRequestComplete( _id, _response, _code, _successful );
			}

			break;
		}
	}
}