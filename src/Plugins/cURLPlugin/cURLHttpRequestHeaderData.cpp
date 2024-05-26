#include "cURLHttpRequestHeaderData.h"

#include "cURLHttpErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/HttpLogger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestHeaderData::cURLHttpRequestHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestHeaderData::~cURLHttpRequestHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestHeaderData::setData( const Data & _data )
    {
        m_data = _data;
    }
    //////////////////////////////////////////////////////////////////////////
    const Data & cURLHttpRequestHeaderData::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestHeaderData::_onCURL( CURL * _curl )
    {
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_CUSTOMREQUEST, "POST") );

        const Data::value_type * data_buffer = m_data.data();
        Data::size_type data_size = m_data.size();

        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POSTFIELDS, data_buffer) );
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POSTFIELDSIZE, (long)data_size) );

        this->setupWriteResponse( _curl );

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            Stringstream ss;

            for( const String & header : m_headers )
            {
                ss << header;
                ss << std::endl;
            }

            String header_str = ss.str();

            LOGGER_HTTP_STATISTIC( "[HTTP] header data url '%s' header:\n %s"
                , m_url.c_str()
                , header_str.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestHeaderData::_onCURLCleanup( CURL * _curl )
    {
        MENGINE_UNUSED( _curl );
    }
    //////////////////////////////////////////////////////////////////////////
}
