#include "cURLGetMessageThreadTask.h"

#include "cURLErrorHelper.h"

#include "Kernel/ConfigHelper.h"
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
        this->setupWriteResponse( _curl );

        if( CONFIG_VALUE( "HTTP", "Log", false ) == true )
        {
            LOGGER_STATISTIC( "HTTP: get message url '%s'"
                , m_url.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
