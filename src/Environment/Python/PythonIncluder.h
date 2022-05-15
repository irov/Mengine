#pragma once

#include "Config/Config.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   define PYBIND_STL_SUPPORT
#   define PYBIND_CALL_DEBUG

#   include "pybind/pybind.hpp"
#endif