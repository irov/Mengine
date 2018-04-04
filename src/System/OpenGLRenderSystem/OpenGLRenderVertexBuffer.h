#pragma once

#include "Interface/RenderSystemInterface.h"

#include "OpenGLRenderExtension.h"

#include "Core/ServantBase.h"

namespace Mengine
{
    class OpenGLRenderVertexBuffer
        : public ServantBase<RenderVertexBufferInterface>
    {
    public:
        OpenGLRenderVertexBuffer();
        ~OpenGLRenderVertexBuffer() override;

    public:
        bool initialize( uint32_t _elementSize, EBufferType _bufferType );

    protected:
        uint32_t getVertexCount() const override;
        uint32_t getVertexSize() const override;

    protected:
        bool resize( uint32_t _vertexCount ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;

    protected:
        void draw( const void * _buffer, size_t _size ) override;
        
    public:
        bool enable() override;
        void disable() override;

    protected:
        uint32_t m_vertexCount;
        uint32_t m_vertexSize;

        GLenum m_usage;

        GLuint m_id;

        uint32_t m_lockOffset;
        uint32_t m_lockCount;
        MemoryInterfacePtr m_lockMemory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OpenGLRenderVertexBuffer> OpenGLRenderVertexBufferPtr;
}
