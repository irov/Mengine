#pragma once

#include "MetalRenderTypes.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Kernel/Factorable.h"
#include "Interface/MemoryInterface.h"

namespace Mengine
{
    class MetalRenderVertexBuffer
        : public RenderVertexBufferInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderVertexBuffer );

    public:
        MetalRenderVertexBuffer( MetalDeviceId _device );
        ~MetalRenderVertexBuffer() override;

    public:
        bool initialize( uint32_t _vertexSize, EBufferType _bufferType ) override;
        void finalize() override;

    protected:
        uint32_t getVertexCount() const override;
        uint32_t getVertexSize() const override;

    protected:
        bool resize( uint32_t _count ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;

    public:
        MetalBufferId getBuffer() const;

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;

    protected:
        MetalDeviceId m_device;
        MetalBufferId m_buffer;
        EBufferType m_bufferType;
        uint32_t m_vertexSize;
        uint32_t m_vertexCount;
        uint32_t m_lockOffset;
        MemoryBufferInterfacePtr m_memory;
    };

    typedef IntrusivePtr<MetalRenderVertexBuffer, RenderVertexBufferInterface> MetalRenderVertexBufferPtr;
}

