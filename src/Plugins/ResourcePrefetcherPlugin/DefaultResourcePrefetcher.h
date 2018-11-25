#pragma once

#include "ResourcePrefetcherInterface.h"

#include "Engine/ResourceSound.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DefaultResourcePrefetcher
        : public ResourcePrefetcherInterface
        , public Factorable
    {
    public:
        DefaultResourcePrefetcher();
        ~DefaultResourcePrefetcher() override;

    public:
        bool prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch( const ResourcePtr & _resource ) override;
    };
}