#pragma once

#include "ResourcePrefetcherServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Observable.h"
#include "Kernel/Hashtable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourcePrefetcherService
        : public ServiceBase<ResourcePrefetcherServiceInterface>
        , public Observable
    {
    public:
        ResourcePrefetcherService();
        ~ResourcePrefetcherService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void addResourcePrefetcher( const ConstString & _type, const ResourcePrefetcherInterfacePtr & _prefetcher ) override;
        void removeResourcePrefetcher( const ConstString & _type ) override;

    public:
        bool prefetchResource( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetchResource( const ResourcePtr & _resource ) override;
        
    protected:
        typedef Hashtable<ConstString, ResourcePrefetcherInterfacePtr> HashtableResourcePrefetchers;
        HashtableResourcePrefetchers m_prefetchers;
    };
}