#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/ThreadId.h"

namespace Mengine
{
    class PlatformSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PlatformSystem" );

    public:
        virtual bool beginThread( ThreadId _threadId ) = 0;
        virtual void endThread( ThreadId _threadId ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PLATFORM_SYSTEM()\
    ((Mengine::PlatformSystemInterface *)SERVICE_GET(Mengine::PlatformSystemInterface))
//////////////////////////////////////////////////////////////////////////