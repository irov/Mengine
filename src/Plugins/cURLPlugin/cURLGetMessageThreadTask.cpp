#include "cURLGetMessageThreadTask.h"

#include "Interface/ConfigServiceInterface.h"

#include "cURLErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLGetMessageThreadTask::cURLGetMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLGetMessageThreadTask::~cURLGetMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLGetMessageThreadTask::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLGetMessageThreadTask::_onCURL( CURL * _curl )
    {
        CURLCALL( curl_easy_setopt, (_curl, CURLOPT_URL, m_url.c_str()) );

        this->setupWriteResponse( _curl );

        if( CONFIG_VALUE( "HTTP", "Log", false ) == true )
        {
            LOGGER_STATISTIC( "HTTP: get message url '%s'"
                , m_url.c_str()
            );
        }
    }
}
