#pragma once

#include "Interface/RenderServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "Kernel/ServiceBase.h"

#include "RenderMaterial.h"

#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/ConstString.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"

#include "stdex/static_array.h"
#include "stdex/dynamic_array.h"

#include "Kernel/Factory.h"
#include "Kernel/Pool.h"

#include "math/mat4.h"
#include "math/vec4.h"

#include "Config/Vector.h"
#include "Config/List.h"

#include "Kernel/ConstString.h"

#ifndef MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX
#define MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX 512
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////	
    enum ERenderObjectFlag
    {
        RENDER_OBJECT_FLAG_NONE = 0x00000000,
        RENDER_OBJECT_FLAG_DEBUG = 0x00000001
    };
    //////////////////////////////////////////////////////////////////////////
    enum ERenderPassFlag
    {
        RENDER_PASS_FLAG_NONE = 0x00000000,
        RENDER_PASS_FLAG_SINGLE = 0x00000001
    };
    //////////////////////////////////////////////////////////////////////////
    struct RenderObject
    {
        RenderMaterialInterface * material;
        uint32_t materialSmartId;

        const RenderVertex2D * vertexData;
        uint32_t vertexCount;

        const RenderIndex * indexData;
        uint32_t indexCount;

        mt::box2f bb;

        uint32_t minIndex;
        uint32_t startIndex;

        uint32_t dipVerticesCount;
        uint32_t dipIndiciesCount;

        uint32_t baseVertexIndex;

        uint32_t flags;
    };
    //////////////////////////////////////////////////////////////////////////
    struct RenderBatch
        : public Factorable
    {
        RenderVertexAttributeInterfacePtr vertexAttribute;

        RenderVertexBufferInterfacePtr vertexBuffer;

        uint32_t vertexCount;
        MemoryInterfacePtr vertexMemory;

        RenderIndexBufferInterfacePtr indexBuffer;

        uint32_t indexCount;
        MemoryInterfacePtr indexMemory;

        uint32_t vbPos;
        uint32_t ibPos;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderBatch> RenderBatchPtr;
    //////////////////////////////////////////////////////////////////////////
    struct RenderPass
        : public Factorable
    {
        uint32_t beginRenderObject;
        uint32_t countRenderObject;

        RenderBatchPtr batch;

        RenderVertexAttributeInterfacePtr vertexAttribute;
        RenderVertexBufferInterfacePtr vertexBuffer;
        RenderIndexBufferInterfacePtr indexBuffer;

        RenderViewportInterfacePtr viewport;
        RenderCameraInterfacePtr camera;
        RenderTransformationInterfacePtr transformation;
        RenderScissorInterfacePtr scissor;
        RenderTargetInterfacePtr target;
        RenderProgramVariableInterfacePtr programVariable;

        const RenderObject * materialEnd[MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX];

        uint32_t flags;
    };
    //////////////////////////////////////////////////////////////////////////
    //typedef IntrusivePtr<RenderPass> RenderPassPtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderService
        : public ServiceBase<RenderServiceInterface>
    {
    public:
        RenderService();
        ~RenderService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, uint32_t _bits, bool _fullscreen,
            int32_t _FSAAType, int32_t _FSAAQuality ) override;

        void destroyRenderWindow() override;

    public:
        void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen ) override;

    public:
        void addRenderMesh( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , uint32_t _vertexCount, uint32_t _indexCount, const Char * _doc ) override;

        void addRenderObject( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _variable
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indexCount
            , const mt::box2f * _bb, bool _debug, const Char * _doc ) override;

        void addRenderQuad( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug, const Char * _doc ) override;

        void addRenderLine( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug, const Char * _doc ) override;

    public:
        void addDebugRenderObject( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indexCount, const Char * _doc ) override;

        void addDebugRenderQuad( const RenderContext * _context
            , const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount, const Char * _doc ) override;

    protected:
        const RenderBatchPtr & requestRenderBatch_( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount );

        bool testRenderPass_( const RenderContext * _context
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , const RenderVertexAttributeInterfacePtr & _vertexAttribute
            , const RenderProgramVariableInterfacePtr & _programVariable ) const;

        RenderPass * requestRenderPass_( const RenderContext * _context
            , const RenderBatchPtr & _batch
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , const RenderVertexAttributeInterfacePtr & _vertexAttribute
            , const RenderProgramVariableInterfacePtr & _programVariable );

    public:
        VectorRenderVertex2D & getDebugRenderVertex2D( uint32_t _count ) override;
        VectorRenderIndex & getDebugRenderIndex( uint32_t _count ) override;

    public:
        void setBatchMode( ERenderBatchMode _mode ) override;
        ERenderBatchMode getBatchMode() const override;

    public:
        void enableDebugFillrateCalcMode( bool _enable ) override;
        bool isDebugFillrateCalcMode() const override;

        void enableDebugStepRenderMode( bool _enable ) override;
        bool isDebugStepRenderMode() const override;

        void enableRedAlertMode( bool _enable ) override;
        bool isRedAlertMode() const override;

        void endLimitRenderObjects() override;
        void increfLimitRenderObjects() override;
        bool decrefLimitRenderObjects() override;

    public:
        bool beginScene() override;
        void endScene() override;
        void swapBuffers() override;

    public:
        void onWindowClose() override;
        void onDeviceLostPrepare() override;
        void onDeviceLostRestore() override;

    public:
        const Viewport & getRenderViewport() const;

    public:
        bool isWindowCreated() const override;

    public:
        const RenderServiceDebugInfo & getDebugInfo() const override;
        void resetFrameCount() override;

    public:
        void setVSync( bool _vSync ) override;
        bool getVSync() const override;

    protected:
        void restoreTextureStage_( uint32_t _stage );

        void restoreRenderSystemStates_();
        void restoreRenderFrameStates_();

        void renderPasses_();
        void renderPass_( const RenderPass * _renderPass );

        void renderObjects_( const RenderPass * _renderPass );
        void renderObject_( RenderObject * _renderObject );

    protected:
        void calcRenderViewport_( const Viewport & _viewport, Viewport & _renderViewport ) const;

    protected:
        bool makeBatches_();

    protected:
        void insertRenderPasses_();
        void insertRenderObjects_( const RenderPass * _renderPass, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos );
        bool insertRenderObject_( const RenderObject * _renderObject, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t _vbPos, uint32_t _ibPos ) const;

        void flushRender_();

    protected:
        void calcQuadSquare_( const RenderVertex2D * _vertex, uint32_t _vertexNum, const Viewport & _viewport );
        void calcMeshSquare_( const RenderVertex2D * _vertex, uint32_t _vertexNum, const RenderIndex * _indices, uint32_t _indicesNum, const Viewport & _viewport );

    protected:
        bool createNullTexture_();
        bool createWhitePixelTexture_();

    protected:
        void updateTexture_( uint32_t _stageId, const RenderTextureInterfacePtr & _texture );
        void updateMaterial_( RenderMaterialInterface * _material );
        void updateStage_( const RenderMaterialStage * _stage );

    protected:
        RenderSystemInterface * m_renderSystem;

        bool m_windowCreated;
        bool m_vsync;
        Resolution m_windowResolution;
        bool m_fullscreen;

        Resolution m_contentResolution;

        RenderTextureInterfacePtr m_nullTexture;	// dummy white pixel
        RenderTextureInterfacePtr m_whitePixelTexture;

        ERenderBatchMode m_batchMode;

        uint32_t m_maxVertexCount;
        uint32_t m_maxIndexCount;

        RenderVertexBufferInterfacePtr m_currentRenderVertexBuffer;
        RenderIndexBufferInterfacePtr m_currentRenderIndexBuffer;
        RenderProgramVariableInterfacePtr m_currentRenderProgramVariable;
        RenderProgramVariableInterfacePtr m_defaultRenderProgramVariable;

        uint32_t m_currentRenderTextureStage;
        RenderTextureStage m_renderTextureStages[MENGINE_MAX_TEXTURE_STAGES];

        RenderTextureStage m_defaultRenderTextureStage;

        uint32_t m_currentMaterialId;
        const RenderMaterialStage * m_currentRenderMaterialStage;

        uint32_t m_currentTexturesID[MENGINE_MAX_TEXTURE_STAGES];

        EBlendFactor m_currentBlendSrc;
        EBlendFactor m_currentBlendDst;
        EBlendOp m_currentBlendOp;

        RenderViewportInterfacePtr m_currentRenderViewport;
        RenderCameraInterfacePtr m_currentRenderCamera;
        RenderTransformationInterfacePtr m_currentRenderTransformation;
        RenderScissorInterfacePtr m_currentRenderScissor;
        RenderProgramInterfacePtr m_currentRenderProgram;

        RenderServiceDebugInfo m_debugInfo;	    // debug info

        typedef stdex::dynamic_array<RenderIndex> DynamicArrayRenderIndices;
        DynamicArrayRenderIndices m_indicesQuad;
        DynamicArrayRenderIndices m_indicesLine;

        Viewport m_renderViewport;

        FactoryPtr m_factoryRenderBatch;

        typedef stdex::dynamic_array<RenderObject> ArrayRenderObject;
        ArrayRenderObject m_renderObjects;

        typedef Vector<RenderBatchPtr> VectorRenderBatch;
        VectorRenderBatch m_renderBatches;

        typedef Pool<RenderPass, 128> PoolRenderPass;
        PoolRenderPass m_poolRenderPass;

        typedef Vector<RenderPass *> VectorRenderPass;
        VectorRenderPass m_renderPasses;

        struct DebugRenderObject
        {
            RenderContext context;
            RenderMaterialInterfacePtr material;
            const RenderVertex2D * vertices;
            uint32_t vertexCount;
            const RenderIndex * indices;
            uint32_t indexCount;
        };

        typedef Vector<DebugRenderObject> VectorDebugRenderObjects;
        VectorDebugRenderObjects m_debugRenderObjects;

        typedef List<VectorRenderVertex2D> ListDebugVertices;
        ListDebugVertices m_debugRenderVertices;

        typedef List<VectorRenderIndex> ListDebugIndices;
        ListDebugIndices m_debugRenderIndices;

        VectorRenderBatch m_cacheRenderBatches;

        bool m_depthBufferTestEnable;
        bool m_depthBufferWriteEnable;
        bool m_alphaBlendEnable;

        bool m_debugFillrateCalcMode;
        bool m_debugStepRenderMode;
        bool m_debugRedAlertMode;

        bool m_stopRenderObjects;
        uint32_t m_limitRenderObjects;
        uint32_t m_iterateRenderObjects;

    protected:
        void batchRenderObjectNormal_( ArrayRenderObject::iterator _begin, ArrayRenderObject::iterator _end, RenderObject * _ro, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos );
        void batchRenderObjectSmart_( const RenderPass * _renderPass, ArrayRenderObject::iterator _begin, RenderObject * _ro, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos );
    };
}
