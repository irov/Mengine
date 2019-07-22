#include "DataflowResourcePrefetcher.h"

#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Content.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    bool DataflowResourcePrefetcher::_prefetch( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer ) const
    {
        Content * content = _resource->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, false, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = content->getFilePath();
        const ConstString & dataflowType = content->getDataflowType();

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        if( PREFETCHER_SERVICE()
            ->prefetchData( fileGroup, filePath, dataflow, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool DataflowResourcePrefetcher::_unfetch( const ResourcePtr & _resource ) const
    {
        Content * content = _resource->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, false, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        bool successful = PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );

        return successful;
    }
}