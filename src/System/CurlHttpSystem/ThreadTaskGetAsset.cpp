#	include "ThreadTaskGetAsset.h"

#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include "curl/curl.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskGetAsset::ThreadTaskGetAsset()
		: m_id(0)
		, m_receiver(nullptr)
		, m_successful(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskGetAsset::initialize( const String & _url, const ConstString & _category, const FilePath & _filepath, HttpRequestID _id, HttpDownloadAssetReceiver * _receiver )
	{
		m_url = _url;
		m_category = _category;
		m_filePath = _filepath;
		m_id = _id;
		m_receiver = _receiver;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskGetAsset::_onRun()
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

		uint32_t realsize = _size * _nmemb;

		stream_ptr->write( _contents, realsize );

		return realsize;
	}
	//////////////////////////////////////////////////////////////////////////
	static int XFERInfoCallback( void * _userp
		, curl_off_t _dltotal, curl_off_t _dlnow
		, curl_off_t _ultotal, curl_off_t _ulnow 
		)
	{
		(void)_dltotal;
		(void)_dlnow;
		(void)_ultotal;
		(void)_ulnow;

		ThreadTaskGetAsset * task = (ThreadTaskGetAsset *)_userp;

		if( task->isCancel() == true )
		{
			return 1;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskGetAsset::_onMain()
	{		
		/* init the curl session */
		CURL * curl = curl_easy_init();

		/* specify URL to get */
		const char * url_str = m_url.c_str();
		curl_easy_setopt( curl, CURLOPT_URL, url_str );

		/* send all data to this function  */
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback );

		/* we pass our 'chunk' struct to the callback function */
		OutputStreamInterface * stream_ptr = m_stream.get();
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void *)stream_ptr );

		curl_easy_setopt( curl, CURLOPT_XFERINFOFUNCTION, XFERInfoCallback );
		curl_easy_setopt( curl, CURLOPT_XFERINFODATA, (void *)this );

		/* some servers don't like requests that are made without a user-agent
		field, so we provide one */
		//curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		
		curl_easy_setopt( curl, CURLOPT_NOPROGRESS, 0L );

		/* get it! */
		CURLcode res = curl_easy_perform(curl);

		curl_easy_cleanup( curl );

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
	void ThreadTaskGetAsset::_onComplete( bool _successful )
	{
		m_stream->flush();
		m_stream = nullptr;

		m_receiver->onDownloadAssetComplete( m_id, _successful );
	}
}
