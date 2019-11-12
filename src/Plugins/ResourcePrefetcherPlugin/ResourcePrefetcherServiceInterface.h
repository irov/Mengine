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
        virtual bool prefetchResource( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool unfetchResource( const ResourcePtr & _resource ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RESOURCEPREFETCHER_SERVICE()\
    ((Mengine::ResourcePrefetcherServiceInterface *)SERVICE_GET(Mengine::ResourcePrefetcherServiceInterface))
//////////////////////////////////////////////////////////////////////////