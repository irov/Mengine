#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Unknowable.h"

#include "Config/Char.h"

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
        virtual void startThread() = 0;
        virtual void stopThread() = 0;

    public:
        virtual uint32_t getMemoryUsage() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ALLOCATOR_SYSTEM()\
    ((Mengine::AllocatorSystemInterface *)SERVICE_GET(Mengine::AllocatorSystemInterface))
//////////////////////////////////////////////////////////////////////////