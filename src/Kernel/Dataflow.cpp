#include "Dataflow.h"

#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/DataServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        DataInterfacePointer getDataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const DataflowContext * _context, const DocumentPtr & _doc )
        {
            DataInterfacePtr prefetch_data;
            if( PREFETCHER_SERVICE()
                ->getData( _fileGroup, _filePath, &prefetch_data ) == true )
            {
                return prefetch_data;
            }

            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "dataflow invalid open file '%s/%s'"
                , _fileGroup->getFolderPath().c_str()
                , _filePath.c_str()
            );

            DataInterfacePtr data = DATA_SERVICE()
                ->dataflow( _dataflow, stream, _context, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( data, "dataflow invalid create data for file '%s/%s'"
                , _fileGroup->getFolderPath().c_str()
                , _filePath.c_str()
            );

            return data;
        }
        //////////////////////////////////////////////////////////////////////////
        DataInterfacePointer popDataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const DataflowContext * _context, const DocumentPtr & _doc )
        {
            DataInterfacePtr prefetch_data;
            if( PREFETCHER_SERVICE()
                ->popData( _fileGroup, _filePath, &prefetch_data ) == true )
            {
                return prefetch_data;
            }

            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "dataflow invalid open file '%s/%s'"
                , _fileGroup->getFolderPath().c_str()
                , _filePath.c_str()
            );

            DataInterfacePtr data = DATA_SERVICE()
                ->dataflow( _dataflow, stream, _context, _doc );

            stream = nullptr;

            MENGINE_ASSERTION_MEMORY_PANIC( data, "dataflow invalid create data for file '%s/%s'"
                , _fileGroup->getFolderPath().c_str()
                , _filePath.c_str()
            );

            return data;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}