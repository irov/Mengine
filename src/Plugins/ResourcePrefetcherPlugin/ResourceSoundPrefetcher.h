#pragma once

#include "ResourcePrefetcherHelper.h"

#include "Engine/ResourceSound.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceSoundPrefetcher
        : public ResourcePrefetcherT<ResourceSoundPtr>
        , public Factorable
    {
    public:
        ResourceSoundPrefetcher();
        ~ResourceSoundPrefetcher() override;

    public:
        bool _prefetch( const ResourceSoundPtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool _unfetch( const ResourceSoundPtr & _resource ) override;
    };
}