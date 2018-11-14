#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceValidatorInterface
        : public Mixin
    {
    public:
        virtual bool validate( const ResourcePtr & _resource ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceValidatorInterface> ResourceValidatorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}