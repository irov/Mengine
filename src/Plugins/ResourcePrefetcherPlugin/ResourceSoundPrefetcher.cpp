#include "ResourceSoundPrefetcher.h"

#include "Interface/PrefetcherServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceSoundPrefetcher::ResourceSoundPrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSoundPrefetcher::~ResourceSoundPrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSoundPrefetcher::_prefetch( const ResourceSoundPtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = _resource->getFilePath();
        const ConstString & codecType = _resource->getCodecType();

        if( PREFETCHER_SERVICE()
            ->prefetchSoundDecoder( fileGroup, filePath, codecType, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool ResourceSoundPrefetcher::_unfetch( const ResourceSoundPtr & _resource )
    {
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = _resource->getFilePath();

        bool successful = PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );

        return successful;
    }
}