#pragma once

#include "BaseResourcePrefetcher.h"

#include "Kernel/ResourceSound.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class SoundDecoderResourcePrefetcher
        : public BaseResourcePrefetcher
    {
    public:
        SoundDecoderResourcePrefetcher();
        ~SoundDecoderResourcePrefetcher() override;

    public:
        bool _prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const override;
        bool _unfetch( const ResourcePtr & _resource ) const override;
    };
}