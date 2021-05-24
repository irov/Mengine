#include "MockupRenderIndexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Documentable.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/DocumentableHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderIndexBuffer::MockupRenderIndexBuffer()
        : m_bufferType( BT_STATIC )
        , m_indexSize( 0 )
        , m_indexCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderIndexBuffer::~MockupRenderIndexBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderIndexBuffer::initialize( uint32_t _indexSize, EBufferType _bufferType )
    {
        m_indexSize = _indexSize;
        m_bufferType = _bufferType;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory proxy" );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderIndexBuffer::finalize()
    {
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderIndexBuffer::getIndexCount() const
    {
        return m_indexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderIndexBuffer::getIndexSize() const
    {
        return m_indexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderIndexBuffer::resize( uint32_t _count )
    {
        if( m_indexCount >= _count )
        {
            return true;
        }

        m_indexCount = _count;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr MockupRenderIndexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCount )
        {
            LOGGER_ERROR( "lock count %d offset %d more max size %d (doc '%s')"
                , _count
                , _offset
                , m_indexCount
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderIndexBuffer" )
            );

            return nullptr;
        }

        m_memory->newBuffer( _count * m_indexSize );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderIndexBuffer::unlock()
    {
        m_memory->clearBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderIndexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _offset );
        MENGINE_UNUSED( _count );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}