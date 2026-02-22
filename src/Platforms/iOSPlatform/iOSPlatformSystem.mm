#include "iOSPlatformSystem.h"

#include "Kernel/AllocatorHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformSystem, Mengine::iOSPlatformSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
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
    }
    //////////////////////////////////////////////////////////////////////////
    iOSPlatformSystem::iOSPlatformSystem()
        : m_old_SDL_malloc_func( nullptr )
        , m_old_SDL_calloc_func( nullptr )
        , m_old_SDL_realloc_func( nullptr )
        , m_old_SDL_free_func( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSPlatformSystem::~iOSPlatformSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformSystem::_initializeService()
    {
        SDL_GetMemoryFunctions( &m_old_SDL_malloc_func, &m_old_SDL_calloc_func, &m_old_SDL_realloc_func, &m_old_SDL_free_func );

        if( SDL_SetMemoryFunctions( &Detail::SDL_malloc_func, &Detail::SDL_calloc_func, &Detail::SDL_realloc_func, &Detail::SDL_free_func ) == false )
        {
            return false;
        }

        if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformSystem::_finalizeService()
    {
        SDL_Quit();

        SDL_SetMemoryFunctions( m_old_SDL_malloc_func, m_old_SDL_calloc_func, m_old_SDL_realloc_func, m_old_SDL_free_func );
        
        m_old_SDL_malloc_func = nullptr;
        m_old_SDL_calloc_func = nullptr;
        m_old_SDL_realloc_func = nullptr;
        m_old_SDL_free_func = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformSystem::beginThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformSystem::endThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
