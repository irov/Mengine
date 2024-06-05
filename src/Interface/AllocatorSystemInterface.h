#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Unknowable.h"

#include "Config/Char.h"
#include "Config/Thread.h"

namespace Mengine
{
    class AllocatorSystemInterface
        : public ServiceInterface
        , public Unknowable
    {
        SERVICE_DECLARE( "AllocatorSystem" );

    public:
        virtual void * malloc( size_t _size, const Char * _doc ) = 0;
        virtual void free( void * _mem, const Char * _doc ) = 0;
        virtual void * calloc( size_t _num, size_t _size, const Char * _doc ) = 0;
        virtual void * realloc( void * _mem, size_t _size, const Char * _doc ) = 0;

    public:
        virtual void beginThread( ThreadId _threadId ) = 0;
        virtual void endThread( ThreadId _threadId ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ALLOCATOR_SYSTEM()\
    ((Mengine::AllocatorSystemInterface *)SERVICE_GET(Mengine::AllocatorSystemInterface))
//////////////////////////////////////////////////////////////////////////