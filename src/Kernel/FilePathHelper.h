#pragma once

#include "Kernel/FilePath.h"

namespace Mengine
{
    namespace Helper
    {
        FilePath emptyPath();
        FilePath concatenationFilePath( const FilePath & _left, const FilePath & _right );
        bool makeFullPath( const ConstString & _fileGroupName, const FilePath & _fileName, FilePath & _fullPath );
        FilePath getPathFolder( const FilePath & _fullpath );
        FilePath unicodeToFilePath( const WString & _unicode );
        void pathCorrectBackslash( WChar * _out );
        void pathCorrectBackslashTo( WChar * _out, const WChar * _in );
        void pathCorrectForwardslash( WChar * _out );
        void pathRemoveBackslash( WChar * _path );
        bool pathRemoveFileSpec( WChar * _in );
        void pathCorrectBackslashA( Char * _out );
        void pathCorrectBackslashToA( Char * _out, const Char * _in );
        void pathRemoveBackslashA( Char * _path );
        bool pathRemoveFileSpecA( Char * _in );
    }
}