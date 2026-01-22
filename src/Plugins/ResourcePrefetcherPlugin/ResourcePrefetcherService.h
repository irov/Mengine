#pragma once

#include "ResourcePrefetcherServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

namespace Mengine
{    
    class ResourcePrefetcherService
        : public ServiceBase<ResourcePrefetcherServiceInterface>
    {
        DECLARE_FACTORABLE( ResourcePrefetcherService );

    public:
        ResourcePrefetcherService();
        ~ResourcePrefetcherService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool prefetchResource( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetchResource( const ResourcePtr & _resource ) override;
    };
}