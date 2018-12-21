#include "Dataflow.h"

#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/DataServiceInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        DataPointer dataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _dataflowType )
        {
            DataInterfacePtr prefetch_data;
            if( PREFETCHER_SERVICE()
                ->getData( _fileGroup, _filePath, prefetch_data ) == true )
            {
                return prefetch_data;
            }

            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _filePath, false );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "dataflow invalid open file '%s/%s'"
                    , _fileGroup->getFolderPath().c_str()
                    , _filePath.c_str()
                );

                return nullptr;
            }

            DataflowInterfacePtr dataflow = VOCALUBARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), _dataflowType );

            if( dataflow == nullptr )
            {
                LOGGER_ERROR( "dataflow invalid create '%s'"
                    , _dataflowType.c_str()
                );

                return nullptr;
            }

            DataInterfacePtr data = DATA_SERVICE()
                ->dataflow( dataflow, stream );

            if( data == nullptr )
            {
                LOGGER_ERROR( "dataflow invalid create data '%s' for file '%s'"
                    , _dataflowType.c_str()
                    , _filePath.c_str()
                );

                return nullptr;
            }

            return data;
        }
        //////////////////////////////////////////////////////////////////////////
        DataPointer dataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow )
        {
            DataInterfacePtr prefetch_data;
            if( PREFETCHER_SERVICE()
                ->getData( _fileGroup, _filePath, prefetch_data ) == true )
            {
                return prefetch_data;
            }

            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _filePath, false );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "dataflow invalid open file '%s/%s'"
                    , _fileGroup->getFolderPath().c_str()
                    , _filePath.c_str()
                );

                return nullptr;
            }

            DataInterfacePtr data = DATA_SERVICE()
                ->dataflow( _dataflow, stream );

            if( data == nullptr )
            {
                LOGGER_ERROR( "dataflow invalid create data for file '%s'"
                    , _filePath.c_str()
                );

                return nullptr;
            }

            return data;
        }
    }
}