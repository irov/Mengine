#pragma once

#include "ResourcePrefetcherInterface.h"

#include "Engine/ResourceSound.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceDefaultPrefetcher
        : public ResourcePrefetcherInterface
        , public Factorable
    {
    public:
        ResourceDefaultPrefetcher();
        ~ResourceDefaultPrefetcher() override;

    public:
        bool prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch( const ResourcePtr & _resource ) override;
    };
}