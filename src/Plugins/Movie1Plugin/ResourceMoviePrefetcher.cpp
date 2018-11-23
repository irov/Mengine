#include "ResourceMoviePrefetcher.h"

#include "Interface/PrefetcherServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMoviePrefetcher::ResourceMoviePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMoviePrefetcher::~ResourceMoviePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMoviePrefetcher::_prefetch( const ResourceMoviePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
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
    bool ResourceMoviePrefetcher::_unfetch( const ResourceMoviePtr & _resource )
    {
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = _resource->getFilePath();

        bool successful = PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );

        return successful;
    }
}