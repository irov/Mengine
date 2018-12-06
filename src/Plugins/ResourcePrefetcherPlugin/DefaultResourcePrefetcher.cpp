#include "DefaultResourcePrefetcher.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DefaultResourcePrefetcher::DefaultResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DefaultResourcePrefetcher::~DefaultResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DefaultResourcePrefetcher::prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        _resource->incrementReference();

        _observer->onPrefetchIgnored();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DefaultResourcePrefetcher::unfetch( const ResourcePtr & _resource )
    {
        _resource->decrementReference();

        return true;
    }
}