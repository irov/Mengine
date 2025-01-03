#pragma once

#include "Kernel/Mixin.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class InputStreamInterface
        : public Mixin
    {
    public:
        virtual size_t read( void * const _buffer, size_t _size ) = 0;
        virtual bool seek( size_t _pos ) = 0;
        virtual void rewind() = 0;
        virtual bool rseek( size_t _pos ) = 0;
        virtual bool skip( size_t _size ) = 0;
        virtual size_t tell() const = 0;
        virtual size_t size() const = 0;
        virtual bool eof() const = 0;

    public:
        virtual bool time( uint64_t * const _time ) const = 0;

    public:
        virtual bool memory( void ** const _memory, size_t * const _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<InputStreamInterface> InputStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
