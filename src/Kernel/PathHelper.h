#pragma once

#include "Kernel/FilePath.h"

#include "Config/String.h"

namespace Mengine
{
    namespace Helper
    {
        FilePath emptyPath();
        FilePath concatenationFilePath( const FilePath & _left, const FilePath & _right );
        bool makeFullPath( const ConstString & _fileGroupName, const FilePath & _fileName, FilePath & _fullPath );
        FilePath getPathFolder( const FilePath & _fullpath );
        FilePath unicodeToFilePath( const WString & _unicode );
        void pathCorrectBackslashW( WChar * _out );
        void pathCorrectBackslashToW( WChar * _out, const WChar * _in );
        void pathCorrectForwardslashW( WChar * _path );
        void pathCorrectForwardslashA( Char * _path );
        void pathRemoveBackslashW( WChar * _path );
        bool pathRemoveFileSpecW( WChar * _in );
        void pathCorrectBackslashA( Char * _out );
        void pathCorrectBackslashToA( Char * _out, const Char * _in );
        void pathRemoveBackslashA( Char * _path );
        bool pathRemoveFileSpecA( Char * _in );
    }
}