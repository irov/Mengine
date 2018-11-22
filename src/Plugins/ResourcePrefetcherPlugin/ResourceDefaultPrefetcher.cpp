#include "ResourceDefaultPrefetcher.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceDefaultPrefetcher::ResourceDefaultPrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceDefaultPrefetcher::~ResourceDefaultPrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceDefaultPrefetcher::prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {        
        _resource->incrementReference();

        _observer->onPrefetchIgnored();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceDefaultPrefetcher::unfetch( const ResourcePtr & _resource )
    {
        _resource->decrementReference();

        return true;
    }
}