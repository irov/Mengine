#include "DataflowResourcePrefetcher.h"

#include "Interface/ContentInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrefetcherHelper.h"

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
        const ContentInterfacePtr & content = _resource->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        const FilePath & filePath = content->getFilePath();

        const ConstString & groupName = _resource->getGroupName();

        DataflowContext context;
        context.filePath = filePath;
        context.groupName = groupName;

        if( Helper::prefetchData( content, &context, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool DataflowResourcePrefetcher::_unfetch( const ResourcePtr & _resource ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        bool successful = Helper::unfetch( content );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}