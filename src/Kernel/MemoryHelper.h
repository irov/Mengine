#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryStream( const InputStreamInterfacePtr & _stream, const Char * _doc );
        MemoryInterfacePtr createMemoryStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const Char * _doc );
        MemoryInterfacePtr createMemoryStreamExtraSize( const InputStreamInterfacePtr & _stream, size_t _extraSize, const Char * _doc );
        MemoryInterfacePtr createMemoryFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, const Char * _doc );
        MemoryInterfacePtr createMemoryBuffer( size_t _size, const Char * _doc );
        MemoryInterfacePtr createMemoryCacheBuffer( size_t _size, const Char * _doc );
        MemoryInterfacePtr createMemoryCacheStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const Char * _doc );
        MemoryInterfacePtr createMemoryCacheStreamExtraSize( const InputStreamInterfacePtr & _stream, size_t _extraSize, const Char * _doc );
        MemoryInterfacePtr createMemoryCacheStream( const InputStreamInterfacePtr & _stream, const Char * _doc );
        MemoryInterfacePtr createMemoryCacheFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, const Char * _doc );
        MemoryInterfacePtr createMemoryCacheFileString( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, const Char * _doc );
    }
}


