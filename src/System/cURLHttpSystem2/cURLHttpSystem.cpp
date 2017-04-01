#	include "cURLHttpSystem.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "ThreadTaskDownloadAsset.h"

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

		m_factoryTaskDownloadAsset = new FactoryPool<ThreadTaskDownloadAsset, 8>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void cURLHttpSystem::_finalize()
	{
		curl_global_cleanup();

		m_factoryTaskDownloadAsset = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	HttpAssetID cURLHttpSystem::downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver )
	{
		if( FILE_SERVICE(m_serviceProvider)
			->hasFileGroup( _category, nullptr ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("CurlHttpSystem::downloadAsset not found category '%s' for filepath '%s'"
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		if( FILE_SERVICE(m_serviceProvider)
			->existFile( _category, _path, nullptr ) == true )
		{
			LOGGER_ERROR(m_serviceProvider)("CurlHttpSystem::downloadAsset category '%s' file alredy exist '%s'"
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		uint32_t task_id = ++m_enumeratorDownloadAsset;
		
		ThreadTaskDownloadAssetPtr task = m_factoryTaskDownloadAsset->createObject();

		task->setServiceProvider( m_serviceProvider );
		task->initialize( _url, _category, _path, task_id, this );
		
		if( THREAD_SERVICE(m_serviceProvider)
			->addTask( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ThreadCurlHttpSystem"), task ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("CurlHttpSystem::downloadAsset category '%s' path '%s' invalid add task"
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
	bool cURLHttpSystem::cancelAsset( HttpAssetID _id )
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
	void cURLHttpSystem::onDownloadAssetComplete( HttpAssetID _id, bool _successful )
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
}