#pragma once

#include "Kernel/FilePath.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t Win32ConcatenateFilePathA( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath, size_t _capacity );
        size_t Win32ConcatenateFilePathW( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * const _concatenatePath, size_t _capacity );
        size_t Win32ConcatenateFilePathTempA( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _concatenatePath, size_t _capacity );
        size_t Win32ConcatenateFilePathTempW( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * const _concatenatePath, size_t _capacity );
    }
}