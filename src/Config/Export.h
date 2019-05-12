#pragma once

#include "Config/Config.h"

#if defined(MENGINE_TOOLCHAIN_MSVC)
#   define DLL_EXPORT __declspec(dllexport)
#else
#   define DLL_EXPORT
#endif