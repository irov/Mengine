#include "SDLAllocatorSystem.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/Error.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdLib.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorSystem, Mengine::SDLAllocatorSystem );
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
    SDLAllocatorSystem::SDLAllocatorSystem()
        : m_memoryUsage( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLAllocatorSystem::~SDLAllocatorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLAllocatorSystem::_initializeService()
    {
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_GetMemoryFunctions( &m_old_SDL_malloc_func, &m_old_SDL_calloc_func, &m_old_SDL_realloc_func, &m_old_SDL_free_func );

        if( SDL_SetMemoryFunctions( &Detail::SDL_malloc_func, &Detail::SDL_calloc_func, &Detail::SDL_realloc_func, &Detail::SDL_free_func ) != 0 )
        {
            LOGGER_ERROR( "invalid set memory functions: %s"
                , SDL_GetError()
            );

            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLAllocatorSystem::_finalizeService()
    {
#if !defined(MENGINE_PLATFORM_UWP)
        SDL_SetMemoryFunctions( m_old_SDL_malloc_func, m_old_SDL_calloc_func, m_old_SDL_realloc_func, m_old_SDL_free_func );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * SDLAllocatorSystem::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        void * mem = MENGINE_MALLOC( _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid alloc memory '%zu' total '%u' [%s]"
            , _size
            , this->getMemoryUsage()
            , _doc
        );

#if defined(MENGINE_DEBUG)
        size_t usage_size = MENGINE_MALLOC_SIZE( mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

        this->report( usage_size, 0 );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLAllocatorSystem::free( void * _mem, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _mem == nullptr )
        {
            return;
        }

#if defined(MENGINE_DEBUG)
        size_t old_size = MENGINE_MALLOC_SIZE( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1 );
#endif

        MENGINE_FREE( _mem );

#if defined(MENGINE_DEBUG)
        this->report( 0, old_size );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * SDLAllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        void * mem = MENGINE_CALLOC( _num, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid calloc memory '%zu' total '%u' [%s]"
            , _num * _size
            , this->getMemoryUsage()
            , _doc
        );

#if defined(MENGINE_DEBUG)
        size_t usage_size = MENGINE_MALLOC_SIZE( mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

        this->report( usage_size, 0 );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void * SDLAllocatorSystem::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _mem == nullptr )
        {
            void * mem = MENGINE_MALLOC( _size );

            MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from [nullptr] [%s]"
                , _size
                , this->getMemoryUsage()
                , _doc
            );

#if defined(MENGINE_DEBUG)
            size_t usage_size = MENGINE_MALLOC_SIZE( mem );

            MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

            this->report( usage_size, 0 );
#endif

            return mem;
        }

#if defined(MENGINE_DEBUG)
        size_t old_size = MENGINE_MALLOC_SIZE( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1 );
#endif

        void * mem = MENGINE_REALLOC( _mem, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from [%p] [%s]"
            , _size
            , this->getMemoryUsage()
            , _mem
            , _doc
        );

#if defined(MENGINE_DEBUG)
        size_t usage_size = MENGINE_MALLOC_SIZE( mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

        this->report( usage_size, old_size );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLAllocatorSystem::startThread()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLAllocatorSystem::stopThread()
    {
        //Empty
    }    
    ////////////////////////////////////////////////////////////////////////
    uint32_t SDLAllocatorSystem::getMemoryUsage() const
    {
        uint32_t memoryUsage = m_memoryUsage;

        return memoryUsage;
    }
    ////////////////////////////////////////////////////////////////////////
    void SDLAllocatorSystem::report( size_t _add, size_t _minus )
    {
        MENGINE_ASSERTION_FATAL( m_memoryUsage + _add >= _minus );

        m_memoryUsage += (uint32_t)_add;
        m_memoryUsage -= (uint32_t)_minus;
    }
    //////////////////////////////////////////////////////////////////////////
}
