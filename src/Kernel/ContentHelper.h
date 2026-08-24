#pragma once

#include "Interface/ContentInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/DataflowInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/PathString.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    namespace Helper
    {
        ContentInterfacePtr makeFileContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc );
        const ConstString & findContentCodecType( const ContentInterfacePtr & _content );
        PathString getContentFullPath( const ContentInterfacePtr & _content );
    }
}
