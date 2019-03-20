#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderIndexBufferInterface
        : public Mixin
    {
    public:
        virtual uint32_t getIndexCount() const = 0;
        virtual uint32_t getIndexSize() const = 0;

    public:
        virtual void enable() = 0;
        virtual void disable() = 0;

    public:
        virtual bool resize( uint32_t _count ) = 0;

    public:
        virtual MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size, const Char * _doc ) = 0;
        virtual bool unlock() = 0;

    public:
        virtual void draw( const void * _buffer, size_t _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderIndexBufferInterface> RenderIndexBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
