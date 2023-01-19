#pragma once

#include "Config/Config.h"

#define PYBIND_STL_SUPPORT

#ifdef MENGINE_DEBUG
#   define PYBIND_CALL_DEBUG
#endif

#include "pybind/pybind.hpp"
