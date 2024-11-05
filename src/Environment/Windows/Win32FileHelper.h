#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/FilePath.h"
#include "Kernel/LambdaFilePath.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        time_t Win32FileTimeToUnixTime( const FILETIME * _filetime );

        HANDLE Win32CreateFile( const WChar * _filePath, DWORD _desiredAccess, DWORD _sharedMode, DWORD _creationDisposition );
        bool Win32ValidateFile( const WChar * _filePath );
        bool Win32ExistFile( const WChar * _filePath );

        bool Win32ExistDirectory( const WChar * _basePath, const WChar * _directory );
        bool Win32CreateDirectory( const WChar * _basePath, const WChar * _directory );

        typedef Lambda<bool(const WChar *)> LambdaListDirectoryFilePath;
        bool Win32ListDirectory( const WChar * _dir, const WChar * _mask, const WChar * _path, const LambdaListDirectoryFilePath & _lambda, bool * const _stop );
    }
}