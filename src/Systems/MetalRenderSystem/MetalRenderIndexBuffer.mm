#include "MetalRenderIndexBuffer.h"

#include "Interface/MemoryServiceInterface.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderIndexBuffer::MetalRenderIndexBuffer( MetalDeviceId _device )
        : m_device( _device )
        , m_buffer( nil )
        , m_bufferType( BT_STATIC )
        , m_indexSize( 0 )
        , m_indexCount( 0 )
        , m_lockOffset( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderIndexBuffer::~MetalRenderIndexBuffer() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderIndexBuffer::initialize( uint32_t _indexSize, EBufferType _bufferType )
    {
        m_indexSize = _indexSize;
        m_bufferType = _bufferType;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderIndexBuffer::finalize()
    {
        m_memory = nullptr;
        m_buffer = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderIndexBuffer::getIndexCount() const
    {
        return m_indexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderIndexBuffer::getIndexSize() const
    {
        return m_indexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderIndexBuffer::resize( uint32_t _count )
    {
        if( m_indexCount >= _count )
        {
            return true;
        }

        m_indexCount = _count;

        m_buffer = [m_device newBufferWithLength:m_indexCount * m_indexSize options:MTLResourceStorageModeShared];

        return m_buffer != nil;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr MetalRenderIndexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCount )
        {
            LOGGER_ASSERTION( "offset %u count %u more max size %u", _offset, _count, m_indexCount );

            return nullptr;
        }

        m_lockOffset = _offset;

        m_memory->newBuffer( _count * m_indexSize );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderIndexBuffer::unlock()
    {
        if( m_buffer != nil )
        {
            memcpy( ((uint8_t *)[m_buffer contents]) + m_lockOffset * m_indexSize, m_memory->getBuffer(), m_memory->getSize() );
        }

        m_memory->clearBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MetalBufferId MetalRenderIndexBuffer::getBuffer() const
    {
        return m_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderIndexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCount )
        {
            LOGGER_ASSERTION( "offset %u count %u more max size %u", _offset, _count, m_indexCount );

            return false;
        }

        if( m_buffer == nil )
        {
            return false;
        }

        memcpy( ((uint8_t *)[m_buffer contents]) + _offset * m_indexSize, _buffer, _count * m_indexSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}

