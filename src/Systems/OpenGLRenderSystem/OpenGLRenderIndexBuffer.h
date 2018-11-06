#pragma once

#include "Interface/RenderSystemInterface.h"

#include "OpenGLRenderExtension.h"

#include "Kernel/ServantBase.h"

namespace Mengine
{
    class OpenGLRenderIndexBuffer
        : public ServantBase<RenderIndexBufferInterface>
    {
    public:
        OpenGLRenderIndexBuffer();
        ~OpenGLRenderIndexBuffer() override;

    public:
        bool initialize( uint32_t _indexSize, EBufferType _bufferType );

    public:
        uint32_t getIndexCount() const override;
        uint32_t getIndexSize() const override;

    protected:
        bool enable() override;
        void disable() override;
        
    protected:
        bool resize( uint32_t _count ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;
        void draw( const void * _buffer, size_t _size ) override;

    protected:
        void * m_memory;
        uint32_t m_indexSize;
        uint32_t m_indexCount;

        GLenum m_usage;

        GLuint m_id;

        uint32_t m_lockOffset;
        uint32_t m_lockCount;
        MemoryInterfacePtr m_lockMemory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OpenGLRenderIndexBuffer> OpenGLRenderIndexBufferPtr;
}
