#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/VectorFilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool collectTiledMapAutomappingRuleMaps( const FileGroupInterfacePtr & _fileGroup, const FilePath & _ruleListPath, VectorFilePath * const _parsedRuleLists, VectorFilePath * const _ruleMaps );
        //////////////////////////////////////////////////////////////////////////
    }
}
