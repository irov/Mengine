#pragma once

#include "Interface/DataInterface.h"
#include "Interface/DataflowInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/DataInterfacePointer.h"

namespace Mengine
{
    namespace Helper
    {
        DataInterfacePointer getDataflow( const ContentInterfacePtr & _content, const DataflowContext * _context, const DocumentInterfacePtr & _doc );
        DataInterfacePointer popDataflow( const ContentInterfacePtr & _content, const DataflowInterfacePtr & _dataflow, const DataflowContext * _context, const DocumentInterfacePtr & _doc );
    }
}