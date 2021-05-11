#include "MockupRenderVertexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Documentable.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderVertexBuffer::MockupRenderVertexBuffer()
        : m_bufferType( BT_STATIC )
        , m_vertexSize( 0 )
        , m_vertexCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderVertexBuffer::~MockupRenderVertexBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderVertexBuffer::initialize( uint32_t _vertexSize, EBufferType _bufferType )
    {
        m_vertexSize = _vertexSize;
        m_bufferType = _bufferType;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderVertexBuffer::finalize()
    {
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderVertexBuffer::getVertexCount() const
    {
        return m_vertexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderVertexBuffer::getVertexSize() const
    {
        return m_vertexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderVertexBuffer::resize( uint32_t _count )
    {
        if( m_vertexCount >= _count )
        {
            return true;
        }
        
        m_vertexCount = _count;                

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr MockupRenderVertexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_vertexCount )
        {
            LOGGER_ERROR( "offset %u count %u more max size %u"
                , _offset
                , _count                
                , m_vertexCount
            );

            return nullptr;
        }

        m_memory->newBuffer( _count * m_vertexSize );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderVertexBuffer::unlock()
    {
        m_memory->clearBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderVertexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _offset );
        MENGINE_UNUSED( _count );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}