#pragma once

#include "Interface/PlatformSystemInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SDL3PlatformSystem
        : public ServiceBase<PlatformSystemInterface>
    {
    public:
        SDL3PlatformSystem();
        ~SDL3PlatformSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool beginThread( ThreadId _threadId ) override;
        void endThread( ThreadId _threadId ) override;

    protected:
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_malloc_func m_old_SDL3_malloc_func;
        SDL_calloc_func m_old_SDL3_calloc_func;
        SDL_realloc_func m_old_SDL3_realloc_func;
        SDL_free_func m_old_SDL3_free_func;
#endif
    };
}