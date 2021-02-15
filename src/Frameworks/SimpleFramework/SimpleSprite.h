#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/FilePath.h"
#include "Kernel/Document.h"

#include "math/vec2.h"

namespace Mengine
{
    namespace Helper
    {
        ShapeQuadFixedPtr createSimpleSprite( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const mt::vec2f & _maxSize, const DocumentPtr & _doc );
    }
}