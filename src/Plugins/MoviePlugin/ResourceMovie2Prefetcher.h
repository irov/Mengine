#pragma once

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherHelper.h"

#include "ResourceMovie2.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceMovie2Prefetcher
        : public ResourcePrefetcherT<ResourceMovie2Ptr>
        , public Factorable
    {
    public:
        ResourceMovie2Prefetcher();
        ~ResourceMovie2Prefetcher() override;

    public:
        bool _prefetch( const ResourceMovie2Ptr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool _unfetch( const ResourceMovie2Ptr & _resource ) override;
    };
}