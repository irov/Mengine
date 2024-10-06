#include "FileGroupHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * getFileGroupFullPath( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
        {
            static MENGINE_THREAD_LOCAL Char fullPath[MENGINE_MAX_PATH + 1] = {'\0'};

            _fileGroup->getFullPath( _filePath, fullPath );

            return fullPath;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}