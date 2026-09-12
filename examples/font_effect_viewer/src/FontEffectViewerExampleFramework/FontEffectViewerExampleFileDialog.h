#pragma once

#include "Config/Char.h"

#include <cstddef>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSelectFilePath( const Char * _title, const Char * _currentPath, Char * const _path, size_t _capacity );
    bool FontEffectViewerExampleSelectSaveFilePath( const Char * _title, const Char * _currentPath, const Char * _defaultName, Char * const _path, size_t _capacity );
    //////////////////////////////////////////////////////////////////////////
}
