#pragma once

#include "Interface/PrefetcherObserverInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceConverterInterface
        : public Mixin
    {
    public:
        virtual bool convert( const ResourcePtr & _resource ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceConverterInterface> ResourceConverterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}