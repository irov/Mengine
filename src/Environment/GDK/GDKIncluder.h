#pragma once

#include "Config/Config.h"

#if !defined(MENGINE_PLATFORM_WINDOWS)
#error "include only for 'Windows' platform"
#endif

#include "Environment/Windows/WindowsIncluder.h"

#pragma warning(push, 0)
#include <XGameRuntimeInit.h>
#pragma warning(pop)