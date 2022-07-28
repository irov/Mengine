#include "cURLPostMessageThreadTask.h"

#include "cURLErrorHelper.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLPostMessageThreadTask::cURLPostMessageThreadTask()
        : m_curl_formpost( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLPostMessageThreadTask::~cURLPostMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLPostMessageThreadTask::initialize( const cURLPostParams & _params )
    {
        m_params = _params;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPostMessageThreadTask::_onCURL( CURL * _curl )
    {
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POST, 1) );

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
                LOGGER_ERROR( "url '%s' param '%s' value '%s' get formadd error [%u]"
                    , m_url.c_str()
                    , key.c_str()
                    , value.c_str()
                    , code_formadd
                );
            }
        }

        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HTTPPOST, formpost) );

        m_curl_formpost = formpost;

        /* send all data to this function  */
        this->setupWriteResponse( _curl );

        if( CONFIG_VALUE( "cURLPlugin", "HTTPLog", false ) == true )
        {
            Stringstream ss;

            for( auto && [key, value] : m_params )
            {
                ss << key.c_str() << ": " << value;
                ss << std::endl;
            }

            String params_str = ss.str();

            LOGGER_STATISTIC( "HTTP: post message url '%s' params:\n %s"
                , m_url.c_str()
                , params_str.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPostMessageThreadTask::_onCURLCleanup( CURL * _curl )
    {
        MENGINE_UNUSED( _curl );

        if( m_curl_formpost != nullptr )
        {
            ::curl_formfree( m_curl_formpost );
            m_curl_formpost = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
