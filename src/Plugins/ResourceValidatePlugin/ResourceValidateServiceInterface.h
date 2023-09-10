#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Resource.h"

namespace Mengine
{
    class ResourceValidateServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ResourceValidateService" );

    public:
        virtual bool validateResource( const ResourcePtr & _resource ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RESOURCEVALIDATE_SERVICE()\
    ((Mengine::ResourceValidateServiceInterface *)SERVICE_GET(Mengine::ResourceValidateServiceInterface))
//////////////////////////////////////////////////////////////////////////