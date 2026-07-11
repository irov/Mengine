#pragma once

#include "Config/Config.h"

#if !defined(MENGINE_PLATFORM_LINUX)
#   error "Unix environment is supported on Linux only"
#endif

#include <X11/Xlib.h>
