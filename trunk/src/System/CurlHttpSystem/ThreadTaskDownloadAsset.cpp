#	include "ThreadTaskDownloadAsset.h"

#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include "curl/curl.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskDownloadAsset::ThreadTaskDownloadAsset( ServiceProviderInterface * _serviceProvider, const String & _url, const ConstString & _category, const FilePath & _filepath, size_t _id, HttpDownloadAssetReceiver * _receiver )
		: m_serviceProvider(_serviceProvider)
		, m_url(_url)
		, m_category(_category)
		, m_filePath(_filepath)
		, m_id(_id)
		, m_receiver(_receiver)
		, m_successful(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskDownloadAsset::_onRun()
	{
		m_stream = FILE_SERVICE(m_serviceProvider)
			->openOutputFile( m_category, m_filePath );

		if( m_stream == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static size_t WriteMemoryCallback( void * _contents, size_t _size, size_t _nmemb, void * _userp )
	{
		OutputStreamInterface * stream_ptr = (OutputStreamInterface *)_userp;

		size_t realsize = _size * _nmemb;

		stream_ptr->write( _contents, realsize );

		return realsize;
	}
	//////////////////////////////////////////////////////////////////////////
	static int XFERInfoCallback( void * _userp
		, curl_off_t _dltotal, curl_off_t _dlnow
		, curl_off_t _ultotal, curl_off_t _ulnow 
		)
	{
		ThreadTaskDownloadAsset * task = (ThreadTaskDownloadAsset *)_userp;

		if( task->isCancel() == true )
		{
			return 1;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskDownloadAsset::_onMain()
	{		
		/* init the curl session */
		CURL * curl_handle = curl_easy_init();

		/* specify URL to get */
		const char * url_str = m_url.c_str();
		curl_easy_setopt( curl_handle, CURLOPT_URL, url_str );

		/* send all data to this function  */
		curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback );

		/* we pass our 'chunk' struct to the callback function */
		OutputStreamInterface * stream_ptr = m_stream.get();
		curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, (void *)stream_ptr );

		curl_easy_setopt( curl_handle, CURLOPT_XFERINFOFUNCTION, XFERInfoCallback );
		curl_easy_setopt( curl_handle, CURLOPT_XFERINFODATA, (void *)this );

		/* some servers don't like requests that are made without a user-agent
		field, so we provide one */
		//curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		
		curl_easy_setopt( curl_handle, CURLOPT_NOPROGRESS, 0L );

		/* get it! */
		CURLcode res = curl_easy_perform(curl_handle);

		curl_easy_cleanup( curl_handle );

		/* check for errors */
		if( res == CURLE_ABORTED_BY_CALLBACK )
		{
			return false;
		}
		else if( res != CURLE_OK )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskDownloadAsset::_onMain invalid download asset from %s to %s:%s error %d:%s"
				, m_url.c_str()
				, m_category.c_str()
				, m_filePath.c_str()
				, res
				, curl_easy_strerror(res)
				);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskDownloadAsset::_onComplete( bool _successful )
	{
		m_stream->flush();
		m_stream = nullptr;

		if( _successful == false )
		{
			FILE_SERVICE(m_serviceProvider)
				->removeFile( m_category, m_filePath );
		}

		m_receiver->onDownloadAssetComplete( m_id, _successful );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskDownloadAsset::destroy()
	{
		delete this;
	}
}
