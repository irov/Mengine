#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/JSON.h"

namespace Mengine
{
    namespace Helper
    {
        jpp::object loadJSON( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc );
        jpp::object loadJSONStream( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc );
        jpp::object loadJSONStreamFromMemory( const MemoryInterfacePtr & _memory, const DocumentPtr & _doc );
        jpp::object loadJSONStreamFromBuffer( const void * _buffer, size_t _size, const DocumentPtr & _doc );
        bool saveJSON( const OutputStreamInterfacePtr & _stream, const jpp::object & _j );
    }
}