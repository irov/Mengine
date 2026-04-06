#pragma once

#include "Interface/RenderSystemInterface.h"

#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderVertexBuffer
        : public RenderVertexBufferInterface
        , public MetalRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderVertexBuffer );

    public:
        MetalRenderVertexBuffer();
        ~MetalRenderVertexBuffer() override;

    public:
        bool initialize( uint32_t _elementSize, EBufferType _bufferType );
        void finalize() override;

    protected:
        uint32_t getVertexCount() const override;
        uint32_t getVertexSize() const override;

    protected:
        bool resize( uint32_t _vertexCount ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _size ) override;

    public:
        id<MTLBuffer> getMetalBuffer() const;

    public:
        void release();
        bool reload();

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        uint32_t m_vertexCapacity;
        uint32_t m_vertexCount;
        uint32_t m_vertexSize;

        EBufferType m_bufferType;

        id<MTLBuffer> m_buffer;

        uint32_t m_lockOffset;
        uint32_t m_lockCount;

        MemoryBufferInterfacePtr m_lockMemory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderVertexBuffer, RenderVertexBufferInterface> MetalRenderVertexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}
