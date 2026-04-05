#include "MetalRenderIndexBuffer.h"

#include "Interface/MemoryServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Environment/Metal/MetalRenderSystemExtensionInterface.h"

#include "MetalRenderEnum.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/AssertionMemoryPanic.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderIndexBuffer::MetalRenderIndexBuffer()
        : m_indexCapacity( 0 )
        , m_indexCount( 0 )
        , m_indexSize( 0 )
        , m_bufferType( BT_STATIC )
        , m_buffer( nil )
        , m_lockOffset( 0 )
        , m_lockCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderIndexBuffer::~MetalRenderIndexBuffer()
    {
        MENGINE_ASSERTION_FATAL( m_buffer == nil, "index buffer is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderIndexBuffer::initialize( uint32_t _indexSize, EBufferType _bufferType )
    {
        m_indexSize = _indexSize;
        m_bufferType = _bufferType;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        m_lockMemory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderIndexBuffer::finalize()
    {
        m_lockMemory = nullptr;

        this->release();
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
    bool MetalRenderIndexBuffer::resize( uint32_t _indexCount )
    {
        m_indexCount = _indexCount;

        if( m_indexCapacity >= _indexCount )
        {
            return true;
        }

        m_indexCapacity = _indexCount;

        const uint32_t bufferSize = m_indexCapacity * m_indexSize;

        MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        id<MTLDevice> device = extension->getMetalDevice();

        MTLResourceOptions options = Helper::toMTLBufferOptions( m_bufferType );

        id<MTLBuffer> newBuffer = [device newBufferWithLength:bufferSize options:options];

        if( newBuffer == nil )
        {
            return false;
        }

        m_buffer = newBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr MetalRenderIndexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( m_lockMemory == nullptr )
        {
            return nullptr;
        }

        if( _offset + _count > m_indexCount )
        {
            return nullptr;
        }

        m_lockOffset = _offset;
        m_lockCount = _count;

        const uint32_t bufferSize = m_lockCount * m_indexSize;

        m_lockMemory->newBuffer( bufferSize );

        return m_lockMemory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderIndexBuffer::unlock()
    {
        void * memory_buffer = m_lockMemory->getBuffer();

        const uint32_t bufferOffset = m_lockOffset * m_indexSize;
        const uint32_t bufferSize = m_lockCount * m_indexSize;

        uint8_t * contents = (uint8_t *)[m_buffer contents];
        memcpy( contents + bufferOffset, memory_buffer, bufferSize );

        m_lockOffset = 0;
        m_lockCount = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderIndexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCount )
        {
            return false;
        }

        const uint32_t bufferOffset = _offset * m_indexSize;
        const uint32_t bufferSize = _count * m_indexSize;

        uint8_t * contents = (uint8_t *)[m_buffer contents];
        memcpy( contents + bufferOffset, _buffer, bufferSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLBuffer> MetalRenderIndexBuffer::getMetalBuffer() const
    {
        return m_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderIndexBuffer::release()
    {
        m_buffer = nil;
        m_indexCapacity = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderIndexBuffer::onRenderReset()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderIndexBuffer::onRenderRestore()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
