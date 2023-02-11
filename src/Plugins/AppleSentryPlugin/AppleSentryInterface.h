#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class AppleSentryServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleSentryService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_SENTRY_SERVICE()\
    ((Mengine::AppleSentryServiceInterface *)SERVICE_GET(Mengine::AppleSentryServiceInterface))
//////////////////////////////////////////////////////////////////////////