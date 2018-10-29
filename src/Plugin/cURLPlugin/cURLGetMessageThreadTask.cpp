#include "cURLGetMessageThreadTask.h"

#include "Interface/ConfigServiceInterface.h"

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
    bool cURLGetMessageThreadTask::initialize( const String & _url )
    {
        m_url = _url;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLGetMessageThreadTask::_onCURL( CURL * _curl )
    {
        curl_easy_setopt( _curl, CURLOPT_URL, m_url.c_str() );

        this->setupWriteResponse( _curl );

        if( CONFIG_VALUE( "HTTP", "Log", false ) == true )
        {
            LOGGER_STATISTIC( "HTTP: get message url '%s'"
                , m_url.c_str()
            );
        }
    }
}
