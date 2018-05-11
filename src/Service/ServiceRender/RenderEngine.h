#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "Core/ServiceBase.h"

#include "RenderMaterial.h"

#include "Core/Viewport.h"
#include "Core/Resolution.h"
#include "Core/ConstString.h"

#include "stdex/static_array.h"
#include "stdex/heap_array.h"

#include "Factory/Factory.h"

#include "math/mat4.h"
#include "math/vec4.h"

#include "stdex/stl_vector.h"
#include "stdex/stl_list.h"

#include "Core/ConstString.h"

#ifndef MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX
#	define MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX 512
#endif

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////	
    enum ERenderObjectFlag
    {
        RENDER_OBJECT_FLAG_DEBUG = 0x00000001
    };
    //////////////////////////////////////////////////////////////////////////
    enum ERenderPassFlag
    {
        RENDER_PASS_FLAG_SINGLE = 0x00000001
    };
	//////////////////////////////////////////////////////////////////////////
	struct RenderObject
    {
		RenderMaterialInterface * material;
		uint32_t materialSmartId;

		RenderVertexBufferInterface * vertexBuffer;
		RenderIndexBufferInterface * indexBuffer;

		const RenderVertex2D * vertexData;
		uint32_t vertexCount;

        const RenderIndex * indexData;
        uint32_t indexCount;

		mt::box2f bb;
				
		uint32_t minIndex;
		uint32_t startIndex;

        uint32_t dipVerticesNum;
        uint32_t dipIndiciesNum;

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

		RenderViewportInterfacePtr viewport;
		RenderCameraInterfacePtr camera;
		RenderClipplaneInterfacePtr clipplane;
		
        RenderTargetInterfacePtr target;
				
		const RenderObject * materialEnd[MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX];

        uint32_t flags;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderPass> RenderPassPtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderEngine
		: public ServiceBase<RenderServiceInterface>
	{
	public:
		RenderEngine();
		~RenderEngine() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, uint32_t _bits, bool _fullscreen, 
			int _FSAAType, int _FSAAQuality ) override;

        void destroyRenderWindow() override;

	public:
		void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen ) override;

	public:
        void addRenderMesh( const RenderState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , uint32_t _indexCount ) override;

		void addRenderObject( const RenderState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
			, const RenderIndex * _indices, uint32_t _indexCount 
			, const mt::box2f * _bb, bool _debug ) override;

		void addRenderQuad( const RenderState * _state, const RenderMaterialInterfacePtr & _material
			, const RenderVertex2D * _vertices, uint32_t _vertexCount
			, const mt::box2f * _bb, bool _debug ) override;

		void addRenderLine( const RenderState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
			, const mt::box2f * _bb, bool _debug ) override;

    protected:
        const RenderBatchPtr & requestRenderBatch_( const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _vertexCount );
        bool testRenderPass_( const RenderState * _state, const RenderBatchPtr & _renderBatch ) const;
        const RenderPassPtr & requestRenderPass_( const RenderState * _state, const RenderMaterialInterfacePtr & _material, uint32_t _vertexCount, uint32_t _indexCount );

	public:
		RenderVertex2D * getDebugRenderVertex2D( size_t _count ) override;

	public:
		void setBatchMode( ERenderBatchMode _mode ) override;
		ERenderBatchMode getBatchMode() const override;

	public:
		void screenshot( const RenderTextureInterfacePtr & _renderTargetImage, const mt::vec4f & _rect ) override;

    public:
		void clear( uint32_t _color ) override;
		void setSeparateAlphaBlendMode() override;

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
		void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
		void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
		void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float zn, float zf ) override;
		void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;
        void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) override;
		
	public:
		const RenderServiceDebugInfo & getDebugInfo() const override;
		void resetFrameCount() override;

	public:
		void setVSync( bool _vSync ) override;
		bool getVSync() const override;

	protected:			
		void restoreTextureStage_( uint32_t _stage );

		void restoreRenderSystemStates_();
		
		void renderPasses_();
		void renderPass_( const RenderPassPtr & _renderPass );

		void renderObjects_( const RenderPassPtr & _renderPass );
		void renderObject_( RenderObject * _renderObject );

	protected:
		void calcRenderViewport_( const Viewport & _viewport, Viewport & _renderViewport ) const;
		
	protected:
        bool makeBatches_();

	protected:
        void insertRenderPasses_();
        void insertRenderObjects_( const RenderPassPtr & _renderPass, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos );
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
						
		RenderVertexBufferInterfacePtr m_currentVertexBuffer;
		RenderIndexBufferInterfacePtr m_currentIndexBuffer;

		uint32_t m_currentTextureStages;
		RenderTextureStage m_currentTextureStage[MENGINE_MAX_TEXTURE_STAGES];
		
		uint32_t m_currentMaterialId;
		const RenderMaterialStage * m_currentStage;

		uint32_t m_currentTexturesID[MENGINE_MAX_TEXTURE_STAGES];

        EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;
		EBlendOp m_currentBlendOp;

		const RenderViewportInterface * m_currentRenderViewport;
		const RenderCameraInterface * m_currentRenderCamera;
		const RenderClipplaneInterface * m_currentRenderClipplane;
		
        RenderTargetInterfacePtr m_currentRenderTarget;

		RenderProgramInterfacePtr m_currentProgram;

		RenderServiceDebugInfo m_debugInfo;	    // debug info

		typedef stdex::heap_array<RenderIndex> TArrayRenderIndices;
		TArrayRenderIndices m_indicesQuad;
        TArrayRenderIndices m_indicesLine;

		Viewport m_renderViewport;

        FactoryPtr m_factoryRenderBatch;
        FactoryPtr m_factoryRenderPass;
       
        typedef stdex::heap_array<RenderObject> TArrayRenderObject;
        TArrayRenderObject m_renderObjects;

        typedef stdex::vector<RenderBatchPtr> TVectorRenderBatch;
        TVectorRenderBatch m_renderBatches;

        typedef stdex::vector<RenderPassPtr> TVectorRenderPass;
        TVectorRenderPass m_renderPasses;

		typedef stdex::vector<RenderVertex2D> TArrayRenderVertex2D;
		typedef stdex::list<TArrayRenderVertex2D> TListDebugVertices;
		TListDebugVertices m_debugVertices;
              
		bool m_depthBufferWriteEnable;
		bool m_alphaBlendEnable;
        
        bool m_debugFillrateCalcMode;
        bool m_debugStepRenderMode;
		bool m_debugRedAlertMode;

		bool m_stopRenderObjects;
		uint32_t m_limitRenderObjects;
		uint32_t m_iterateRenderObjects;
		
	protected:
		void batchRenderObjectNormal_( TArrayRenderObject::iterator _begin, TArrayRenderObject::iterator _end, RenderObject * _ro, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos );
		void batchRenderObjectSmart_( const RenderPassPtr & _renderPass, TArrayRenderObject::iterator _begin, RenderObject * _ro, const MemoryInterfacePtr & _vertexBuffer, uint32_t _vertexSize, const MemoryInterfacePtr & _indexBuffer, uint32_t & _vbPos, uint32_t & _ibPos );
	};
}
