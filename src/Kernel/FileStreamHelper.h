#pragma once

#include "Interface/FileGroupInterface.h"

namespace Mengine
{
    namespace Helper
    {
        InputStreamInterfacePtr openInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _streaming, bool _share, const DocumentPtr & _doc );
        OutputStreamInterfacePtr openOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc );

        const FilePath & getInputStreamDebugRelationPath( const InputStreamInterfacePtr & _stream );
        const FilePath & getInputStreamDebugFolderPath( const InputStreamInterfacePtr & _stream );
        const FilePath & getInputStreamDebugFilePath( const InputStreamInterfacePtr & _stream );
    }
}