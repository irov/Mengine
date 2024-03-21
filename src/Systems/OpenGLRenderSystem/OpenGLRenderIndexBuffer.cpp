#include "OpenGLRenderIndexBuffer.h"

#include "Interface/MemoryServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Environment/OpenGL/OpenGLRenderSystemExtensionInterface.h"

#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderEnum.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderIndexBuffer::OpenGLRenderIndexBuffer()
        : m_indexCapacity( 0 )
        , m_indexCount( 0 )
        , m_indexSize( 0 )
        , m_usage( GL_STATIC_DRAW )
        , m_id( 0 )
        , m_lockOffset( 0 )
        , m_lockCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderIndexBuffer::~OpenGLRenderIndexBuffer()
    {
        MENGINE_ASSERTION_FATAL( m_id == 0, "index buffer '%u' is not released"
            , m_id
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderIndexBuffer::initialize( uint32_t _indexSize, EBufferType _bufferType )
    {
        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        GLuint id = extension->genBuffer();

        if( id == 0 )
        {
            return false;
        }

        m_id = id;

        m_usage = Helper::toGLBufferType( _bufferType );
        m_indexSize = _indexSize;

#if defined(MENGINE_RENDER_OPENGL_ES)
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );
#else
        MemoryProxyInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxy( MENGINE_DOCUMENT_FACTORABLE );
#endif

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderIndexBuffer::finalize()
    {
        m_memory = nullptr;

        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderIndexBuffer::getIndexCount() const
    {
        return m_indexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderIndexBuffer::getIndexSize() const
    {
        return m_indexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderIndexBuffer::resize( uint32_t _indexCount )
    {
        m_indexCount = _indexCount;

        if( m_indexCapacity >= _indexCount )
        {
            return true;
        }

        m_indexCapacity = _indexCount;

        const uint32_t bufferSize = m_indexCapacity * m_indexSize;

        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        MENGINE_GLCALL( glBufferData, (GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr, m_usage) );
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr OpenGLRenderIndexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( m_memory == nullptr )
        {
            return nullptr;
        }

        if( _offset + _count > m_indexCount )
        {
            return nullptr;
        }

        m_lockOffset = _offset;
        m_lockCount = _count;

#if defined(MENGINE_RENDER_OPENGL_ES)
        const uint32_t bufferSize = m_lockCount * m_indexSize;

        m_memory->newBuffer( bufferSize );
#else
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        void * buffer = nullptr;
        MENGINE_GLCALLR( buffer, glMapBuffer, (GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY) );
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid map buffer" );

        const uint32_t bufferOffset = m_lockOffset * m_indexSize;
        const uint32_t bufferSize = m_lockCount * m_indexSize;

        m_memory->setBuffer( (uint8_t *)buffer + bufferOffset, bufferSize );
#endif

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderIndexBuffer::unlock()
    {
#if defined(MENGINE_RENDER_OPENGL_ES)
        void * memory_buffer = m_memory->getBuffer();

        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        MENGINE_GLCALL( glBufferSubData, (GL_ELEMENT_ARRAY_BUFFER, m_lockOffset * m_indexSize, m_lockCount * m_indexSize, memory_buffer) );
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );
#else
        m_memory->setBuffer( nullptr, 0 );

        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        MENGINE_GLCALL( glUnmapBuffer, (GL_ELEMENT_ARRAY_BUFFER) );
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );
#endif

        m_lockOffset = 0;
        m_lockCount = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderIndexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCount )
        {
            return false;
        }

        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );

        const uint32_t bufferOffset = _offset * m_indexSize;
        const uint32_t bufferSize = _count * m_indexSize;

        MENGINE_GLCALL( glBufferSubData, (GL_ELEMENT_ARRAY_BUFFER, bufferOffset, bufferSize, _buffer) );
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

        return  true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderIndexBuffer::enable()
    {
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderIndexBuffer::disable()
    {
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderIndexBuffer::release()
    {
        if( m_id != 0 )
        {
            OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
                ->getUnknown();

            extension->deleteBuffer( m_id );

            m_id = 0;
        }

        m_indexCapacity = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderIndexBuffer::onRenderReset()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderIndexBuffer::onRenderRestore()
    {
        MENGINE_ASSERTION_FATAL( m_id == 0, "index buffer '%u' is not release"
            , m_id
        );

        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        GLuint id = extension->genBuffer();

        if( id == 0 )
        {
            return false;
        }

        m_id = id;

        const uint32_t bufferSize = m_indexCapacity * m_indexSize;

        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        MENGINE_GLCALL( glBufferData, (GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr, m_usage) );
        MENGINE_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}