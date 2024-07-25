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
    bool DefaultResourcePrefetcher::_prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const
    {
        if( _resource->compile() == false )
        {
            LOGGER_ERROR( "invalid compile group '%s' resource type '%s' name '%s'"
                , _resource->getGroupName().c_str()
                , _resource->getType().c_str()
                , _resource->getName().c_str()
            );

            return false;
        }

        _observer->onPrefetchIgnored();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DefaultResourcePrefetcher::_unfetch( const ResourcePtr & _resource ) const
    {
        _resource->release();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}