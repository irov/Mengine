#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderVertexBufferInterface
        : public Mixin
    {
    public:
        virtual uint32_t getVertexCount() const = 0;
        virtual uint32_t getVertexSize() const = 0;

    public:
        virtual void enable() = 0;
        virtual void disable() = 0;

    public:
        virtual bool resize( uint32_t _count ) = 0;

    public:
        virtual MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size, const DocumentPtr & _doc ) = 0;
        virtual bool unlock() = 0;

    public:
        virtual bool draw( const void * _buffer, size_t _size, const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderVertexBufferInterface> RenderVertexBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
