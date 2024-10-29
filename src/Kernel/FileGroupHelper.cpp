#include "FileGroupHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        PathString getFileGroupFullPath( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
        {
            Char fullPath[MENGINE_MAX_PATH + 1] = {'\0'};

            _fileGroup->getFullPath( _filePath, fullPath );

            return PathString( fullPath );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}