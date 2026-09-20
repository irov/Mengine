#pragma once

#include "Config/Char.h"

#ifndef MENGINE_PATH_EMPTY
#define MENGINE_PATH_EMPTY(Path) ((Path)[0] == '\0')
#endif

#ifndef MENGINE_WPATH_EMPTY
#define MENGINE_WPATH_EMPTY(WPath) ((WPath)[0] == L'\0')
#endif

namespace Mengine
{
    typedef Char Path[MENGINE_MAX_PATH + 1];
    typedef WChar WPath[MENGINE_MAX_PATH + 1];
}
