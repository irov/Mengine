#pragma once

#include "Interface/ArchivatorInterface.h"

#include "ResourcePrefetcherInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ArchiveResourcePrefetcher
        : public ResourcePrefetcherInterface
        , public Factorable
    {
    public:
        ArchiveResourcePrefetcher();
        ~ArchiveResourcePrefetcher() override;

    public:
        void setArchivator( const ArchivatorInterfacePtr & _archivator );
        const ArchivatorInterfacePtr & getArchivator() const;

    public:
        bool prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch( const ResourcePtr & _resource ) override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArchiveResourcePrefetcher> ArchiveResourcePrefetcherPtr;
    //////////////////////////////////////////////////////////////////////////
}