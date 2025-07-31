#include "SDL2PlatformSystem.h"

#include "Kernel/AllocatorHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformSystem, Mengine::SDL2PlatformSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_PLATFORM_UWP)
        //////////////////////////////////////////////////////////////////////////
        static void * SDL2_malloc_func( size_t size )
        {
            void * p = Helper::allocateMemory( size, "SDL2" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * SDL2_calloc_func( size_t nmemb, size_t size )
        {
            void * p = Helper::callocateMemory( nmemb, size, "SDL2" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * SDL2_realloc_func( void * mem, size_t size )
        {
            void * p = Helper::reallocateMemory( mem, size, "SDL2" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void SDL2_free_func( void * mem )
        {
            Helper::deallocateMemory( mem, "SDL2" );
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2PlatformSystem::SDL2PlatformSystem()
#if !defined(MENGINE_PLATFORM_UWP)
        : m_old_SDL2_malloc_func( nullptr )
        , m_old_SDL2_calloc_func( nullptr )
        , m_old_SDL2_realloc_func( nullptr )
        , m_old_SDL2_free_func( nullptr )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2PlatformSystem::~SDL2PlatformSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformSystem::_initializeService()
    {
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_GetMemoryFunctions( &m_old_SDL2_malloc_func, &m_old_SDL2_calloc_func, &m_old_SDL2_realloc_func, &m_old_SDL2_free_func );

        if( SDL_SetMemoryFunctions( &Detail::SDL2_malloc_func, &Detail::SDL2_calloc_func, &Detail::SDL2_realloc_func, &Detail::SDL2_free_func ) != 0 )
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
    void SDL2PlatformSystem::_finalizeService()
    {
        SDL_Quit();

#if !defined(MENGINE_PLATFORM_UWP)
        SDL_SetMemoryFunctions( m_old_SDL2_malloc_func, m_old_SDL2_calloc_func, m_old_SDL2_realloc_func, m_old_SDL2_free_func );
        m_old_SDL2_malloc_func = nullptr;
        m_old_SDL2_calloc_func = nullptr;
        m_old_SDL2_realloc_func = nullptr;
        m_old_SDL2_free_func = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformSystem::beginThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformSystem::endThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
