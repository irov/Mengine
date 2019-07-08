#include "DefaultResourcePrefetcher.h"

#include "Kernel/Logger.h"

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
        if( _resource->compile() == false )
        {
            LOGGER_ERROR( "invalid resource '%s:%s' prefetcher compile"
                , _resource->getType().c_str()
                , _resource->getName().c_str()
            );

            return false;
        }

        _observer->onPrefetchIgnored();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DefaultResourcePrefetcher::unfetch( const ResourcePtr & _resource )
    {
        _resource->release();

        return true;
    }
}