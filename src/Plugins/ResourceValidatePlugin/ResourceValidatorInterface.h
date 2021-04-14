#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceValidatorInterface
        : public ServantInterface
    {
    public:
        virtual bool validate( const ResourcePtr & _resource ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceValidatorInterface> ResourceValidatorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}