#pragma once

#include "Config/Config.h"

#if defined(MENGINE_TOOLCHAIN_MSVC)
#   define MENGINE_DLL_EXPORT __declspec(dllexport)
#else
#   define MENGINE_DLL_EXPORT
#endif