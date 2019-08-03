#pragma once

#include "Kernel/FilePath.h"

#include "Config/String.h"

namespace Mengine
{
    namespace Helper
    {
        bool concatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * _concatenatePath, size_t _capacity );
    };
}
