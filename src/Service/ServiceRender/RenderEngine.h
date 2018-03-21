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

#include "Factory/FactoryPool.h"

#include "math/mat4.h"
#include "math/vec4.h"

#include "stdex/stl_vector.h"
#include "stdex/stl_list.h"

#include "Core/ConstString.h"

#	ifndef MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX
#	define MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX 512
#	endif


namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////	
	const uint32_t RO_FLAG_DEBUG = 0x00000001;
	//////////////////////////////////////////////////////////////////////////
	struct RenderObject
    {
		RenderMaterialInterface * material;
		uint32_t materialSmartId;

		RenderVertexBufferInterface * vbHandle;
		RenderIndexBufferInterface * ibHandle;

		const RenderVertex2D * vertexData;
		uint32_t vertexCount;

        const RenderIndex * indicesData;
        uint32_t indicesCount;

		mt::box2f bb;
				
		uint32_t minIndex;
		uint32_t startIndex;

        uint32_t dipVerticesNum;
        uint32_t dipIndiciesNum;

		uint32_t baseVertexIndex;

		uint32_t flags;
	};
	//////////////////////////////////////////////////////////////////////////
	struct RenderPass
	{
		uint32_t beginRenderObject;
		uint32_t countRenderObject;

		const RenderViewportInterface * viewport;
		const RenderCameraInterface * camera;
		const RenderClipplaneInterface * clipplane;
		
        RenderTargetInterfacePtr target;
				
		const RenderObject * materialEnd[MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX];
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderEngine
		: public ServiceBase<RenderServiceInterface>
	{
	public:
		RenderEngine();
		~RenderEngine();

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
		void addRenderObject( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _verticesNum 
			, const RenderIndex * _indices, uint32_t _indicesNum 
			, const mt::box2f * _bb, bool _debug ) override;

		void addRenderQuad( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
			, const RenderVertex2D * _vertices, uint32_t _verticesNum 
			, const mt::box2f * _bb, bool _debug ) override;

		void addRenderLine( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _verticesNum
			, const mt::box2f * _bb, bool _debug ) override;
		
	protected:
		void createRenderPass_();

	public:
		RenderVertex2D * getDebugRenderVertex2D( size_t _count ) override;

	public:
		void setBatchMode( ERenderBatchMode _mode ) override;
		ERenderBatchMode getBatchMode() const override;

	public:
		RenderVertexBufferInterfacePtr createVertexBuffer( const RenderVertex2D * _vertexies, uint32_t _verticesNum );
		RenderIndexBufferInterfacePtr createIndicesBuffer( const RenderIndex * _buffer, uint32_t _count );
		
		bool updateVertexBuffer( const RenderVertexBufferInterfacePtr & _vb, const RenderVertex2D * _vertexies, uint32_t _verticesNum );
		bool updateIndicesBuffer( const RenderIndexBufferInterfacePtr & _ib, const RenderIndex * _buffer, uint32_t _count );

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
		void renderPass_( RenderPass & _renderPass );

		void renderObjects_( RenderPass & _renderPass );
		void renderObject_( RenderObject* _renderObject );

	protected:
		void calcRenderViewport_( const Viewport & _viewport, Viewport & _renderViewport ) const;
		
	protected:
        bool makeBatches_();

	protected:
        void insertRenderPasses_( RenderVertex2D * _vertexBuffer, RenderIndex * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize );
        void insertRenderObjects_( RenderPass * _pass, RenderVertex2D * _vertexBuffer, RenderIndex * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos );
		bool insertRenderObject_( const RenderObject * _renderObject, RenderVertex2D * _vertexBuffer, RenderIndex * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t _vbPos, uint32_t _ibPos ) const;

		void flushRender_();

		bool create2DBuffers_();

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

		RenderVertexBufferInterfacePtr m_vbHandle2D;
		RenderIndexBufferInterfacePtr m_ibHandle2D;

		uint32_t m_renderVertexCount;
		uint32_t m_renderIndexCount;

		ERenderBatchMode m_batchMode;

		uint32_t m_maxVertexCount;
		uint32_t m_maxIndexCount;
						
		RenderVertexBufferInterfacePtr m_currentVBHandle;
		RenderIndexBufferInterfacePtr m_currentIBHandle;

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
       
        typedef stdex::heap_array<RenderObject> TArrayRenderObject;
        TArrayRenderObject m_renderObjects;

        typedef stdex::vector<RenderPass> TVectorRenderPass;
        TVectorRenderPass m_renderPasses;

		typedef stdex::vector<RenderVertex2D> TArrayRenderVertex2D;
		typedef stdex::list<TArrayRenderVertex2D> TListDebugVertices;
		TListDebugVertices m_debugVertices;
              
		bool m_depthBufferWriteEnable;
		bool m_alphaBlendEnable;

		bool m_noShader;

        bool m_debugFillrateCalcMode;
        bool m_debugStepRenderMode;
		bool m_debugRedAlertMode;

		bool m_stopRenderObjects;
		uint32_t m_limitRenderObjects;
		uint32_t m_iterateRenderObjects;
		
	protected:
		void batchRenderObjectNormal_( TArrayRenderObject::iterator _begin, TArrayRenderObject::iterator _end, RenderObject * _ro, RenderVertex2D * _vertexBuffer, RenderIndex * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos );
		void batchRenderObjectSmart_( RenderPass * _renderPass, TArrayRenderObject::iterator _begin, RenderObject * _ro, RenderVertex2D * _vertexBuffer, RenderIndex * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos );
	};
}
