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
        virtual void startThread() = 0;
        virtual void stopThread() = 0;

    public:
        virtual bool checkOverrideCorruption( void * _pointer ) const = 0;

    public:
        virtual uint32_t get_report_count() const = 0;
        virtual size_t get_report_info( uint32_t _index, const char ** _name ) const = 0;
        virtual size_t get_report_total() const = 0;

    public:
        virtual size_t find_alloc_count( const Char * _doc ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ALLOCATOR_SERVICE()\
    ((Mengine::AllocatorServiceInterface *)SERVICE_GET(Mengine::AllocatorServiceInterface))
//////////////////////////////////////////////////////////////////////////