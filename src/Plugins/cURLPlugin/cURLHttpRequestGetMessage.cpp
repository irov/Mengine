#include "cURLHttpRequestGetMessage.h"

#include "cURLHttpErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/HttpLogger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestGetMessage::cURLHttpRequestGetMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestGetMessage::~cURLHttpRequestGetMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestGetMessage::_onCURL( CURL * _curl )
    {
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_CUSTOMREQUEST, "GET") );

        this->setupWriteResponse( _curl );

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            LOGGER_HTTP_STATISTIC( "[HTTP] get message url '%s'"
                , m_url.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
