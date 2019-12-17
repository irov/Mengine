#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderVertexAttributeInterface.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/MemoryInterface.h"

#ifndef MENGINE_RENDER_VERTEX_MAX_BATCH
#define MENGINE_RENDER_VERTEX_MAX_BATCH (65000U)
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderBatchLockData
    {
        MemoryInterfacePtr vertexMemory;
        MemoryInterfacePtr indexMemory;

        uint32_t vertexSize;
        uint32_t indexSize;

        uint32_t vbPos;
        uint32_t ibPos;
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderBatchInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void setVertexAttribute( const RenderVertexAttributeInterfacePtr & _vertexAttribute ) = 0;
        virtual const RenderVertexAttributeInterfacePtr & getVertexAttribute() = 0;

        virtual void setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) = 0;
        virtual const RenderVertexBufferInterfacePtr & getVertexBuffer() = 0;

        virtual void setIndexBuffer( const RenderIndexBufferInterfacePtr & _vertexBuffer ) = 0;
        virtual const RenderIndexBufferInterfacePtr & getIndexBuffer() = 0;

    public:
        virtual RenderBatchLockData * getLockData() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderBatchInterface> RenderBatchInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}