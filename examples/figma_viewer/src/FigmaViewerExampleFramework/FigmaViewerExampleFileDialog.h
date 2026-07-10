#pragma once

#include "Config/Char.h"

#include <cstddef>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool FigmaViewerExampleSelectFilePath( const Char * _title, const Char * _currentPath, Char * const _path, size_t _capacity );
    bool FigmaViewerExampleSelectFolderPath( const Char * _title, const Char * _currentPath, Char * const _path, size_t _capacity );
    //////////////////////////////////////////////////////////////////////////
}
