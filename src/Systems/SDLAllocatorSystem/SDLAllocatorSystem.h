#pragma once

#include "Interface/AllocatorSystemInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class SDLAllocatorSystem
        : public ServiceBase<AllocatorSystemInterface>
    {
    public:
        SDLAllocatorSystem();
        ~SDLAllocatorSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void * malloc( size_t _size, const Char * _doc ) override;
        void free( void * _mem, const Char * _doc ) override;
        void * calloc( size_t _num, size_t _size, const Char * _doc ) override;
        void * realloc( void * _mem, size_t _size, const Char * _doc ) override;

    protected:
        void startThread() override;
        void stopThread() override;

    protected:
        uint32_t getMemoryUsage() const override;

    protected:
        void report( size_t _add, size_t _minus );

    protected:
        AtomicUInt32 m_memoryUsage;

    protected:
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_malloc_func m_old_SDL_malloc_func;
        SDL_calloc_func m_old_SDL_calloc_func;
        SDL_realloc_func m_old_SDL_realloc_func;
        SDL_free_func m_old_SDL_free_func;
#endif
    };
}
