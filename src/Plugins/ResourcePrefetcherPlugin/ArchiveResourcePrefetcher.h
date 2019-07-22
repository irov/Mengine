#pragma once

#include "Interface/ArchivatorInterface.h"

#include "BaseResourcePrefetcher.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ArchiveResourcePrefetcher
        : public BaseResourcePrefetcher
    {
    public:
        ArchiveResourcePrefetcher();
        ~ArchiveResourcePrefetcher() override;

    public:
        void setArchivator( const ArchivatorInterfacePtr & _archivator );
        const ArchivatorInterfacePtr & getArchivator() const;

    public:
        bool _prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const override;
        bool _unfetch( const ResourcePtr & _resource ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArchiveResourcePrefetcher> ArchiveResourcePrefetcherPtr;
    //////////////////////////////////////////////////////////////////////////
}