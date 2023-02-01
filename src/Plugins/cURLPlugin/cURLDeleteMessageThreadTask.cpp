#include "cURLDeleteMessageThreadTask.h"

#include "cURLErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLDeleteMessageThreadTask::cURLDeleteMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLDeleteMessageThreadTask::~cURLDeleteMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLDeleteMessageThreadTask::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLDeleteMessageThreadTask::_onCURL( CURL * _curl )
    {
        CURLCALL( curl_easy_setopt, ( _curl, CURLOPT_CUSTOMREQUEST, "DELETE" ) );

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
