#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class Win32SentryServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Win32SentryService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define WIN32_SENTRY_SERVICE()\
    ((Mengine::Win32SentryServiceInterface *)SERVICE_GET(Mengine::Win32SentryServiceInterface))
//////////////////////////////////////////////////////////////////////////