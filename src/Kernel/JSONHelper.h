#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/JSON.h"
#include "Kernel/String.h"

namespace Mengine
{
    namespace Helper
    {
        jpp::object loadJSON( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc );
        jpp::object loadJSONStream( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc );
        jpp::object loadJSONStreamFromMemory( const MemoryInterfacePtr & _memory, const DocumentInterfacePtr & _doc );
        jpp::object loadJSONStreamFromString( const String & _value, const DocumentInterfacePtr & _doc );
        jpp::object loadJSONStreamFromBuffer( const void * _buffer, size_t _size, const DocumentInterfacePtr & _doc );

        bool writeJSONFile( const jpp::object & _j, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentInterfacePtr & _doc );
        bool writeJSONFileCompact( const jpp::object & _j, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentInterfacePtr & _doc );
        bool writeJSONStream( const jpp::object & _j, const OutputStreamInterfacePtr & _stream );
        bool writeJSONStreamCompact( const jpp::object & _j, const OutputStreamInterfacePtr & _stream );
        bool writeJSONString( const jpp::object & _j, String * const _string );
        bool writeJSONStringCompact( const jpp::object & _j, String * const _string );
    }
}
