#include "POSIXAllocatorSystem.h"

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
SERVICE_FACTORY( AllocatorSystem, Mengine::POSIXAllocatorSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXAllocatorSystem::POSIXAllocatorSystem()
        : m_memoryUsage( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXAllocatorSystem::~POSIXAllocatorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXAllocatorSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXAllocatorSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void * POSIXAllocatorSystem::malloc( size_t _size, const Char * _doc )
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

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1, "usage size failed" );

        this->report( usage_size, 0 );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXAllocatorSystem::free( void * _mem, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _mem == nullptr )
        {
            return;
        }

#if defined(MENGINE_DEBUG)
        size_t old_size = MENGINE_MALLOC_SIZE( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1, "old size failed" );
#endif

        MENGINE_FREE( _mem );

#if defined(MENGINE_DEBUG)
        this->report( 0, old_size );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * POSIXAllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
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

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1, "usage size failed" );

        this->report( usage_size, 0 );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void * POSIXAllocatorSystem::realloc( void * _mem, size_t _size, const Char * _doc )
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

            MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1, "usage size failed" );

            this->report( usage_size, 0 );
#endif

            return mem;
        }

#if defined(MENGINE_DEBUG)
        size_t old_size = MENGINE_MALLOC_SIZE( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1, "old size failed" );
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

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1, "usage size failed" );

        this->report( usage_size, old_size );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXAllocatorSystem::beginThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXAllocatorSystem::endThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }    
    ////////////////////////////////////////////////////////////////////////
    uint32_t POSIXAllocatorSystem::getMemoryUsage() const
    {
        uint32_t memoryUsage = m_memoryUsage;

        return memoryUsage;
    }
    ////////////////////////////////////////////////////////////////////////
    void POSIXAllocatorSystem::report( size_t _add, size_t _minus )
    {
        MENGINE_ASSERTION_FATAL( m_memoryUsage + _add >= _minus, "memory usage %u and add %zu less minus %zu"
            , Helper::atomicLoad( m_memoryUsage )
            , _add
            , _minus
        );

        m_memoryUsage += (uint32_t)_add;
        m_memoryUsage -= (uint32_t)_minus;
    }
    //////////////////////////////////////////////////////////////////////////
}
