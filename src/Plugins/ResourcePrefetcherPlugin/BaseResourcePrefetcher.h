#pragma once

#include "ResourcePrefetcherInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class BaseResourcePrefetcher
        : public ResourcePrefetcherInterface
        , public Factorable
    {
    public:
        BaseResourcePrefetcher();
        ~BaseResourcePrefetcher() override;

    public:
        bool prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch( const ResourcePtr & _resource ) override;

    protected:
        virtual bool _prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const = 0;
        virtual bool _unfetch( const ResourcePtr & _resource ) const = 0;
    };
}