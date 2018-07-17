#include "ThreadTaskHeaderData.h"

#include "Interface/ConfigInterface.h"

#include "Kernel/Logger.h"

#include <sstream>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    ThreadTaskHeaderData::ThreadTaskHeaderData()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskHeaderData::initialize( const String & _url, const TVectorString & _headers, const String & _data )
	{
		m_url = _url;
        m_headers = _headers;
        m_data = _data;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskHeaderData::_onCURL( CURL * _curl )
	{		
		/* specify URL to get */
		curl_easy_setopt( _curl, CURLOPT_URL, m_url.c_str() );

        curl_easy_setopt( _curl, CURLOPT_POST, 1 );

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

            curl_easy_setopt( _curl, CURLOPT_HTTPHEADER, curl_header_list );
        }

        curl_easy_setopt( _curl, CURLOPT_POSTFIELDS, m_data.c_str() );
        
        this->setupWriteResponse( _curl );

        if( CONFIG_VALUE( "HTTP", "Log", false ) == true )
        {
            std::stringstream ss;

            for( TVectorString::const_iterator
                it = m_headers.begin(),
                it_end = m_headers.end();
                it != it_end;
                ++it )
            {
                ss << *it;
                ss << std::endl;
            }

            std::string header_str = ss.str();

            LOGGER_STATISTIC("HTTP: header data url '%s' header:\n %s"
                , m_url.c_str()
                , header_str.c_str()
                );
        }
	}
}
