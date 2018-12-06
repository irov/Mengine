#pragma once

#include "Interface/ServiceInterface.h"

#include "ResourceValidatorInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class ResourceValidateServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ResourceValidateService" );

    public:
        virtual bool validResource( const ResourcePtr & _resource ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RESOURCEVALIDATE_SERVICE()\
    ((Mengine::ResourceValidateServiceInterface *)SERVICE_GET(Mengine::ResourceValidateServiceInterface))
//////////////////////////////////////////////////////////////////////////