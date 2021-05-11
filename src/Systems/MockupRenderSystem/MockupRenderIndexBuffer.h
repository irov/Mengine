#pragma once

#include "Interface/RenderIndexBufferInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderIndexBuffer
        : public RenderIndexBufferInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MockupRenderIndexBuffer );

    public:
        MockupRenderIndexBuffer();
        ~MockupRenderIndexBuffer() override;

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

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;

    protected:
        EBufferType m_bufferType;
        uint32_t m_indexSize;
        uint32_t m_indexCount;

        MemoryBufferInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderIndexBuffer> MockupRenderIndexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}