#include "MetalRenderVertexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "MetalRenderEnum.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/AssertionMemoryPanic.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexBuffer::MetalRenderVertexBuffer()
        : m_vertexCapacity( 0 )
        , m_vertexCount( 0 )
        , m_vertexSize( 0 )
        , m_bufferType( BT_STATIC )
        , m_buffer( nil )
        , m_lockOffset( 0 )
        , m_lockCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexBuffer::~MetalRenderVertexBuffer()
    {
        MENGINE_ASSERTION_FATAL( m_buffer == nil, "vertex buffer is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::initialize( uint32_t _elementSize, EBufferType _bufferType )
    {
        m_vertexSize = _elementSize;
        m_bufferType = _bufferType;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        m_lockMemory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexBuffer::finalize()
    {
        m_lockMemory = nullptr;

        this->release();

        m_vertexCapacity = 0;
        m_vertexCount = 0;
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
    bool MetalRenderVertexBuffer::resize( uint32_t _vertexCount )
    {
        m_vertexCount = _vertexCount;

        if( m_vertexCapacity >= _vertexCount )
        {
            return true;
        }

        m_vertexCapacity = _vertexCount;

        const uint32_t bufferSize = m_vertexCapacity * m_vertexSize;

        MTLResourceOptions options = Helper::toMTLBufferOptions( m_bufferType );

        id<MTLBuffer> newBuffer = [m_device newBufferWithLength:bufferSize options:options];

        if( newBuffer == nil )
        {
            return false;
        }

        m_buffer = newBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr MetalRenderVertexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( m_lockMemory == nullptr )
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

        m_lockMemory->newBuffer( bufferSize );

        return m_lockMemory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::unlock()
    {
        if( m_buffer == nil )
        {
            return false;
        }

        void * memory_buffer = m_lockMemory->getBuffer();

        const uint32_t bufferOffset = m_lockOffset * m_vertexSize;
        const uint32_t bufferSize = m_lockCount * m_vertexSize;

        uint8_t * contents = (uint8_t *)[m_buffer contents];
        memcpy( contents + bufferOffset, memory_buffer, bufferSize );

        m_lockOffset = 0;
        m_lockCount = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( m_buffer == nil )
        {
            return false;
        }

        if( _offset + _count > m_vertexCount )
        {
            return false;
        }

        const uint32_t bufferOffset = _offset * m_vertexSize;
        const uint32_t bufferSize = _count * m_vertexSize;

        uint8_t * contents = (uint8_t *)[m_buffer contents];
        memcpy( contents + bufferOffset, _buffer, bufferSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLBuffer> MetalRenderVertexBuffer::getMetalBuffer() const
    {
        return m_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexBuffer::release()
    {
        m_buffer = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::reload()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexBuffer::onRenderReset()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexBuffer::onRenderRestore()
    {
        if( m_vertexCapacity == 0 )
        {
            return true;
        }

        const uint32_t bufferSize = m_vertexCapacity * m_vertexSize;

        MTLResourceOptions options = Helper::toMTLBufferOptions( m_bufferType );

        id<MTLBuffer> newBuffer = [m_device newBufferWithLength:bufferSize options:options];

        if( newBuffer == nil )
        {
            return false;
        }

        m_buffer = newBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
