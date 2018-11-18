#pragma once

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherHelper.h"

#include "ResourceMovie.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceMoviePrefetcher
        : public ResourcePrefetcherT<ResourceMoviePtr>
        , public Factorable
    {
    public:
        ResourceMoviePrefetcher();
        ~ResourceMoviePrefetcher() override;

    public:
        bool _prefetch( const ResourceMoviePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool _unfetch( const ResourceMoviePtr & _resource ) override;
    };
}