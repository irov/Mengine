#pragma once

#include "Config/Config.h"

#if defined(MENGINE_PLATFORM_IOS)
#   define SDL_MAIN_HANDLED
#endif

#include "SDL.h"
#include "SDL_syswm.h"
