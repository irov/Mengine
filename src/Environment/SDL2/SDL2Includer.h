#pragma once

#include "Config/Config.h"

#include "SDL.h"

#if defined(MENGINE_PLATFORM_WINDOWS) || defined(MENGINE_PLATFORM_LINUX)
#   include "SDL_syswm.h"
#endif