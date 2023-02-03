#pragma once

#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/FactoryInterface.h"

#include "RenderBatch.h"
#include "RenderMaterial.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/ConstString.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/Pool.h"
#include "Kernel/Vector.h"
#include "Kernel/List.h"
#include "Kernel/ConstString.h"

#include "math/mat4.h"
#include "math/vec4.h"

namespace Mengine
{
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
        bool createRenderWindow( const Resolution & _windowResolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _vsync, uint32_t _bits, bool _fullscreen, int32_t _FSAAType, int32_t _FSAAQuality ) override;
        void destroyRenderWindow() override;

    public:
        const RenderTextureInterfacePtr & getNullTexture() const override;
        const RenderTextureInterfacePtr & getWhiteTexture() const override;

    public:
        void changeWindowMode( const Resolution & _windowResolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen ) override;

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
        bool onDeviceLostRestore() override;

    public:
        void setRenderViewport( const Viewport & _renderViewport ) override;
        const Viewport & getRenderViewport() const override;

    public:
        bool isWindowCreated() const override;

    public:
        void setVSync( bool _vSync ) override;
        bool getVSync() const override;

    protected:
        void clearFrameBuffer_();

    protected:
        void restoreRenderSystemStates_();
        void restoreRenderFrameStates_();
        void restoreTextureStage_( uint32_t _stage );

    protected:
        void renderPrimitive_( const RenderPrimitive * _renderObject );

    protected:
        void calcRenderViewport_( const Viewport & _viewport, Viewport * const _renderViewport ) const;

    protected:
        bool makeBatches_( const RenderPipelineInterfacePtr & _renderPipeline );
        void flushRender_( const RenderPipelineInterfacePtr & _renderPipeline );

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

        FactoryInterfacePtr m_factoryRenderBatch;

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

        uint32_t m_currentTexturesID[MENGINE_MAX_TEXTURE_STAGES];

        EBlendFactor m_currentBlendSrc;
        EBlendFactor m_currentBlendDst;
        EBlendOp m_currentBlendOp;

        EBlendFactor m_currentSeparateAlphaBlendSrc;
        EBlendFactor m_currentSeparateAlphaBlendDst;
        EBlendOp m_currentSeparateAlphaBlendOp;

        RenderContext m_currentRenderContext;
        
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
