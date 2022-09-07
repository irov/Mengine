#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    namespace Helper
    {
        MemoryInterfacePtr createMemoryStream( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryStreamExtraSize( const InputStreamInterfacePtr & _stream, size_t _extraSize, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryStreamString( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, bool _share, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryContent( const ContentInterfacePtr & _content, bool _stream, bool _share, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryBuffer( size_t _size, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryCacheBuffer( size_t _size, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryCacheStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryCacheStreamExtraSize( const InputStreamInterfacePtr & _stream, size_t _extraSize, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryCacheStream( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryCacheFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, bool _share, const DocumentPtr & _doc );
        MemoryInterfacePtr createMemoryCacheFileString( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, bool _share, const DocumentPtr & _doc );
    }
}