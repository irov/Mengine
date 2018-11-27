#pragma once

#include "ResourcePrefetcherInterface.h"

#include "Engine/ResourceImageDefault.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ImageDecoderResourcePrefetcher
        : public ResourcePrefetcherInterface
        , public Factorable
    {
    public:
        ImageDecoderResourcePrefetcher();
        ~ImageDecoderResourcePrefetcher() override;

    public:
        bool prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch( const ResourcePtr & _resource ) override;
    };
}