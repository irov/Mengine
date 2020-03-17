#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class AllocatorServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AllocatorService" );

    public:
        virtual void * malloc( size_t _size, const Char * _doc ) = 0;
        virtual void free( void * _mem, const Char * _doc ) = 0;
        virtual void * calloc( size_t _num, size_t _size, const Char * _doc ) = 0;
        virtual void * realloc( void * _mem, size_t _size, const Char * _doc ) = 0;

    public:
        virtual uint32_t count( const Char * _doc ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ALLOCATOR_SERVICE()\
    ((Mengine::AllocatorServiceInterface *)SERVICE_GET(Mengine::AllocatorServiceInterface))
//////////////////////////////////////////////////////////////////////////