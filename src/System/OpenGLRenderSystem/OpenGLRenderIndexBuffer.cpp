#include "OpenGLRenderIndexBuffer.h"

#include "OpenGLRenderError.h"
#include "OpenGLRenderEnum.h"

#include "Core/MemoryAllocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderIndexBuffer::OpenGLRenderIndexBuffer()
        : m_indexSize( 0 )
        , m_indexCount( 0 )
        , m_usage( GL_STATIC_DRAW )
        , m_id( 0 )
        , m_lockOffset( 0 )
        , m_lockCount( 0 )
        , m_lockMemory( nullptr )
        , m_lockFlags( BLF_LOCK_NONE )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderIndexBuffer::~OpenGLRenderIndexBuffer()
    {
        if( m_id != 0 )
        {
            GLCALL( glDeleteBuffers, (1, &m_id) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderIndexBuffer::initialize( uint32_t _indexSize, EBufferType _bufferType )
    {
        GLuint bufId = 0;
        GLCALL( glGenBuffers, (1, &bufId) );

        if( bufId == 0 )
        {
            return false;
        }

        m_id = bufId;

        m_usage = s_getGLBufferType( _bufferType );
        m_indexSize = _indexSize;

        return true;
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
    bool OpenGLRenderIndexBuffer::resize( uint32_t _count )
    {
        m_indexCount = _count;

        const uint32_t bufferSize = m_indexCount * m_indexSize;

        GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        GLCALL( glBufferData, (GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr, m_usage) );
        GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr OpenGLRenderIndexBuffer::lock( uint32_t _offset, uint32_t _count, EBufferLockFlag _flags )
    {
        if( m_lockMemory != nullptr )
        {
            return nullptr;
        }

        if( _offset + _count > m_indexCount )
        {
            return nullptr;
        }

        m_lockOffset = _offset;
        m_lockCount = _count;
        m_lockFlags = _flags;

        const uint32_t bufferSize = m_lockCount * m_indexSize;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryCacheBuffer();

        memory->newMemory( bufferSize, __FILE__, __LINE__ );

        m_lockMemory = memory;

        return m_lockMemory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderIndexBuffer::unlock()
    {
        void * memory_buffer = m_lockMemory->getMemory();

        GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        GLCALL( glBufferSubData, (GL_ELEMENT_ARRAY_BUFFER, m_lockOffset * m_indexSize, m_lockCount * m_indexSize, memory_buffer) );
        GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

        m_lockOffset = 0;
        m_lockCount = 0;
        m_lockMemory = nullptr;
        m_lockFlags = BLF_LOCK_NONE;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderIndexBuffer::draw( const void * _buffer, size_t _size )
    {
        GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        GLCALL( glBufferData, (GL_ELEMENT_ARRAY_BUFFER, _size, nullptr, GL_STREAM_DRAW) );
        GLCALL( glBufferSubData, (GL_ELEMENT_ARRAY_BUFFER, 0, _size, _buffer) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderIndexBuffer::enable()
    {
        IF_GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderIndexBuffer::disable()
    {
        GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );
    }
}