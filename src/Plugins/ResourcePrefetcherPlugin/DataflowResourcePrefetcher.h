#pragma once

#include "ResourcePrefetcherInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DataflowResourcePrefetcher
        : public ResourcePrefetcherInterface
        , public Factorable
    {
    public:
        DataflowResourcePrefetcher();
        ~DataflowResourcePrefetcher() override;

    public:
        bool prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch( const ResourcePtr & _resource ) override;
    };
}