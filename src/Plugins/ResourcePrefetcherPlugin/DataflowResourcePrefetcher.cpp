#include "DataflowResourcePrefetcher.h"

#include "Interface/PrefetcherServiceInterface.h"

#include "Kernel/Content.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DataflowResourcePrefetcher::DataflowResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowResourcePrefetcher::~DataflowResourcePrefetcher()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowResourcePrefetcher::prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        Content * content = _resource->getContent();

        MENGINE_ASSERTION_FATAL( content != nullptr, ("resource '%s' is not contentable"
            , _resource->getType().c_str()
            ) );

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = content->getFilePath();
        const ConstString & dataflowType = content->getDataflowType();

        if( PREFETCHER_SERVICE()
            ->prefetchData( fileGroup, filePath, dataflowType, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool DataflowResourcePrefetcher::unfetch( const ResourcePtr & _resource )
    {
        Content * content = _resource->getContent();

        MENGINE_ASSERTION_FATAL( content != nullptr, ("resource '%s' is not contentable"
            , _resource->getType().c_str()
            ) );

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        bool successful = PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );

        return successful;
    }
}