#pragma once

#include "Kernel/FilePath.h"
#include "Config/Char.h"

#include "Environment/Windows/WindowsIncluder.h"

#ifndef MENGINE_PATH_INVALID_LENGTH
#define MENGINE_PATH_INVALID_LENGTH (~0U)
#endif

namespace Mengine
{
    namespace Helper
    {
        HANDLE Win32CreateFile( const WChar * _filePath, DWORD _desiredAccess, DWORD _sharedMode, DWORD _creationDisposition );
        bool Win32ValidateFile( const WChar * _path );
        size_t Win32ConcatenateFilePathA( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * _concatenatePath, size_t _capacity );
        size_t Win32ConcatenateFilePathW( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * _concatenatePath, size_t _capacity );
    }
}