#include "SDL3PlatformSystem.h"

#include "Kernel/AllocatorHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformSystem, Mengine::SDL3PlatformSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_PLATFORM_UWP)
        //////////////////////////////////////////////////////////////////////////
        static void * SDL3_malloc_func( size_t size )
        {
            void * p = Helper::allocateMemory( size, "SDL3" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * SDL3_calloc_func( size_t nmemb, size_t size )
        {
            void * p = Helper::callocateMemory( nmemb, size, "SDL3" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * SDL3_realloc_func( void * mem, size_t size )
        {
            void * p = Helper::reallocateMemory( mem, size, "SDL3" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void SDL3_free_func( void * mem )
        {
            Helper::deallocateMemory( mem, "SDL3" );
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3PlatformSystem::SDL3PlatformSystem()
#if !defined(MENGINE_PLATFORM_UWP)
        : m_old_SDL3_malloc_func( nullptr )
        , m_old_SDL3_calloc_func( nullptr )
        , m_old_SDL3_realloc_func( nullptr )
        , m_old_SDL3_free_func( nullptr )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3PlatformSystem::~SDL3PlatformSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformSystem::_initializeService()
    {
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_GetMemoryFunctions( &m_old_SDL3_malloc_func, &m_old_SDL3_calloc_func, &m_old_SDL3_realloc_func, &m_old_SDL3_free_func );

        if( SDL_SetMemoryFunctions( &Detail::SDL3_malloc_func, &Detail::SDL3_calloc_func, &Detail::SDL3_realloc_func, &Detail::SDL3_free_func ) == false )
        {
            return false;
        }
#endif

        if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformSystem::_finalizeService()
    {
        SDL_Quit();

#if !defined(MENGINE_PLATFORM_UWP)
        SDL_SetMemoryFunctions( m_old_SDL3_malloc_func, m_old_SDL3_calloc_func, m_old_SDL3_realloc_func, m_old_SDL3_free_func );
        m_old_SDL3_malloc_func = nullptr;
        m_old_SDL3_calloc_func = nullptr;
        m_old_SDL3_realloc_func = nullptr;
        m_old_SDL3_free_func = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformSystem::beginThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformSystem::endThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}



