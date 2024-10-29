#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/PathString.h"

namespace Mengine
{
    namespace Helper
    {
        PathString getFileGroupFullPath( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath );
    }
}