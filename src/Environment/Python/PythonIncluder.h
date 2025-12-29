#pragma once

#include "Config/Config.h"

#define PYBIND_STL_SUPPORT

#if defined(MENGINE_DEBUG)
#   define PYBIND_CALL_DEBUG
#endif

#include "pybind/pybind.hpp"