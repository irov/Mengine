#include "cURLHeaderDataThreadTask.h"

#include "Interface/ConfigInterface.h"

#include "Kernel/Logger.h"

#include "Config/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHeaderDataThreadTask::cURLHeaderDataThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHeaderDataThreadTask::~cURLHeaderDataThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHeaderDataThreadTask::initialize( const String & _url, const VectorString & _headers, const String & _data )
    {
        m_url = _url;
        m_headers = _headers;
        m_data = _data;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHeaderDataThreadTask::_onCURL( CURL * _curl )
    {
        /* specify URL to get */
        curl_easy_setopt( _curl, CURLOPT_URL, m_url.c_str() );

        curl_easy_setopt( _curl, CURLOPT_POST, 1 );

        struct curl_slist * curl_header_list = NULL;
        if( m_headers.empty() == false )
        {
            for( VectorString::const_iterator
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
            Stringstream ss;

            for( VectorString::const_iterator
                it = m_headers.begin(),
                it_end = m_headers.end();
                it != it_end;
                ++it )
            {
                ss << *it;
                ss << std::endl;
            }

            String header_str = ss.str();

            LOGGER_STATISTIC( "HTTP: header data url '%s' header:\n %s"
                , m_url.c_str()
                , header_str.c_str()
            );
        }
    }
}
