#include "POSIXAllocatorSystem.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"

#include "Config/StdLib.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorSystem, Mengine::POSIXAllocatorSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXAllocatorSystem::POSIXAllocatorSystem()
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

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid alloc memory '%zu' [%s]"
            , _size
            , _doc
        );

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

        MENGINE_FREE( _mem );
    }
    //////////////////////////////////////////////////////////////////////////
    void * POSIXAllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        void * mem = MENGINE_CALLOC( _num, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid calloc memory '%zu' [%s]"
            , _num * _size
            , _doc
        );

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void * POSIXAllocatorSystem::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _mem == nullptr )
        {
            void * mem = MENGINE_MALLOC( _size );

            MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' from [nullptr] [%s]"
                , _size
                , _doc
            );

            return mem;
        }

        void * mem = MENGINE_REALLOC( _mem, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' from [%p] [%s]"
            , _size
            , _mem
            , _doc
        );

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
    //////////////////////////////////////////////////////////////////////////
}
