#	include "ThreadTaskHeaderData.h"

#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include "curl/curl.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////
	static size_t s_writeRequestPerformerResponse( char *ptr, size_t size, size_t nmemb, void *userdata )
	{
        ThreadTaskHeaderData * perfomer = static_cast<ThreadTaskHeaderData *>(userdata);
		
		size_t total = size * nmemb;

		perfomer->writeResponse( ptr, total );
			
		return total;
	}
	//////////////////////////////////////////////////////////////////////////
    ThreadTaskHeaderData::ThreadTaskHeaderData()
		: m_id(0)
		, m_receiver(nullptr)
		, m_code(0)
		, m_successful(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskHeaderData::initialize( const String & _url, const TVectorString & _headers, const String & _data, HttpRequestID _id, HttpReceiver * _receiver )
	{
		m_url = _url;
        m_headers = _headers;
        m_data = _data;
		m_id = _id;
		m_receiver = _receiver;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskHeaderData::_onRun()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskHeaderData::_onMain()
	{		
		/* init the curl session */
		CURL * curl = curl_easy_init();

		/* specify URL to get */
		curl_easy_setopt( curl, CURLOPT_URL, m_url.c_str() );

        curl_easy_setopt( curl, CURLOPT_POST, 1 );

        struct curl_slist * curl_header_list = NULL;
        if( m_headers.empty() == false )
        {
            for( TVectorString::const_iterator
                it = m_headers.begin(),
                it_end = m_headers.end();
                it != it_end;
                ++it )
            {
                const String & header = *it;

                curl_header_list = curl_slist_append( curl_header_list, header.c_str() );
            }

            curl_easy_setopt( curl, CURLOPT_HTTPHEADER, curl_header_list );
        }

        curl_easy_setopt( curl, CURLOPT_POSTFIELDS, m_data.c_str() );
        
		/* send all data to this function  */
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void *)this );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, &s_writeRequestPerformerResponse );

		/* get it! */
		CURLcode res = curl_easy_perform(curl);

		long http_code = 0;
		curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code );

		m_code = (uint32_t)http_code;		
        
        if( curl_header_list != NULL )
        {
            curl_slist_free_all( curl_header_list );
        }

		curl_easy_cleanup( curl );

		/* check for errors */
		if( res == CURLE_ABORTED_BY_CALLBACK )
		{
			return false;
		}
		else if( res != CURLE_OK )
		{
			LOGGER_ERROR(m_serviceProvider)("ThreadTaskHeaderData::_onMain invalid post message %s error %d:%s"
				, m_url.c_str()
				, res
				, curl_easy_strerror(res)
				);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskHeaderData::writeResponse( char * _ptr, size_t _size )
	{
		m_response.append( _ptr, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskHeaderData::_onComplete( bool _successful )
	{
		m_receiver->onHttpRequestComplete( m_id, m_response, m_code, _successful );
	}
}
