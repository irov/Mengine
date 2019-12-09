#pragma once

#include "Interface/DataInterface.h"
#include "Interface/DataflowInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/DataPointer.h"

namespace Mengine
{
    namespace Helper
    {
        DataPointer getDataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const DocumentPtr & _doc );
        DataPointer popDataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const DocumentPtr & _doc );
    }
}