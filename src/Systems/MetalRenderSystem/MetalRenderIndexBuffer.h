#pragma once

#include "MetalRenderTypes.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Kernel/Factorable.h"
#include "Interface/MemoryInterface.h"

namespace Mengine
{
    class MetalRenderIndexBuffer
        : public RenderIndexBufferInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderIndexBuffer );

    public:
        MetalRenderIndexBuffer( MetalDeviceId _device );
        ~MetalRenderIndexBuffer() override;

    public:
        bool initialize( uint32_t _indexSize, EBufferType _bufferType ) override;
        void finalize() override;

    protected:
        uint32_t getIndexCount() const override;
        uint32_t getIndexSize() const override;

    protected:
        bool resize( uint32_t _count ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;

    public:
        MetalBufferId getBuffer() const;
        MTLIndexType getIndexType() const;

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;

    protected:
        MetalDeviceId m_device;
        MetalBufferId m_buffer;
        EBufferType m_bufferType;
        uint32_t m_indexSize;
        MTLIndexType m_indexType;
        uint32_t m_indexCount;
        uint32_t m_lockOffset;
        MemoryBufferInterfacePtr m_memory;
    };

    typedef IntrusivePtr<MetalRenderIndexBuffer, RenderIndexBufferInterface> MetalRenderIndexBufferPtr;
}

