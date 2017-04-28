#	include "cURLHttpSystem.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "ThreadTaskGetAsset.h"
#	include "ThreadTaskPostMessage.h"

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
		: m_enumeratorDownloadAsset(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool cURLHttpSystem::_initialize()
	{
		CURLcode code = curl_global_init(CURL_GLOBAL_ALL);

		if( code != CURLE_OK )
		{
			LOGGER_ERROR(m_serviceProvider)("CurlHttpSystem::initialize invalid initialize curl %d:%s"
				, code
				, curl_easy_strerror(code)
				);

			return false;
		}

		THREAD_SERVICE(m_serviceProvider)
			->createThread( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ThreadCurlHttpSystem"), -1, __FILE__, __LINE__ );

		m_factoryTaskDownloadAsset = new FactoryPool<ThreadTaskGetAsset, 8>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void cURLHttpSystem::_finalize()
	{
		curl_global_cleanup();

		m_factoryTaskDownloadAsset = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpRequestID cURLHttpSystem::postMessage( const String & _url, const TMapParams & _params, HttpPostMessageReceiver * _receiver )
	{
		uint32_t task_id = ++m_enumeratorDownloadAsset;

		ThreadTaskPostMessagePtr task = m_factoryTaskPostMessage->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _url, _params, task_id, this );

		if( THREAD_SERVICE( m_serviceProvider )
			->addTask( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ThreadCurlHttpSystem" ), task ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("CurlHttpSystem::postMessage url '%s' invalid add task"
				, _url.c_str()
				);

			return 0;
		}

		PostMessageDesc desc;
		desc.id = task_id;
		desc.task = task;
		desc.receiver = _receiver;

		m_postMessages.push_back( desc );

		return task_id;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpRequestID cURLHttpSystem::downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver )
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

		uint32_t task_id = ++m_enumeratorDownloadAsset;
		
		ThreadTaskGetAssetPtr task = m_factoryTaskDownloadAsset->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _url, _category, _path, task_id, this );
		
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

		DownloadAssetDesc desc;
		desc.id = task_id;
		desc.task = task;
		desc.receiver = _receiver;

		m_downloadAssets.push_back( desc );

		return task_id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool cURLHttpSystem::cancelPostMessage( HttpRequestID _id )
	{
		for( TVectorPostMessages::iterator
			it = m_postMessages.begin(),
			it_end = m_postMessages.end();
			it != it_end;
			++it )
		{
			PostMessageDesc & desc = *it;

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
	bool cURLHttpSystem::cancelDownloadAsset( HttpRequestID _id )
	{
		for( TVectorDownloadAssets::iterator
			it = m_downloadAssets.begin(),
			it_end = m_downloadAssets.end();
		it != it_end;
		++it )
		{
			DownloadAssetDesc & desc = *it;

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
	void cURLHttpSystem::onDownloadAssetComplete( HttpRequestID _id, bool _successful )
	{
		for( TVectorDownloadAssets::iterator
			it = m_downloadAssets.begin(),
			it_end = m_downloadAssets.end();
		it != it_end;
		++it )
		{
			DownloadAssetDesc & desc = *it;

			if( desc.id != _id )
			{
				continue;
			}

			HttpDownloadAssetReceiver * receiver = desc.receiver;

			m_downloadAssets.erase( it );

			if( receiver != nullptr)
			{
				receiver->onDownloadAssetComplete( _id, _successful );
			}
			
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void cURLHttpSystem::onPostMessageComplete( HttpRequestID _id, const String & _response, uint32_t _code, bool _successful )
	{
		for( TVectorPostMessages::iterator
			it = m_postMessages.begin(),
			it_end = m_postMessages.end();
			it != it_end;
			++it )
		{
			PostMessageDesc & desc = *it;

			if( desc.id != _id )
			{
				continue;
			}

			HttpPostMessageReceiver * receiver = desc.receiver;

			m_postMessages.erase( it );

			if( receiver != nullptr )
			{
				receiver->onPostMessageComplete( _id, _response, _code, _successful );
			}

			break;
		}
	}
}