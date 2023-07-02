#pragma once

#include "Interface/FileGroupInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ShapeQuadSize.h"
#include "Kernel/FilePath.h"

#include "math/vec2.h"

namespace Mengine
{
    namespace Helper
    {
        ShapeQuadSizePtr createSimpleSprite( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const mt::vec2f & _textureSize, const mt::vec2f & _size, const DocumentInterfacePtr & _doc );
    }
}