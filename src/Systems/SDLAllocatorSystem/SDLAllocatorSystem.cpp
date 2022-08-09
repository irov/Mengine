#include "SDLAllocatorSystem.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Error.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"

#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorSystem, Mengine::SDLAllocatorSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLAllocatorSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void * SDLAllocatorSystem::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        void * mem = ::malloc( _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid alloc memory '%zu' total '%u' [%s]"
            , _size
            , this->getMemoryUsage()
            , _doc
        );

#ifdef MENGINE_DEBUG
        size_t usage_size = _msize( mem );

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

#ifdef MENGINE_DEBUG
        size_t old_size = _msize( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1 );
#endif

        ::free( _mem );

#ifdef MENGINE_DEBUG
        this->report( 0, old_size );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * SDLAllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        void * mem = ::calloc( _num, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid calloc memory '%zu' total '%u' [%s]"
            , _num * _size
            , this->getMemoryUsage()
            , _doc
        );

#ifdef MENGINE_DEBUG
        size_t usage_size = _msize( mem );

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
            void * mem = ::malloc( _size );

            MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from [nullptr] [%s]"
                , _size
                , this->getMemoryUsage()
                , _doc
            );

#ifdef MENGINE_DEBUG
            size_t usage_size = _msize( mem );

            MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

            this->report( usage_size, 0 );
#endif

            return mem;
        }

#ifdef MENGINE_DEBUG
        size_t old_size = _msize( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1 );
#endif

        void * mem = ::realloc( _mem, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from [%p] [%s]"
            , _size
            , this->getMemoryUsage()
            , _mem
            , _doc
        );

#ifdef MENGINE_DEBUG
        size_t usage_size = _msize( mem );

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

        m_memoryUsage += _add;
        m_memoryUsage -= _minus;
    }
    //////////////////////////////////////////////////////////////////////////
}
