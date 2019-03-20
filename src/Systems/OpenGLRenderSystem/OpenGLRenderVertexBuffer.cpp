#include "OpenGLRenderVertexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "OpenGLRenderError.h"
#include "OpenGLRenderEnum.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexBuffer::OpenGLRenderVertexBuffer()
        : m_vertexCount( 0 )
        , m_vertexSize( 0 )
        , m_usage( GL_STATIC_DRAW )
        , m_id( 0 )
        , m_lockOffset(0)
        , m_lockCount(0)
        , m_lockMemory(nullptr)
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

        return true;
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
    MemoryInterfacePtr OpenGLRenderVertexBuffer::lock( uint32_t _offset, uint32_t _count, const Char * _doc )
    {
        if( m_lockMemory != nullptr )
        {
            return nullptr;
        }

        if( _offset + _count > m_vertexCount )
        {
            return nullptr;
        }
                
        m_lockOffset = _offset;
        m_lockCount = _count;

        const uint32_t bufferSize = m_lockCount * m_vertexSize;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryCacheBuffer( _doc );

        memory->newBuffer( bufferSize, "OpenGLRenderVertexBuffer", __FILE__, __LINE__ );

        m_lockMemory = memory;
        
        return m_lockMemory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexBuffer::unlock()
    {
        void * memory_buffer = m_lockMemory->getBuffer();

        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        GLCALL( glBufferSubData, (GL_ARRAY_BUFFER, m_lockOffset * m_vertexSize, m_lockCount * m_vertexSize, memory_buffer) );
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

        m_lockOffset = 0;
        m_lockCount = 0;
        m_lockMemory = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexBuffer::draw( const void * _buffer, size_t _size )
    {
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        GLCALL( glBufferData, (GL_ARRAY_BUFFER, _size, nullptr, GL_STREAM_DRAW) );
        GLCALL( glBufferSubData, (GL_ARRAY_BUFFER, 0, _size, _buffer) );
        GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
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