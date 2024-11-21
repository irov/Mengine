#include "FileGroupHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        PathString getFileGroupFullPath( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
        {
            Path fullPath = {'\0'};

            _fileGroup->getFullPath( _filePath, fullPath );

            return PathString( fullPath );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}