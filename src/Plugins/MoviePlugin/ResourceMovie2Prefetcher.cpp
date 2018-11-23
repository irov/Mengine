#include "ResourceMovie2Prefetcher.h"

#include "Interface/PrefetcherServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie2Prefetcher::ResourceMovie2Prefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie2Prefetcher::~ResourceMovie2Prefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2Prefetcher::_prefetch( const ResourceMovie2Ptr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = _resource->getFilePath();
        const ConstString & dataflowType = _resource->getDataflowType();

        if( PREFETCHER_SERVICE()
            ->prefetchData( fileGroup, filePath, dataflowType, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool ResourceMovie2Prefetcher::_unfetch( const ResourceMovie2Ptr & _resource )
    {
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = _resource->getFilePath();

        bool successful = PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );

        return successful;
    }
}