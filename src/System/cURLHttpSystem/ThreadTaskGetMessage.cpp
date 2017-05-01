#	include "ThreadTaskGetMessage.h"

#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include "curl/curl.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////
	static size_t s_writeRequestPerformerResponse( char *ptr, size_t size, size_t nmemb, void *userdata )
	{
		ThreadTaskGetMessage * perfomer = static_cast<ThreadTaskGetMessage *>(userdata);
		
		size_t total = size * nmemb;

		perfomer->writeResponse( ptr, total );
			
		return total;
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskGetMessage::ThreadTaskGetMessage()
		: m_id(0)
		, m_receiver(nullptr)
		, m_code(0)
		, m_successful(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskGetMessage::initialize( const String & _url, HttpRequestID _id, HttpReceiver * _receiver )
	{
		m_url = _url;
		m_id = _id;
		m_receiver = _receiver;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskGetMessage::_onRun()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskGetMessage::_onMain()
	{		
		/* init the curl session */
		CURL * curl = curl_easy_init();

		curl_easy_setopt( curl, CURLOPT_URL, m_url.c_str() );

		/* send all data to this function  */
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void *)this );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, &s_writeRequestPerformerResponse );

		/* get it! */
		CURLcode res = curl_easy_perform(curl);

		long http_code = 0;
		curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code );

		m_code = (uint32_t)http_code;		

		curl_easy_cleanup( curl );

		/* check for errors */
		if( res == CURLE_ABORTED_BY_CALLBACK )
		{
			return false;
		}
		else if( res != CURLE_OK )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskPostMessage::_onMain invalid post message %s error %d:%s"
				, m_url.c_str()
				, res
				, curl_easy_strerror(res)
				);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskGetMessage::writeResponse( char * _ptr, size_t _size )
	{
		m_response.append( _ptr, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskGetMessage::_onComplete( bool _successful )
	{
		m_receiver->onPostMessageComplete( m_id, m_response, m_code, _successful );
	}
}
