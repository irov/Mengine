#include "cURLHttpRequestDeleteMessageThreadTask.h"

#include "cURLHttpErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestDeleteMessageThreadTask::cURLHttpRequestDeleteMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestDeleteMessageThreadTask::~cURLHttpRequestDeleteMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpRequestDeleteMessageThreadTask::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestDeleteMessageThreadTask::_onCURL( CURL * _curl )
    {
        MENGINE_CURLCALL( curl_easy_setopt, ( _curl, CURLOPT_CUSTOMREQUEST, "DELETE" ) );

        this->setupWriteResponse( _curl );

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            LOGGER_STATISTIC( "HTTP: get message url '%s'"
                , m_url.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
