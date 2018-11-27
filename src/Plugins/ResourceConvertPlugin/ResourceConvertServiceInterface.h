#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class ResourceConvertServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ResourceConvertService" );
    };
}
//////////////////////////////////////////////////////////////////////////
#define RESOURCECONVERT_SERVICE()\
    ((Mengine::ResourceConvertServiceInterface *)SERVICE_GET(Mengine::ResourceConvertServiceInterface))
//////////////////////////////////////////////////////////////////////////