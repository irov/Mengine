#pragma once

#include "Interface/RenderPipelineInterface.h"
#include "Interface/RenderBatchInterface.h"
#include "Interface/RenderExternalInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/RenderPrimitive.h"
#include "Kernel/Pool.h"
#include "Kernel/Vector.h"
#include "Kernel/DynamicArray.h"

#include "math/box2.h"

#ifndef MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX
#define MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX 512
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderObject
    {
        uint32_t materialSmartId;

        const RenderVertex2D * vertexData;
        uint32_t vertexCount;

        const RenderIndex * indexData;
        uint32_t indexCount;

        mt::box2f bb;

        uint32_t flags;
    };
    //////////////////////////////////////////////////////////////////////////
    struct RenderPass
    {
        uint32_t beginRenderObject;
        uint32_t countRenderObject;

        RenderBatchInterfacePtr batch;

        RenderVertexAttributeInterfacePtr vertexAttribute;
        RenderVertexBufferInterfacePtr vertexBuffer;
        RenderIndexBufferInterfacePtr indexBuffer;
        RenderProgramVariableInterfacePtr programVariable;

        RenderContext context;

        ZGroupType zGroup;
        ZIndexType zIndex;

        RenderExternalInterfacePtr external;

        const RenderObject * materialEnd[MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX];

        uint32_t flags;
    };
    //////////////////////////////////////////////////////////////////////////
    class BatchRenderPipeline
        : public RenderPipelineInterface
    {
        DECLARE_FACTORABLE( BatchRenderPipeline );

    public:
        BatchRenderPipeline();
        ~BatchRenderPipeline() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void addRenderMesh( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , uint32_t _vertexCount, uint32_t _indexCount
            , uint32_t _baseVertexIndex, uint32_t _startIndex, const DocumentPtr & _doc ) override;

        void addRenderObject( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indexCount
            , const mt::box2f * _bb, bool _debug, const DocumentPtr & _doc ) override;

        void addRenderQuad( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug, const DocumentPtr & _doc ) override;

        void addRenderLine( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug, const DocumentPtr & _doc ) override;

    public:
        void addRenderExternal( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderExternalInterfacePtr & _external
            , const DocumentPtr & _doc ) override;

    public:
        void addDebugRenderObject( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indexCount, const DocumentPtr & _doc ) override;

        void addDebugRenderQuad( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount, const DocumentPtr & _doc ) override;

    public:
        void endDebugLimitRenderObjects() override;
        void increfDebugLimitRenderObjects() override;
        bool decrefDebugLimitRenderObjects() override;

    public:
        void enableDebugFillrateCalcMode( bool _enable ) override;
        bool isDebugFillrateCalcMode() const override;

        void enableDebugStepRenderMode( bool _enable ) override;
        bool isDebugStepRenderMode() const override;

    public:
        void prepare() override;
        void flush() override;
        void batch() override;
        void render() override;
        void clear() override;
        bool isEmpty() const override;

    protected:
        bool testRenderPass_( const RenderContext * _context
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , const RenderVertexAttributeInterfacePtr & _vertexAttribute
            , const RenderProgramVariableInterfacePtr & _programVariable ) const;

        RenderPass & requestRenderPass_( const RenderContext * _context
            , const RenderBatchInterfacePtr & _batch
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , const RenderVertexAttributeInterfacePtr & _vertexAttribute
            , const RenderProgramVariableInterfacePtr & _programVariable );

    protected:
        void insertRenderObjects_( const RenderPass & _renderPass, const MemoryInterfacePtr & _vertexMemory, uint32_t _vertexSize, const MemoryInterfacePtr & _indexMemory, uint32_t * const _vbPos, uint32_t * const _ibPos );
        bool insertRenderObject_( const RenderObject * _renderObject, const MemoryInterfacePtr & _vertexMemory, uint32_t _vertexSize, const MemoryInterfacePtr & _indexMemory, uint32_t _vbPos, uint32_t _ibPos ) const;
            
    protected:
        RenderServiceInterface * m_renderService;

        ERenderBatchMode m_batchMode;

        typedef DynamicArray<RenderObject> DynamicArrayRenderObjects;
        DynamicArrayRenderObjects m_renderObjects;

        typedef DynamicArray<RenderPrimitive> DynamicArrayRenderPrimitives;
        DynamicArrayRenderPrimitives m_renderPrimitives;

        typedef Vector<RenderPass> VectorRenderPass;
        VectorRenderPass m_renderPasses;

        typedef DynamicArray<RenderIndex> DynamicArrayRenderIndices;
        DynamicArrayRenderIndices m_indicesQuad;
        DynamicArrayRenderIndices m_indicesLine;

#ifndef MENGINE_MASTER_RELEASE
        struct DebugRenderObject
        {
            RenderContext context;
            RenderMaterialInterfacePtr material;
            const RenderVertex2D * vertices;
            uint32_t vertexCount;
            const RenderIndex * indices;
            uint32_t indexCount;

#ifdef MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<DebugRenderObject> VectorDebugRenderObjects;
        VectorDebugRenderObjects m_debugRenderObjects;
#endif

#ifdef MENGINE_DEBUG
        bool m_debugFillrateCalcMode;
        bool m_debugStepRenderMode;
        bool m_debugStopRenderObjects;
        uint32_t m_debugLimitRenderObjects;
        uint32_t m_iterateRenderObjects;
#endif

    protected:
        void batchRenderObjectNormal_( DynamicArrayRenderObjects::iterator _roBegin, DynamicArrayRenderObjects::iterator _roEnd, DynamicArrayRenderPrimitives::iterator _rpBegin, RenderPrimitive * _rp, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t * _vbPos, uint32_t * _ibPos );
        void batchRenderObjectSmart_( const RenderPass & _renderPass, DynamicArrayRenderObjects::iterator _roBegin, DynamicArrayRenderPrimitives::iterator _rpBegin, const RenderObject * _ro, RenderPrimitive * _rp, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t * _vbPos, uint32_t * _ibPos );
    };
}