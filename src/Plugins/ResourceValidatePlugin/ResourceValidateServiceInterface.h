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
        virtual void addResourceValidator( const ConstString & _type, const ResourceValidatorInterfacePtr & _validator ) = 0;
        virtual void removeResourceValidator( const ConstString & _type ) = 0;

    public:
        virtual bool validResource( const ResourcePtr & _resource ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RESOURCEVALIDATE_SERVICE()\
    ((Mengine::ResourceValidateServiceInterface *)SERVICE_GET(Mengine::ResourceValidateServiceInterface))
//////////////////////////////////////////////////////////////////////////