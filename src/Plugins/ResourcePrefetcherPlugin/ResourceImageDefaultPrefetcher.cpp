#include "ResourceImageDefaultPrefetcher.h"

#include "Interface/PrefetcherServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultPrefetcher::ResourceImageDefaultPrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefaultPrefetcher::~ResourceImageDefaultPrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefaultPrefetcher::_prefetch( const ResourceImageDefaultPtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = _resource->getFilePath();
        const ConstString & codecType = _resource->getCodecType();

        if( PREFETCHER_SERVICE()
            ->prefetchImageDecoder( fileGroup, filePath, codecType, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefaultPrefetcher::_unfetch( const ResourceImageDefaultPtr & _resource )
    {
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = _resource->getFilePath();

        bool successful = PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );

        return successful;
    }
}