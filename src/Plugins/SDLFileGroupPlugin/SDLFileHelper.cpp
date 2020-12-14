#include "SDLFileHelper.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool concatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath, size_t _capacity )
        {
            FilePath::size_type relationSize = _relationPath.size();
            FilePath::size_type folderSize = _folderPath.size();
            FilePath::size_type fileSize = _filePath.size();

            FilePath::size_type filePathSize = relationSize + folderSize + fileSize;

            if( filePathSize > _capacity )
            {
                LOGGER_ERROR( "invalid full path max size:\nrelation: %s\nfolder: %s\nfile: %s\ntotal size %zu [max size: %zu]"
                    , _relationPath.c_str()
                    , _folderPath.c_str()
                    , _filePath.c_str()
                    , filePathSize
                    , _capacity
                );

                return false;
            }

            MENGINE_STRCPY( _concatenatePath, _relationPath.c_str() );
            MENGINE_STRCAT( _concatenatePath, _folderPath.c_str() );
            MENGINE_STRCAT( _concatenatePath, _filePath.c_str() );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool concatenateFilePathTemp( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath, size_t _capacity )
        {
            FilePath::size_type relationSize = _relationPath.size();
            FilePath::size_type folderSize = _folderPath.size();
            FilePath::size_type fileSize = _filePath.size();

            FilePath::size_type filePathSize = relationSize + folderSize + fileSize + 5;

            if( filePathSize > _capacity )
            {
                LOGGER_ERROR( "invalid full path max size:\nrelation: %s\nfolder: %s\nfile: %s\ntotal size %zu [max size: %zu]"
                    , _relationPath.c_str()
                    , _folderPath.c_str()
                    , _filePath.c_str()
                    , filePathSize
                    , _capacity
                );

                return false;
            }

            MENGINE_STRCPY( _concatenatePath, _relationPath.c_str() );
            MENGINE_STRCAT( _concatenatePath, _folderPath.c_str() );
            MENGINE_STRCAT( _concatenatePath, _filePath.c_str() );
            MENGINE_STRCAT( _concatenatePath, ".~tmp" );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
