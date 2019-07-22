#pragma once

#include "BaseResourcePrefetcher.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DefaultResourcePrefetcher
        : public BaseResourcePrefetcher
    {
    public:
        DefaultResourcePrefetcher();
        ~DefaultResourcePrefetcher() override;

    public:
        bool _prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const override;
        bool _unfetch( const ResourcePtr & _resource ) const override;
    };
}