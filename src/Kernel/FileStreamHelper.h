#pragma once

#include "Interface/FileGroupInterface.h"

namespace Mengine
{
    namespace Helper
    {
        InputStreamInterfacePtr openInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _streaming, bool _share, const DocumentPtr & _doc );
        OutputStreamInterfacePtr openOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc );

#ifdef MENGINE_DEBUG
        const FilePath & getInputStreamRelationPath( const InputStreamInterfacePtr & _stream );
        const FilePath & getInputStreamFolderPath( const InputStreamInterfacePtr & _stream );
        const FilePath & getInputStreamFilePath( const InputStreamInterfacePtr & _stream );
#endif
    }
}