#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DebugFileInterface
        : public Mixin
    {
    public:
        virtual const FilePath & getDebugRelationPath() const = 0;
        virtual const FilePath & getDebugFolderPath() const = 0;
        virtual const FilePath & getDebugFilePath() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DebugFileInterface> DebugFileInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}


