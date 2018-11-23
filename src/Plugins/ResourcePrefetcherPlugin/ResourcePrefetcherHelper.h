#pragma once

#include "ResourcePrefetcherInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ResourcePrefetcherT
        : public ResourcePrefetcherInterface
    {
    public:
        bool prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) override
        {
            const T & resourceT = stdex::intrusive_static_cast<T>(_resource);

            return this->_prefetch( resourceT, _observer );
        }

        bool unfetch( const ResourcePtr & _resource ) override
        {
            const T & resourceT = stdex::intrusive_static_cast<T>(_resource);

            return this->_unfetch( resourceT );
        }

    protected:
        virtual bool _prefetch( const T & _resource, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool _unfetch( const T & _resource ) = 0;
    };
}