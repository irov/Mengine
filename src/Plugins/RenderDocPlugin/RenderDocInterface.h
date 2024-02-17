#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class RenderDocServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderDocService" )

    public:
    };
}
//////////////////////////////////////////////////////////////////////////
#define RENDERDOC_SERVICE()\
    ((Mengine::RenderDocServiceInterface *)SERVICE_GET(Mengine::RenderDocServiceInterface))
//////////////////////////////////////////////////////////////////////////
