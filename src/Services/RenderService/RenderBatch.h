#pragma once

#include "Interface/RenderBatchInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderBatch
        : public RenderBatchInterface
    {
        DECLARE_FACTORABLE( RenderBatch );

    public:
        RenderBatch();
        ~RenderBatch() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setVertexAttribute( const RenderVertexAttributeInterfacePtr & _vertexAttribute ) override;
        const RenderVertexAttributeInterfacePtr & getVertexAttribute() const override;

        void setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;
        const RenderVertexBufferInterfacePtr & getVertexBuffer() const override;

        void setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;
        const RenderIndexBufferInterfacePtr & getIndexBuffer() const override;

    public:
        void restore();

    public:
        bool process( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount, uint32_t _indexCount );

    public:
        bool lock();
        bool unlock();

    public:
        void deviceLostPrepare();
        void deviceLostRestore();

    public:
        RenderBatchLockData * getLockData() override;

    protected:
        RenderBatchLockData m_lockData;

        RenderVertexAttributeInterfacePtr m_vertexAttribute;
        RenderVertexBufferInterfacePtr m_vertexBuffer;
        RenderIndexBufferInterfacePtr m_indexBuffer;

        uint32_t m_vertexCount;
        uint32_t m_indexCount;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderBatch, RenderBatchInterface> RenderBatchPtr;
    //////////////////////////////////////////////////////////////////////////
}