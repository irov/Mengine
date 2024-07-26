#include "SoundDecoderResourcePrefetcher.h"

#include "Kernel/PrefetcherHelper.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderResourcePrefetcher::SoundDecoderResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderResourcePrefetcher::~SoundDecoderResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderResourcePrefetcher::_prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        MENGINE_ASSERTION_FATAL( content != nullptr, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        if( Helper::prefetchSoundDecoder( content, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool SoundDecoderResourcePrefetcher::_unfetch( const ResourcePtr & _resource ) const
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