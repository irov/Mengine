#pragma once

#include "Config/Config.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   define PYBIND_STL_SUPPORT

#   ifdef MENGINE_DEBUG
#       define PYBIND_CALL_DEBUG
#   endif

#   include "pybind/pybind.hpp"
#endif