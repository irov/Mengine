#pragma once

#include "Kernel/FilePath.h"
#include "Kernel/WString.h"

#include "Config/InitializerList.h"

namespace Mengine
{
    namespace Helper
    {
        bool concatenateFilePath( InitializerList<FilePath> && _paths, Char * const _concatenatePath );
        FilePath concatenateFilePath( InitializerList<FilePath> && _paths );
        FilePath getFolderPath( const FilePath & _path );
        FilePath replaceFileSpec( const FilePath & _path, const Char * _fileSpec );
        FilePath unicodeToFilePath( const WString & _unicode );        
        
        bool isCorrectFolderPathW( const WChar * _path );
        bool isCorrectFolderPathA( const Char * _path );

        void pathCombineW( WChar * const _out, const WChar * _base, const WChar * _path, WChar _delim );
        void pathCombineA( Char * const _out, const Char * _base, const Char * _path, Char _delim );
        void pathAppendW( WChar * const _base, const WChar * _path, WChar _delim );
        void pathAppendA( Char * const _base, const Char * _path, Char _delim );
        void pathCorrectFolderPathW( WChar * const _folderPath, WChar _delim );
        void pathCorrectFolderPathA( Char * const _folderPath, Char _delim );
        void pathCorrectBackslashW( WChar * const _filePath );
        void pathCorrectBackslashA( Char * const _filePath );
        void pathCorrectBackslashToW( WChar * const _out, const WChar * _filePath );
        void pathCorrectBackslashToA( Char * const _out, const Char * _filePath );
        void pathCorrectForwardslashW( WChar * const _filePath );
        void pathCorrectForwardslashA( Char * const _filePath );
        void pathCorrectForwardslashToW( WChar * const _out, const WChar * _filePath );
        void pathCorrectForwardslashToA( Char * const _out, const Char * _filePath );
        void pathRemoveSlashW( WChar * const _filePath, WChar _delim );
        void pathRemoveSlashA( Char * const _filePath, Char _delim );
        bool pathRemoveFileSpecW( WChar * const _in, WChar _delim );
        bool pathRemoveFileSpecA( Char * const _in, Char _delim );
    }
}