#include "cURLHeaderDataThreadTask.h"

#include "Interface/ConfigServiceInterface.h"

#include "cURLErrorHelper.h"

#include "Kernel/Logger.h"

#include "Config/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHeaderDataThreadTask::cURLHeaderDataThreadTask()
        : m_curl_header_list( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHeaderDataThreadTask::~cURLHeaderDataThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHeaderDataThreadTask::initialize( const String & _data )
    {
        m_data = _data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHeaderDataThreadTask::_onCURL( CURL * _curl )
    {
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_URL, m_url.c_str()) );
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POST, 1) );

        struct curl_slist * curl_header_list = nullptr;

        if( m_headers.empty() == false )
        {
            for( const String & header : m_headers )
            {
                const Char * header_buffer = header.c_str();

                curl_header_list = curl_slist_append( curl_header_list, header_buffer );
            }

            CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HTTPHEADER, curl_header_list) );

            m_curl_header_list = curl_header_list;
        }

        const String::value_type * data_buffer = m_data.c_str();
        String::size_type data_size = m_data.size();
        
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POSTFIELDS, data_buffer) );
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POSTFIELDSIZE, (long)data_size) );

        this->setupWriteResponse( _curl );

        if( CONFIG_VALUE( "HTTP", "Log", false ) == true )
        {
            Stringstream ss;

            for( const String & header : m_headers )
            {
                ss << header;
                ss << std::endl;
            }

            String header_str = ss.str();

            LOGGER_STATISTIC( "HTTP: header data url '%s' header:\n %s"
                , m_url.c_str()
                , header_str.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHeaderDataThreadTask::_onCURLCleanup( CURL * _curl )
    {
        MENGINE_UNUSED( _curl );

        if( m_curl_header_list != nullptr )
        {
            curl_slist_free_all( m_curl_header_list );
            m_curl_header_list = nullptr;
        }
    }
}
