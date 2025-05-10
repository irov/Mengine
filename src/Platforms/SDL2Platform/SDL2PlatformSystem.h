#pragma once

#include "Interface/PlatformSystemInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SDL2PlatformSystem
        : public ServiceBase<PlatformSystemInterface>
    {
    public:
        SDL2PlatformSystem();
        ~SDL2PlatformSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void beginThread( ThreadId _threadId ) override;
        void endThread( ThreadId _threadId ) override;

    protected:
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_malloc_func m_old_SDL2_malloc_func;
        SDL_calloc_func m_old_SDL2_calloc_func;
        SDL_realloc_func m_old_SDL2_realloc_func;
        SDL_free_func m_old_SDL2_free_func;
#endif
    };
}
