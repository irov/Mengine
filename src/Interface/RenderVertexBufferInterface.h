#pragma once

#include "Interface/ServantInterface.h"

#include "Interface/MemoryInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderVertexBufferInterface
        : public ServantInterface
    {
    public:
        virtual uint32_t getVertexCount() const = 0;
        virtual uint32_t getVertexSize() const = 0;

    public:
        virtual bool enable() = 0;
        virtual void disable() = 0;

    public:
        virtual bool resize( uint32_t _count ) = 0;

    public:
        virtual MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size ) = 0;
        virtual bool unlock() = 0;

    public:
        virtual void draw( const void * _buffer, size_t _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderVertexBufferInterface> RenderVertexBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
