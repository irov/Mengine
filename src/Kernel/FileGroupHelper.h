#pragma once

#include "Interface/FileGroupInterface.h"

namespace Mengine
{
    namespace Helper
    {
        const Char * getFileGroupFullPath( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath );
    }
}