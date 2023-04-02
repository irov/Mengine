#pragma once

#include "Interface/PlatformSystemInterface.h"

#include "Environment/SDL/SDLIncluder.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SDLPlatformSystem
        : public ServiceBase<PlatformSystemInterface>
    {
    public:
        SDLPlatformSystem();
        ~SDLPlatformSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_malloc_func m_old_SDL_malloc_func;
        SDL_calloc_func m_old_SDL_calloc_func;
        SDL_realloc_func m_old_SDL_realloc_func;
        SDL_free_func m_old_SDL_free_func;
#endif
    };
}
