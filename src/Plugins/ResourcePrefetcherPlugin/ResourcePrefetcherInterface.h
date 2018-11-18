#pragma once

#include "Interface/PrefetcherObserverInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourcePrefetcherInterface
        : public Mixin
    {
    public:
        virtual bool prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool unfetch( const ResourcePtr & _resource ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourcePrefetcherInterface> ResourcePrefetcherInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}