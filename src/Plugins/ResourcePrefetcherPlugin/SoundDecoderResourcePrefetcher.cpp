#include "SoundDecoderResourcePrefetcher.h"

#include "Interface/PrefetcherServiceInterface.h"

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
    bool SoundDecoderResourcePrefetcher::prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        Content * content = _resource->getContent();

        MENGINE_ASSERTION_FATAL( content != nullptr, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = content->getFilePath();
        const ConstString & codecType = content->getCodecType();

        if( PREFETCHER_SERVICE()
            ->prefetchSoundDecoder( fileGroup, filePath, codecType, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool SoundDecoderResourcePrefetcher::unfetch( const ResourcePtr & _resource )
    {
        Content * content = _resource->getContent();

        MENGINE_ASSERTION_FATAL( content != nullptr, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        bool successful = PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );

        return successful;
    }
}