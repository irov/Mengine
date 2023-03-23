#include "OpenGLRenderVertexBuffer.h"

#include "Interface/MemoryServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/OpenGLRenderSystemExtensionInterface.h"

#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderEnum.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexBuffer::OpenGLRenderVertexBuffer()
        : m_vertexCapacity( 0 )
        , m_vertexCount( 0 )
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
        MENGINE_ASSERTION_FATAL( m_id == 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexBuffer::initialize( uint32_t _elementSize, EBufferType _bufferType )
    {
        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        GLuint id = extension->genBuffer();

        if( id == 0 )
        {
            return false;
        }

        m_id = id;

        m_vertexSize = _elementSize;
        m_usage = Helper::toGLBufferType( _bufferType );

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
    void OpenGLRenderVertexBuffer::finalize()
    {
        m_memory = nullptr;

        this->release();
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

        if( m_vertexCapacity >= _vertexCount )
        {
            return true;
        }

        m_vertexCapacity = _vertexCount;

        const uint32_t bufferSize = m_vertexCapacity * m_vertexSize;

        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        MENGINE_GLCALL( glBufferData, (GL_ARRAY_BUFFER, bufferSize, nullptr, m_usage) );
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

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
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        void * buffer = nullptr;
        MENGINE_GLCALLR( buffer, glMapBuffer, (GL_ARRAY_BUFFER, GL_WRITE_ONLY) );
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid map buffer" );

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

        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        MENGINE_GLCALL( glBufferSubData, (GL_ARRAY_BUFFER, m_lockOffset * m_vertexSize, m_lockCount * m_vertexSize, memory_buffer) );
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
#else
        m_memory->setBuffer( nullptr, 0 );

        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        MENGINE_GLCALL( glUnmapBuffer, (GL_ARRAY_BUFFER) );
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
#endif

        m_lockOffset = 0;
        m_lockCount = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_vertexCount )
        {
            return false;
        }

        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );

        const uint32_t bufferOffset = _offset * m_vertexSize;
        const uint32_t bufferSize = _count * m_vertexSize;

        MENGINE_GLCALL( glBufferSubData, (GL_ARRAY_BUFFER, bufferOffset, bufferSize, _buffer) );
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexBuffer::enable()
    {
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexBuffer::disable()
    {
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexBuffer::release()
    {
        if( m_id != 0 )
        {
            OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
                ->getUnknown();

            extension->deleteBuffer( m_id );

            m_id = 0;
        }

        m_vertexCapacity = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexBuffer::onRenderReset()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexBuffer::onRenderRestore()
    {
        MENGINE_ASSERTION_FATAL( m_id == 0 );

        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        GLuint id = extension->genBuffer();

        if( id == 0 )
        {
            return false;
        }

        m_id = id;

        const uint32_t bufferSize = m_vertexCapacity * m_vertexSize;

        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
        MENGINE_GLCALL( glBufferData, (GL_ARRAY_BUFFER, bufferSize, nullptr, m_usage) );
        MENGINE_GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}