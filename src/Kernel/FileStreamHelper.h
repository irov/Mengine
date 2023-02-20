#pragma once

#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        InputStreamInterfacePtr openInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _streaming, bool _share, const DocumentPtr & _doc );
        InputStreamInterfacePtr openInputStreamMutexFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, const DocumentPtr & _doc );
        bool closeInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const InputStreamInterfacePtr & _stream );
        //////////////////////////////////////////////////////////////////////////
        OutputStreamInterfacePtr openOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentPtr & _doc );
        bool closeOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const OutputStreamInterfacePtr & _stream );
        bool writeOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const MemoryInterfacePtr & _memory, const DocumentPtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        const FilePath & getInputStreamDebugRelationPath( const InputStreamInterfacePtr & _stream );
        const FilePath & getInputStreamDebugFolderPath( const InputStreamInterfacePtr & _stream );
        const FilePath & getInputStreamDebugFilePath( const InputStreamInterfacePtr & _stream );
        //////////////////////////////////////////////////////////////////////////
        const FilePath & getOutputStreamDebugRelationPath( const OutputStreamInterfacePtr & _stream );
        const FilePath & getOutputStreamDebugFolderPath( const OutputStreamInterfacePtr & _stream );
        const FilePath & getOutputStreamDebugFilePath( const OutputStreamInterfacePtr & _stream );
        //////////////////////////////////////////////////////////////////////////
    }
}