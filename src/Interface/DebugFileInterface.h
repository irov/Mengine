#pragma once

#include "Config/Config.h"

#if !defined(MENGINE_DEBUG_FILE_PATH)
#   if defined(MENGINE_DEBUG)
#       define MENGINE_DEBUG_FILE_PATH 1
#   else
#       define MENGINE_DEBUG_FILE_PATH 0
#   endif
#endif

#if MENGINE_DEBUG_FILE_PATH == 1
#   define MENGINE_DEBUG_FILE_PATH_ENABLE
#endif

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)

#   include "Kernel/Mixin.h"
#   include "Kernel/FilePath.h"

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

#endif