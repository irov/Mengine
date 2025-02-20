#include "Dataflow.h"

#include "Interface/DataServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrefetcherHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        DataInterfacePointer getDataflow( const ContentInterfacePtr & _content, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
        {
            DataInterfacePtr prefetch_data;
            if( Helper::getPrefetchData( _content, &prefetch_data ) == true )
            {
                return prefetch_data;
            }

            InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "dataflow invalid open file '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            const DataflowInterfacePtr & dataflow = _content->getDataflow();

            DataInterfacePtr data = DATA_SERVICE()
                ->dataflow( dataflow, stream, _context, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( data, "dataflow invalid create data for file '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return data;
        }
        //////////////////////////////////////////////////////////////////////////
        DataInterfacePointer popDataflow( const ContentInterfacePtr & _content, const DataflowInterfacePtr & _dataflow, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
        {
            DataInterfacePtr prefetch_data;
            if( Helper::popPrefetchData( _content, &prefetch_data ) == true )
            {
                return prefetch_data;
            }

            InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "dataflow invalid open file '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            DataInterfacePtr data = DATA_SERVICE()
                ->dataflow( _dataflow, stream, _context, _doc );

            stream = nullptr;

            MENGINE_ASSERTION_MEMORY_PANIC( data, "dataflow invalid create data for file '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return data;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}