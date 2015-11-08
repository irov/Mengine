#   include "MarmaladeLayer.h"

SERVICE_FACTORY( MarmaladeLayer, Menge::MarmaladeLayer );

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MarmaladeLayer::MarmaladeLayer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MarmaladeLayer::~MarmaladeLayer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeLayer::concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity )
    {
        size_t folderSize = _folder.size();
		size_t fileNameSize = _fileName.size();

        if( folderSize + fileNameSize > _capacity )
        {
            return false;
        }

        strcpy( _filePath, _folder.c_str() );
        strcat( _filePath, _fileName.c_str() );

        return true;
    }
}