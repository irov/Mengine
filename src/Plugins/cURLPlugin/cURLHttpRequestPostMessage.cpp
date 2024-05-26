#include "cURLHttpRequestPostMessage.h"

#include "cURLHttpErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/HttpLogger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestPostMessage::cURLHttpRequestPostMessage()
        : m_curl_formpost( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestPostMessage::~cURLHttpRequestPostMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestPostMessage::setPostProperties( const HttpRequestPostProperties & _params )
    {
        m_params = _params;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestPostProperties & cURLHttpRequestPostMessage::getPostProperties() const
    {
        return m_params;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestPostMessage::_onCURL( CURL * _curl )
    {
        MENGINE_CURLCALL( curl_easy_setopt, ( _curl, CURLOPT_CUSTOMREQUEST, "POST" ) );

        struct curl_httppost * lastptr = nullptr;
        struct curl_httppost * formpost = nullptr;

        for( auto && [key, value] : m_params )
        {
            /* Fill in the  field */
            CURLFORMcode code_formadd = curl_formadd( &formpost,
                &lastptr,
                CURLFORM_COPYNAME, key.c_str(),
                CURLFORM_COPYCONTENTS, value.c_str(),
                CURLFORM_END );

            if( code_formadd != CURL_FORMADD_OK )
            {
                LOGGER_HTTP_ERROR( "[HTTP] url '%s' param '%s' value '%s' get formadd error [%u]"
                    , m_url.c_str()
                    , key.c_str()
                    , value.c_str()
                    , code_formadd
                );
            }
        }

        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HTTPPOST, formpost) );

        m_curl_formpost = formpost;

        /* send all data to this function  */
        this->setupWriteResponse( _curl );

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            Stringstream ss;

            for( auto && [key, value] : m_params )
            {
                ss << key.c_str() << ": " << value;
                ss << std::endl;
            }

            String params_str = ss.str();

            LOGGER_HTTP_STATISTIC( "[HTTP] post message url '%s' params:\n %s"
                , m_url.c_str()
                , params_str.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestPostMessage::_onCURLCleanup( CURL * _curl )
    {
        MENGINE_UNUSED( _curl );

        if( m_curl_formpost != nullptr )
        {
            curl_formfree( m_curl_formpost );
            m_curl_formpost = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
