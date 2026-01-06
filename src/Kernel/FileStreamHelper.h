#pragma once

#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        InputStreamInterfacePtr openInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _streaming, bool _share, const DocumentInterfacePtr & _doc );
        InputStreamInterfacePtr openInputStreamMutexFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, const DocumentInterfacePtr & _doc );
        void closeInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const InputStreamInterfacePtr & _stream );
        //////////////////////////////////////////////////////////////////////////
        OutputStreamInterfacePtr openOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentInterfacePtr & _doc );
        bool closeOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const OutputStreamInterfacePtr & _stream );
        bool writeOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const MemoryInterfacePtr & _memory, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        bool createDirectory( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath );
        //////////////////////////////////////////////////////////////////////////
    }
}