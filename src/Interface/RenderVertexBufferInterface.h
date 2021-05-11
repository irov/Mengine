#pragma once

#include "Interface/MemoryInterface.h"
#include "Interface/RenderEnumInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderVertexBufferInterface
        : public Mixin
    {
    public:
        virtual bool initialize( uint32_t _vertexSize, EBufferType _bufferType ) = 0;
        virtual void finalize() = 0;

    public:
        virtual uint32_t getVertexCount() const = 0;
        virtual uint32_t getVertexSize() const = 0;

    public:
        virtual bool resize( uint32_t _vertexCount ) = 0;

    public:
        virtual MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size ) = 0;
        virtual bool unlock() = 0;

    public:
        virtual bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderVertexBufferInterface> RenderVertexBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
