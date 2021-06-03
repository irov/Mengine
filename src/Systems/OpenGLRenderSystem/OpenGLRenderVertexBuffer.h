#pragma once

#include "Interface/RenderSystemInterface.h"

#include "OpenGLRenderExtension.h"
#include "OpenGLRenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderVertexBuffer
        : public RenderVertexBufferInterface
        , public OpenGLRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( OpenGLRenderVertexBuffer );

    public:
        OpenGLRenderVertexBuffer();
        ~OpenGLRenderVertexBuffer() override;

    public:
        bool initialize( uint32_t _elementSize, EBufferType _bufferType ) override;
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
        void enable();
        void disable();

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

        GLenum m_usage;

        GLuint m_id;

        uint32_t m_lockOffset;
        uint32_t m_lockCount;

#if defined(MENGINE_RENDER_OPENGL_ES)
        MemoryBufferInterfacePtr m_memory;
#else
        MemoryProxyInterfacePtr m_memory;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderVertexBuffer, RenderVertexBufferInterface> OpenGLRenderVertexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}
