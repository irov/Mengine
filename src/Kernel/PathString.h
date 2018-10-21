#pragma once

#include "stdex/array_string.h"

#ifndef MENGINE_MAX_PATH
#   define MENGINE_MAX_PATH 260
#endif

namespace Mengine
{
    typedef stdex::array_string<MENGINE_MAX_PATH> PathString;
}