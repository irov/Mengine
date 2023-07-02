#include "SDLPlatformSystem.h"

#include "Kernel/AllocatorHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformSystem, Mengine::SDLPlatformSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_PLATFORM_UWP)
        //////////////////////////////////////////////////////////////////////////
        static void * SDL_malloc_func( size_t size )
        {
            void * p = Helper::allocateMemory( size, "SDL" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * SDL_calloc_func( size_t nmemb, size_t size )
        {
            void * p = Helper::callocateMemory( nmemb, size, "SDL" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * SDL_realloc_func( void * mem, size_t size )
        {
            void * p = Helper::reallocateMemory( mem, size, "SDL" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void SDL_free_func( void * mem )
        {
            Helper::deallocateMemory( mem, "SDL" );
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SDLPlatformSystem::SDLPlatformSystem()
#if !defined(MENGINE_PLATFORM_UWP)
        : m_old_SDL_malloc_func( nullptr )
        , m_old_SDL_calloc_func( nullptr )
        , m_old_SDL_realloc_func( nullptr )
        , m_old_SDL_free_func( nullptr )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLPlatformSystem::~SDLPlatformSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatformSystem::_initializeService()
    {
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_GetMemoryFunctions( &m_old_SDL_malloc_func, &m_old_SDL_calloc_func, &m_old_SDL_realloc_func, &m_old_SDL_free_func );

        if( SDL_SetMemoryFunctions( &Detail::SDL_malloc_func, &Detail::SDL_calloc_func, &Detail::SDL_realloc_func, &Detail::SDL_free_func ) != 0 )
        {
            return false;
        }
#endif

        if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformSystem::_finalizeService()
    {
        SDL_Quit();

#if !defined(MENGINE_PLATFORM_UWP)
        SDL_SetMemoryFunctions( m_old_SDL_malloc_func, m_old_SDL_calloc_func, m_old_SDL_realloc_func, m_old_SDL_free_func );
        m_old_SDL_malloc_func = nullptr;
        m_old_SDL_calloc_func = nullptr;
        m_old_SDL_realloc_func = nullptr;
        m_old_SDL_free_func = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
