#pragma once

#include "Interface/RenderEnumInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderIndexBufferInterface
        : public Mixin
    {
    public:
        virtual bool initialize( uint32_t _indexSize, EBufferType _bufferType ) = 0;
        virtual void finalize() = 0;

    public:
        virtual uint32_t getIndexCount() const = 0;
        virtual uint32_t getIndexSize() const = 0;

    public:
        virtual bool resize( uint32_t _indexCount ) = 0;

    public:
        virtual MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) = 0;
        virtual bool unlock() = 0;

    public:
        virtual bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderIndexBufferInterface> RenderIndexBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
