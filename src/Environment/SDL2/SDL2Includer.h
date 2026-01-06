#pragma once

#include "Config/Config.h"

#if defined(MENGINE_PLATFORM_IOS)
#   define SDL_MAIN_HANDLED
#endif

#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_loadso.h"
#include "SDL_filesystem.h"
#include "SDL_video.h"
#include "SDL_thread.h"
#include "SDL_timer.h"