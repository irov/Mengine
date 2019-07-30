#include "SDLFileHelper.h"

namespace Mengine
{
    namespace Helper
    {
        bool concatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * _concatenatePath, size_t _capacity )
        {
            size_t relationSize = _relationPath.size();
            size_t folderSize = _folderPath.size();
            size_t fileSize = _filePath.size();

            if( relationSize + folderSize + fileSize > _capacity )
            {
                return false;
            }

            strcpy( _concatenatePath, _relationPath.c_str() );
            strcat( _concatenatePath, _folderPath.c_str() );
            strcat( _concatenatePath, _filePath.c_str() );

            return true;
        }
    }
}
