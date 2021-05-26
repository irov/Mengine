#pragma once

#include "Interface/RenderIndexBufferInterface.h"

#include "OpenGLRenderResourceHandler.h"
#include "OpenGLRenderExtension.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderIndexBuffer
        : public RenderIndexBufferInterface
        , public OpenGLRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( OpenGLRenderIndexBuffer );

    public:
        OpenGLRenderIndexBuffer();
        ~OpenGLRenderIndexBuffer() override;

    public:
        bool initialize( uint32_t _indexSize, EBufferType _bufferType ) override;
        void finalize() override;

    public:
        uint32_t getIndexCount() const override;
        uint32_t getIndexSize() const override;

    public:
        void enable();
        void disable();

    protected:
        bool resize( uint32_t _indexCount ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;

    public:
        void release();

    public:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        uint32_t m_indexCapacity;
        uint32_t m_indexCount;
        uint32_t m_indexSize;

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
    typedef IntrusivePtr<OpenGLRenderIndexBuffer, RenderIndexBufferInterface> OpenGLRenderIndexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}
