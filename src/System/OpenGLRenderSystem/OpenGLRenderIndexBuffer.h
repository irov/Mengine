#   pragma once

#   include "Interface/RenderSystemInterface.h"

#   include "Core/ServantBase.h"

#   include "OpenGLRenderHeader.h"

namespace Menge
{
    class OpenGLRenderIndexBuffer
        : public ServantBase<RenderIndexBufferInterface>
    {
    public:
        OpenGLRenderIndexBuffer();
        ~OpenGLRenderIndexBuffer();

    public:
        bool initialize( uint32_t _indexNum, bool _dynamic );

    protected:
        Pointer lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
        bool unlock() override;

    public:
        void enable();

    protected:
        RenderIndices * m_memory;
        uint32_t m_indexNum;

        GLenum m_usage;

        GLuint m_id;

        uint32_t m_lockOffset;
        uint32_t m_lockCount;
        RenderIndices * m_lockMemory;
        EBufferLockFlag m_lockFlags;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OpenGLRenderIndexBuffer> OpenGLRenderIndexBufferPtr;
}
