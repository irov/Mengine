#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class EnumeratorServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "EnumeratorService" )

    public:
        virtual UniqueId generateUniqueIdentity() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ENUMERATOR_SERVICE()\
    ((Mengine::EnumeratorServiceInterface *)SERVICE_GET(Mengine::EnumeratorServiceInterface))
//////////////////////////////////////////////////////////////////////////