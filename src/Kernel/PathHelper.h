#pragma once

#include "Kernel/FilePath.h"

#include "Config/String.h"

namespace Mengine
{
    namespace Helper
    {
        FilePath concatenationFilePath( const FilePath & _left, const FilePath & _right );
        bool makeFullPath( const ConstString & _fileGroupName, const FilePath & _filePath, FilePath & _fullPath );
        FilePath getPathFolder( const FilePath & _fullpath );
        FilePath unicodeToFilePath( const WString & _unicode );
        void pathCorrectBackslashW( WChar * _out );
        void pathCorrectBackslashToW( WChar * _out, const WChar * _in );
        void pathCorrectForwardslashW( WChar * _filePath );
        void pathCorrectForwardslashA( Char * _filePath );
        void pathRemoveBackslashW( WChar * _filePath );
        bool pathRemoveFileSpecW( WChar * _in );
        void pathCorrectBackslashA( Char * _out );
        void pathCorrectBackslashToA( Char * _out, const Char * _in );
        void pathRemoveBackslashA( Char * _filePath );
        bool pathRemoveFileSpecA( Char * _in );
    }
}