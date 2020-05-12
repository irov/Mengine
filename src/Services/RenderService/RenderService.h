#pragma once

#include "Interface/RenderServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

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

#include "stdex/static_array.h"
#include "stdex/dynamic_array.h"

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
        bool beginRenderPass( const RenderVertexBufferInterfacePtr & _vertexBuffer, 
            const RenderIndexBufferInterfacePtr & _indexBuffer, 
            const RenderViewportInterfacePtr & _viewport,
            const RenderCameraInterfacePtr & _camera,
            const RenderTransformationInterfacePtr & _transformation,
            const RenderScissorInterfacePtr & _scissor,
            const RenderTargetInterfacePtr & _target,
            const RenderProgramVariableInterfacePtr & _programVariable ) override;

        void endRenderPass( const RenderTargetInterfacePtr & _target ) override;

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

    protected:
        void restoreTextureStage_( uint32_t _stage );

        void restoreRenderSystemStates_();
        void restoreRenderFrameStates_();
        
        void renderPrimitive_( const RenderPrimitive * _renderObject );

    protected:
        void calcRenderViewport_( const Viewport & _viewport, Viewport & _renderViewport ) const;

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
        Resolution m_windowResolution;
        bool m_fullscreen;

        Resolution m_contentResolution;

        RenderTextureInterfacePtr m_nullTexture;
        RenderTextureInterfacePtr m_whitePixelTexture;

        FactoryPtr m_factoryRenderBatch;

        typedef Vector<RenderBatchPtr> VectorRenderBatch;
        VectorRenderBatch m_renderBatches;
        VectorRenderBatch m_cacheRenderBatches;

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

        RenderServiceDebugInfo m_debugInfo;

        Viewport m_renderViewport;

        typedef List<VectorRenderVertex2D> ListDebugVertices;
        ListDebugVertices m_debugRenderVertices;

        typedef List<VectorRenderIndex> ListDebugIndices;
        ListDebugIndices m_debugRenderIndices;        

        bool m_depthBufferTestEnable;
        bool m_depthBufferWriteEnable;
        bool m_alphaBlendEnable;
    };
}
