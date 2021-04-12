#pragma once

#include "BaseResourcePrefetcher.h"

#include "Kernel/ResourceImageDefault.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class ImageDecoderResourcePrefetcher
        : public BaseResourcePrefetcher
    {
        DECLARE_FACTORABLE( ImageDecoderResourcePrefetcher );

    public:
        ImageDecoderResourcePrefetcher();
        ~ImageDecoderResourcePrefetcher() override;

    public:
        bool _prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const override;
        bool _unfetch( const ResourcePtr & _resource ) const override;
    };
}