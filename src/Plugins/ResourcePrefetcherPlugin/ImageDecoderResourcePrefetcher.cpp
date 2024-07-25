#include "ImageDecoderResourcePrefetcher.h"

#include "Kernel/PrefetcherHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderResourcePrefetcher::ImageDecoderResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderResourcePrefetcher::~ImageDecoderResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderResourcePrefetcher::_prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        MENGINE_ASSERTION_FATAL( content != nullptr, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        if( Helper::prefetchImageDecoder( content, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderResourcePrefetcher::_unfetch( const ResourcePtr & _resource ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        MENGINE_ASSERTION_FATAL( content != nullptr, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        bool successful = Helper::unfetch( content );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}