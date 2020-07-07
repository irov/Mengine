#pragma once

#include "Kernel/FilePath.h"
#include "Kernel/String.h"

namespace Mengine
{
    namespace Helper
    {
        bool concatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath, size_t _capacity );
        bool concatenateFilePathTemp( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath, size_t _capacity );
    };
}
