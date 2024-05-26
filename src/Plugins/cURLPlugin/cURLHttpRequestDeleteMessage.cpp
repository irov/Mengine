#include "cURLHttpRequestDeleteMessage.h"

#include "cURLHttpErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/HttpLogger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestDeleteMessage::cURLHttpRequestDeleteMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestDeleteMessage::~cURLHttpRequestDeleteMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestDeleteMessage::_onCURL( CURL * _curl )
    {
        MENGINE_CURLCALL( curl_easy_setopt, ( _curl, CURLOPT_CUSTOMREQUEST, "DELETE" ) );

        this->setupWriteResponse( _curl );

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            LOGGER_HTTP_STATISTIC( "[HTTP] delete message url '%s'"
                , m_url.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
