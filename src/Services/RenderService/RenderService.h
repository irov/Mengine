#pragma once

#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "RenderOrder.h"
#include "RenderBatch.h"
#include "RenderMaterial.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/ConstString.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/Factory.h"
#include "Kernel/Pool.h"
#include "Kernel/Vector.h"
#include "Kernel/List.h"
#include "Kernel/ConstString.h"

#include "math/mat4.h"
#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ERenderObjectFlag
    {
        RENDER_OBJECT_FLAG_NONE = 0x00000000,
        RENDER_OBJECT_FLAG_DEBUG = 0x00000001
    };
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
        const RenderTextureInterfacePtr & getNullTexture() const override;
        const RenderTextureInterfacePtr & getWhiteTexture() const override;

    public:
        void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen ) override;

    public:
        const RenderBatchInterfacePtr & requestRenderBatch( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount, uint32_t _indexCount, const DocumentPtr & _doc ) override;

    public:
        VectorRenderVertex2D & getDebugRenderVertex2D( uint32_t _count ) override;
        VectorRenderIndex & getDebugRenderIndex( uint32_t _count ) override;

    public:
        bool beginScene( const RenderPipelineInterfacePtr & _renderPipeline ) override;
        void endScene( const RenderPipelineInterfacePtr & _renderPipeline ) override;
        void swapBuffers() override;

    public:
        bool beginRenderPass( const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderContext * context ) override;

        void endRenderPass( const RenderContext * context ) override;

        void renderPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) override;

    public:
        void onWindowClose() override;
        void onDeviceLostPrepare() override;
        void onDeviceLostRestore() override;

    public:
        void setRenderViewport( const Viewport & _renderViewport ) override;
        const Viewport & getRenderViewport() const override;

    public:
        bool isWindowCreated() const override;

    public:
        const RenderServiceDebugInfo & getDebugInfo() const override;
        void resetFrameCount() override;

    public:
        void setVSync( bool _vSync ) override;
        bool getVSync() const override;

    public:
        const RenderOrderInterfacePtr & getRenderOrder( int32_t _index, const DocumentPtr & _doc ) override;

    protected:
        void clearFrameBuffer_();

    protected:
        void restoreTextureStage_( uint32_t _stage );

        void restoreRenderSystemStates_();
        void restoreRenderFrameStates_();

        void renderPrimitive_( const RenderPrimitive * _renderObject );

    protected:
        void calcRenderViewport_( const Viewport & _viewport, Viewport * const _renderViewport ) const;

    protected:
        bool makeBatches_( const RenderPipelineInterfacePtr & _renderPipeline );
        void flushRender_( const RenderPipelineInterfacePtr & _renderPipeline );

    protected:
        void calcQuadSquare_( const RenderVertex2D * _vertex, uint32_t _vertexNum, const Viewport & _viewport );
        void calcMeshSquare_( const RenderVertex2D * _vertex, uint32_t _vertexNum, const RenderIndex * _indices, uint32_t _indicesNum, const Viewport & _viewport );

    protected:
        bool createNullTexture_();
        bool createWhitePixelTexture_();

    protected:
        void updateTexture_( uint32_t _stageId, const RenderTextureInterfacePtr & _texture );
        void updateMaterial_( const RenderMaterialInterface * _material );
        void updateStage_( const RenderMaterialStage * _stage );

    protected:
        RenderSystemInterface * m_renderSystem;

        bool m_windowCreated;
        bool m_vsync;
        bool m_fullscreen;

        Resolution m_windowResolution;
        Resolution m_contentResolution;

        RenderTextureInterfacePtr m_nullTexture;
        RenderTextureInterfacePtr m_whiteTexture;

        FactoryPtr m_factoryRenderBatch;
        FactoryPtr m_factoryRenderOrder;

        typedef Vector<RenderOrderPtr> VectorRenderOrder;
        VectorRenderOrder m_renderOrders;

        typedef Vector<RenderBatchPtr> VectorRenderBatch;
        VectorRenderBatch m_renderBatches;
        VectorRenderBatch m_cacheRenderBatches;

        uint32_t m_maxVertexCount;
        uint32_t m_maxIndexCount;

        RenderVertexBufferInterfacePtr m_currentRenderVertexBuffer;
        RenderIndexBufferInterfacePtr m_currentRenderIndexBuffer;
        RenderProgramVariableInterfacePtr m_currentRenderProgramVariable;
        RenderProgramInterfacePtr m_currentRenderProgram;

        uint32_t m_currentRenderTextureStage;
        RenderTextureStage m_renderTextureStages[MENGINE_MAX_TEXTURE_STAGES];

        RenderTextureStage m_defaultRenderTextureStage;

        uint32_t m_currentMaterialId;
        const RenderMaterialStage * m_currentRenderMaterialStage;

        uint32_t m_currentTexturesID[MENGINE_MAX_TEXTURE_STAGES] = {0U};

        EBlendFactor m_currentBlendSrc;
        EBlendFactor m_currentBlendDst;
        EBlendOp m_currentBlendOp;

        EBlendFactor m_currentSeparateAlphaBlendSrc;
        EBlendFactor m_currentSeparateAlphaBlendDst;
        EBlendOp m_currentSeparateAlphaBlendOp;

        RenderContext m_currentRenderContext;
        
        RenderServiceDebugInfo m_debugInfo;

        Viewport m_renderViewport;

        typedef List<VectorRenderVertex2D> ListDebugVertices;
        ListDebugVertices m_debugRenderVertices;

        typedef List<VectorRenderIndex> ListDebugIndices;
        ListDebugIndices m_debugRenderIndices;

        bool m_currentSeparateAlphaBlendEnable;
        bool m_currentAlphaBlendEnable;
        bool m_currentDepthBufferTestEnable;
        bool m_currentDepthBufferWriteEnable;
    };
}
