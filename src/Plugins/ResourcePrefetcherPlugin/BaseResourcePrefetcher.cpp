#include "BaseResourcePrefetcher.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseResourcePrefetcher::BaseResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseResourcePrefetcher::~BaseResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseResourcePrefetcher::prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        bool successful = _resource->prefetch( [this, &_resource, &_observer]()
        {
            bool successful = this->_prefetch( _resource, _observer );

            return successful;
        } );

        if( successful == false )
        {
            LOGGER_WARNING( "invalid group '%s' resource '%s:%s' prefetch"
                , _resource->getGroupName().c_str()
                , _resource->getType().c_str()
                , _resource->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseResourcePrefetcher::unfetch( const ResourcePtr & _resource )
    {
        bool successful = _resource->unfetch( [this, &_resource]()
        {
            bool successful = this->_unfetch( _resource );

            return successful;
        } );

        if( successful == false )
        {
            LOGGER_WARNING( "invalid group '%s' resource '%s:%s' unfetch"
                , _resource->getGroupName().c_str()
                , _resource->getType().c_str()
                , _resource->getName().c_str()
            );

            return false;
        }

        return true;
    }
}