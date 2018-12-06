#pragma once

#include "Interface/DataInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"
#include "Kernel/DataPointer.h"

namespace Mengine
{
    namespace Helper
    {
        DataPointer dataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _dataflowType );
    }
}