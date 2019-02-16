#pragma once

#include "Config/Typedef.h"

#include "Interface/Interface.h"

namespace Mengine
{
    class InputStreamInterface
        : public Interface
    {
    public:
        virtual size_t read( void * _buffer, size_t _size ) = 0;
        virtual bool seek( size_t _pos ) = 0;
        virtual bool skip( size_t _pos ) = 0;
        virtual size_t tell() const = 0;
        virtual size_t size() const = 0;
        virtual bool eof() const = 0;

    public:
        virtual bool time( uint64_t & _time ) const = 0;

    public:
        virtual bool memory( void ** _memory, size_t * _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<InputStreamInterface> InputStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
