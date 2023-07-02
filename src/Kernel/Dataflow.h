#pragma once

#include "Interface/DataInterface.h"
#include "Interface/DataflowInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/DataInterfacePointer.h"

namespace Mengine
{
    namespace Helper
    {
        DataInterfacePointer getDataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const DataflowContext * _context, const DocumentInterfacePtr & _doc );
        DataInterfacePointer popDataflow( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const DataflowContext * _context, const DocumentInterfacePtr & _doc );
    }
}