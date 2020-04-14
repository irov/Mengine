#include "OpenGLRenderVertexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "OpenGLRenderError.h"
#include "OpenGLRenderEnum.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexBuffer::OpenGLRenderVertexBuffer()
        : m_vertexCount( 0 )
        , m_vertexSize( 0 )
        , m_usage( GL_STATIC_DRAW )
        , m_id( 0 )
        , m_lockOffset( 0 )
        , m_lockCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexBuffer::~OpenGLRenderVertexBuffer()
    {
        if( m_id != 0 )
        {
            GLCALL( glDeleteBuffers, (1, &m_id) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexBuffer::initialize( uint32_t _elementSize, EBufferType _bufferType )
    {
        GLuint bufId = 0;
        GLCALL( glGenBuffers, (1, &bufId) );

        if( bufId == 0 )
        {
            return false;
        }

        m_id = bufId;

        m_vertexSize = _elementSize;
        m_usage = s_getGLBufferType( _bufferType );

#if defined(MENGINE_RENDER_OPENGL_ES)
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );
#else
        MemoryProxyInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxy( MENGINE_DOCUMENT_FACTORABLE );
#endif

        MENGINE_ASSERTION_MEMORY_PANIC( memory, false );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexBuffer::finalize()
    {
        if( m_id != 0 )
        {
            GLCALL( glDeleteBuffers, (1, &m_id) );
            m_id = 0;
        }

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderVertexBuffer::getVertexCount() const
    {
        return m_vertexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderVertexBuffer::getVertexSize() const
    {
        return m_vertexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexBuffer::resize( uint32_t _vertexCount )
    {
        m_vertexCount = _vertexCount;

        const uint32_t bufferSize = _vertexCount * m_vertexSize;

        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        GLCALL( glBufferData, (GL_ARRAY_BUFFER, bufferSize, nullptr, m_usage) );
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr OpenGLRenderVertexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( m_memory == nullptr )
        {
            return nullptr;
        }

        if( _offset + _count > m_vertexCount )
        {
            return nullptr;
        }

        m_lockOffset = _offset;
        m_lockCount = _count;

#if defined(MENGINE_RENDER_OPENGL_ES)
        const uint32_t bufferSize = m_lockCount * m_vertexSize;

        m_memory->newBuffer( bufferSize );
#else
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        void * buffer = nullptr;
        GLCALLR( buffer, glMapBuffer, (GL_ARRAY_BUFFER, GL_WRITE_ONLY) );
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

        const uint32_t bufferOffset = m_lockOffset * m_vertexSize;
        const uint32_t bufferSize = m_lockCount * m_vertexSize;

        m_memory->setBuffer( (uint8_t *)buffer + bufferOffset, bufferSize );
#endif

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexBuffer::unlock()
    {
#if defined(MENGINE_RENDER_OPENGL_ES)
        void * memory_buffer = m_memory->getBuffer();

        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        GLCALL( glBufferSubData, (GL_ARRAY_BUFFER, m_lockOffset * m_vertexSize, m_lockCount * m_vertexSize, memory_buffer) );
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
#else
        m_memory->setBuffer( nullptr, 0 );

        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        GLCALL( glUnmapBuffer, (GL_ARRAY_BUFFER) );
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
#endif

        m_lockOffset = 0;
        m_lockCount = 0;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexBuffer::draw( const void * _buffer, size_t _size )
    {
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        GLCALL( glBufferData, (GL_ARRAY_BUFFER, _size, nullptr, GL_STREAM_DRAW) );
        GLCALL( glBufferSubData, (GL_ARRAY_BUFFER, 0, _size, _buffer) );
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexBuffer::enable()
    {
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexBuffer::disable()
    {
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
    }
}