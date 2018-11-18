#pragma once

#include "ResourcePrefetcherHelper.h"

#include "Engine/ResourceImageDefault.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceImageDefaultPrefetcher
        : public ResourcePrefetcherT<ResourceImageDefaultPtr>
        , public Factorable
    {
    public:
        ResourceImageDefaultPrefetcher();
        ~ResourceImageDefaultPrefetcher() override;

    public:
        bool _prefetch( const ResourceImageDefaultPtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool _unfetch( const ResourceImageDefaultPtr & _resource ) override;
    };
}