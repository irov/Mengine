#pragma once

#include "Interface/RenderIndexBufferInterface.h"

#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderIndexBuffer
        : public RenderIndexBufferInterface
        , public MetalRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderIndexBuffer );

    public:
        MetalRenderIndexBuffer();
        ~MetalRenderIndexBuffer() override;

    public:
        bool initialize( uint32_t _indexSize, EBufferType _bufferType ) override;
        void finalize() override;

    public:
        uint32_t getIndexCount() const override;
        uint32_t getIndexSize() const override;

    protected:
        bool resize( uint32_t _indexCount ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;

    public:
        id<MTLBuffer> getMetalBuffer() const;

    public:
        void release();

    public:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        uint32_t m_indexCapacity;
        uint32_t m_indexCount;
        uint32_t m_indexSize;

        EBufferType m_bufferType;

        id<MTLBuffer> m_buffer;

        uint32_t m_lockOffset;
        uint32_t m_lockCount;

        MemoryBufferInterfacePtr m_lockMemory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderIndexBuffer, RenderIndexBufferInterface> MetalRenderIndexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}
