#include "MetalRenderVertexBuffer.h"

#include "Interface/MemoryServiceInterface.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexBuffer::MetalRenderVertexBuffer( MetalDeviceId _device )
        : m_device( _device )
        , m_buffer( nil )
        , m_bufferType( BT_STATIC )
        , m_vertexSize( 0 )
        , m_vertexCount( 0 )
        , m_lockOffset( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexBuffer::~MetalRenderVertexBuffer() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::initialize( uint32_t _vertexSize, EBufferType _bufferType )
    {
        m_vertexSize = _vertexSize;
        m_bufferType = _bufferType;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexBuffer::finalize()
    {
        m_memory = nullptr;
        m_buffer = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderVertexBuffer::getVertexCount() const
    {
        return m_vertexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderVertexBuffer::getVertexSize() const
    {
        return m_vertexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::resize( uint32_t _count )
    {
        if( m_vertexCount >= _count )
        {
            return true;
        }

        m_vertexCount = _count;

        m_buffer = [m_device newBufferWithLength:m_vertexCount * m_vertexSize options:MTLResourceStorageModeShared];

        return m_buffer != nil;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr MetalRenderVertexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_vertexCount )
        {
            LOGGER_ASSERTION( "offset %u count %u more max size %u", _offset, _count, m_vertexCount );

            return nullptr;
        }

        m_lockOffset = _offset;

        m_memory->newBuffer( _count * m_vertexSize );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::unlock()
    {
        if( m_buffer != nil )
        {
            void * memoryBuffer = m_memory->getBuffer();
            size_t memorySize = m_memory->getSize();
            void * bufferContents = [m_buffer contents];

            memcpy( ((uint8_t *)bufferContents) + m_lockOffset * m_vertexSize, memoryBuffer, memorySize );
        }

        m_memory->clearBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MetalBufferId MetalRenderVertexBuffer::getBuffer() const
    {
        return m_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_vertexCount )
        {
            LOGGER_ASSERTION( "offset %u count %u more max size %u", _offset, _count, m_vertexCount );

            return false;
        }

        if( m_buffer == nil )
        {
            return false;
        }

        void * bufferContents = [m_buffer contents];
        memcpy( ((uint8_t *)bufferContents) + _offset * m_vertexSize, _buffer, _count * m_vertexSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}

