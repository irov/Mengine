#include "RenderBatch.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderBatch::RenderBatch()
        : m_vertexCount( 0 )
        , m_indexCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderBatch::~RenderBatch()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderBatch::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderBatch::finalize()
    {
        m_lockData.vertexMemory = nullptr;
        m_lockData.indexMemory = nullptr;

        m_vertexAttribute = nullptr;

        m_vertexBuffer = nullptr;
        m_indexBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderBatch::setVertexAttribute( const RenderVertexAttributeInterfacePtr & _vertexAttribute )
    {
        m_vertexAttribute = _vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexAttributeInterfacePtr & RenderBatch::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderBatch::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        m_vertexBuffer = _vertexBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexBufferInterfacePtr & RenderBatch::getVertexBuffer() const
    {
        return m_vertexBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderBatch::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        m_indexBuffer = _indexBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderIndexBufferInterfacePtr & RenderBatch::getIndexBuffer() const
    {
        return m_indexBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderBatch::restore()
    {
        m_vertexCount = 0U;
        m_indexCount = 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderBatch::process( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount, uint32_t _indexCount )
    {
        if( m_vertexAttribute != _vertexAttribute )
        {
            return false;
        }

        if( m_vertexCount + _vertexCount >= MENGINE_RENDER_VERTEX_MAX_BATCH )
        {
            return false;
        }

        m_vertexCount += _vertexCount;
        m_indexCount += _indexCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderBatch::lock()
    {
        if( m_vertexCount == 0U )
        {
            m_lockData.vertexMemory = nullptr;
            m_lockData.indexMemory = nullptr;

            m_lockData.vertexSize = m_vertexBuffer->getVertexSize();
            m_lockData.indexSize = m_indexBuffer->getIndexSize();

            m_lockData.vbPos = 0U;
            m_lockData.ibPos = 0U;

            return true;
        }

        if( m_vertexBuffer->resize( m_vertexCount ) == false )
        {
            LOGGER_ERROR( "failed to resize vertex buffer '%u'"
                , m_vertexCount
            );

            return false;
        }

        MemoryInterfacePtr vertexMemory = m_vertexBuffer->lock( 0, m_vertexCount );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexMemory, "failed to lock vertex buffer '%u'"
            , m_vertexCount
        );

        m_lockData.vertexMemory = vertexMemory;

        if( m_indexBuffer->resize( m_indexCount ) == false )
        {
            LOGGER_ERROR( "failed to resize index buffer '%u'"
                , m_indexCount
            );

            return false;
        }

        MemoryInterfacePtr indexMemory = m_indexBuffer->lock( 0, m_indexCount );

        MENGINE_ASSERTION_MEMORY_PANIC( indexMemory, "failed to lock index buffer '%u'"
            , m_indexCount
        );

        m_lockData.indexMemory = indexMemory;

        m_lockData.vertexSize = m_vertexBuffer->getVertexSize();
        m_lockData.indexSize = m_indexBuffer->getIndexSize();

        m_lockData.vbPos = 0U;
        m_lockData.ibPos = 0U;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderBatch::unlock()
    {   
        if( m_vertexCount == 0U )
        {
            return true;
        }

        m_lockData.vertexMemory = nullptr;
        m_lockData.indexMemory = nullptr;

        m_lockData.vertexSize = 0U;
        m_lockData.indexSize = 0U;

        m_lockData.vbPos = 0U;
        m_lockData.ibPos = 0U;

        if( m_vertexBuffer->unlock() == false )
        {
            LOGGER_ERROR( "failed to unlock vertex buffer" );

            return false;
        }

        if( m_indexBuffer->unlock() == false )
        {
            LOGGER_ERROR( "failed to unlock index buffer" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderBatchLockData * RenderBatch::getLockData()
    {
        return &m_lockData;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderBatch::deviceLostPrepare()
    {
        m_lockData.vertexMemory = nullptr;
        m_lockData.indexMemory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderBatch::deviceLostRestore()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}