#pragma once

#include "Config/Config.h"

// SDL_MAIN_HANDLED prevents SDL_main.h from redefining main functions
// This is needed for all modules except the main application
#define SDL_MAIN_HANDLED

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_loadso.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_video.h"
#include "SDL3/SDL_properties.h"
#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_timer.h"

