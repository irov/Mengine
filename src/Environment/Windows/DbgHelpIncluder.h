#pragma once

#include "Config/Config.h"

#if !defined(MENGINE_PLATFORM_WINDOWS)
#error "include only for 'Windows' platform"
#endif

#if defined(MENGINE_TOOLCHAIN_MINGW)
#   include <dbghelp.h>
#elif defined(MENGINE_TOOLCHAIN_MSVC)
#   include <DbgHelp.h>
#else
#   error "not supported platform"
#endif