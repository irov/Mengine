#pragma once

#include "Interface/RenderVertexBufferInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderVertexBuffer
        : public RenderVertexBufferInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MockupRenderVertexBuffer );

    public:
        MockupRenderVertexBuffer();
        ~MockupRenderVertexBuffer() override;

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

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;

    protected:
        EBufferType m_bufferType;
        uint32_t m_vertexSize;
        uint32_t m_vertexCount;

        MemoryBufferInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderVertexBuffer> MockupRenderVertexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}