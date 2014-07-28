#	include "CurlHttpSystem.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "ThreadTaskDownloadAsset.h"

#	include "curl/curl.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Menge::HttpSystemInterface, Menge::CurlHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CurlHttpSystem::CurlHttpSystem()
		: m_serviceProvider(nullptr)
		, m_enumeratorDownloadAsset(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CurlHttpSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * CurlHttpSystem::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CurlHttpSystem::initialize()
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
			->createThread( CONST_STRING_LOCAL(m_serviceProvider, "ThreadCurlHttpSystem"), -1 );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CurlHttpSystem::finalize()
	{
		curl_global_cleanup();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t CurlHttpSystem::downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver )
	{
		if( FILE_SERVICE(m_serviceProvider)
			->hasFileGroup( _category, nullptr ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("CurlHttpSystem::downloadAsset not found category '%s' for filepath '%s'"
				, _category.c_str()
				, _path.c_str()
				);

			return false;
		}

		if( FILE_SERVICE(m_serviceProvider)
			->existFile( _category, _path, nullptr ) == true )
		{
			LOGGER_ERROR(m_serviceProvider)("CurlHttpSystem::downloadAsset category '%s' file alredy exist '%s'"
				, _category.c_str()
				, _path.c_str()
				);

			return false;
		}

		size_t task_id = ++m_enumeratorDownloadAsset;
		
		ThreadTaskDownloadAsset * task = 
			new ThreadTaskDownloadAsset( m_serviceProvider, _url, _category, _path, task_id, this );
		
		if( THREAD_SERVICE(m_serviceProvider)
			->addTask( CONST_STRING_LOCAL(m_serviceProvider, "ThreadCurlHttpSystem"), task ) == false )
		{
			delete task;

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
	bool CurlHttpSystem::cancelAsset( size_t _id )
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
	void CurlHttpSystem::onDownloadAssetComplete( size_t _id, bool _successful )
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