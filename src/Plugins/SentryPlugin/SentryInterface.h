#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class SentryServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SentryService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define SENTRY_SERVICE()\
    ((Mengine::SentryServiceInterface *)SERVICE_GET(Mengine::SentryServiceInterface))
//////////////////////////////////////////////////////////////////////////