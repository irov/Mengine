#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class EnumeratorServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "EnumeratorService" )

    public:
        virtual uint32_t generateUniqueIdentity() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ENUMERATOR_SERVICE()\
    ((Mengine::EnumeratorServiceInterface*)SERVICE_GET(Mengine::EnumeratorServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define GENERATE_UNIQUE_IDENTITY()\
    ENUMERATOR_SERVICE()->generateUniqueIdentity()
//////////////////////////////////////////////////////////////////////////