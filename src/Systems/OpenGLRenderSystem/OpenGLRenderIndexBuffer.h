#pragma once

#include "Interface/RenderSystemInterface.h"

#include "OpenGLRenderExtension.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class OpenGLRenderIndexBuffer
        : public RenderIndexBufferInterface
        , public Factorable
    {
    public:
        OpenGLRenderIndexBuffer();
        ~OpenGLRenderIndexBuffer() override;

    public:
        bool initialize( uint32_t _indexSize, EBufferType _bufferType ) override;
        void finalize() override;

    public:
        uint32_t getIndexCount() const override;
        uint32_t getIndexSize() const override;

    protected:
        void enable() override;
        void disable() override;

    protected:
        bool resize( uint32_t _count ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;
        bool draw( const void * _buffer, size_t _size ) override;

    protected:
        uint32_t m_indexSize;
        uint32_t m_indexCount;

        GLenum m_usage;

        GLuint m_id;

        uint32_t m_lockOffset;
        uint32_t m_lockCount;

        MemoryBufferInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderIndexBuffer> OpenGLRenderIndexBufferPtr;
}
