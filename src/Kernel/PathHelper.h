#pragma once

#include "Kernel/FilePath.h"
#include "Kernel/String.h"

#include "Config/InitializerList.h"

namespace Mengine
{
    namespace Helper
    {
        bool concatenateFilePath( InitializerList<FilePath> && _paths, Char * const _concatenatePath );
        FilePath concatenateFilePath( InitializerList<FilePath> && _paths );
        FilePath getPathFolder( const FilePath & _fullpath );
        FilePath replaceFileSpec( const FilePath & _fullpath, const Char * _fileSpec );
        FilePath unicodeToFilePath( const WString & _unicode );
        void pathCorrectBackslashW( WChar * const _out );
        void pathCorrectBackslashToW( WChar * const _out, const WChar * _in );
        void pathCorrectForwardslashW( WChar * const _filePath );
        void pathCorrectForwardslashA( Char * const _filePath );
        void pathRemoveBackslashW( WChar * const _filePath );
        bool pathRemoveFileSpecW( WChar * const _in );
        void pathCorrectBackslashA( Char * const _out );
        void pathCorrectBackslashToA( Char * const _out, const Char * _in );
        void pathRemoveBackslashA( Char * const _filePath );
        bool pathRemoveFileSpecA( Char * const _in );
    }
}