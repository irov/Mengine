#pragma once

#include "Interface/ServiceInterface.h"

#include "ResourcePrefetcherInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class ResourcePrefetcherServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ResourcePrefetcherService" );

    public:
        virtual void addResourcePrefetcher( const ConstString & _type, const ResourcePrefetcherInterfacePtr & _validator ) = 0;
        virtual void removeResourcePrefetcher( const ConstString & _type ) = 0;

    public:
        virtual bool prefetchResource( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool unfetchResource( const ResourcePtr & _resource ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RESOURCEPREFETCHER_SERVICE()\
    ((Mengine::ResourcePrefetcherServiceInterface *)SERVICE_GET(Mengine::ResourcePrefetcherServiceInterface))
//////////////////////////////////////////////////////////////////////////