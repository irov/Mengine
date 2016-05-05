#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#	include "RenderMaterial.h"

#	include "Core/Viewport.h"
#	include "Core/Resolution.h"
#	include "Core/ConstString.h"

#   include "stdex/static_array.h"
#	include "stdex/heap_array.h"

#   include "Factory/FactoryPool.h"

#	include "math/mat4.h"
#	include "math/vec4.h"

#	include <stdex/stl_vector.h>
#	include <stdex/stl_list.h>

#	include "Core/ConstString.h"

#	ifndef MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX
#	define MENGINE_RENDER_PATH_BATCH_MATERIAL_MAX 512
#	endif


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////	
	const uint32_t RO_FLAG_DEBUG = 0x00000001;
	//////////////////////////////////////////////////////////////////////////
	struct RenderObject
    {
		RenderMaterial * material;
		uint32_t materialSmartHash;

		RenderVertexBufferInterface * vbHandle;
		RenderIndexBufferInterface * ibHandle;

		const RenderVertex2D * vertexData;
		uint32_t verticesNum;

        const RenderIndices * indicesData;
        uint32_t indicesNum;

		mt::box2f bb;
				
		uint32_t minIndex;
		uint32_t startIndex;

        uint32_t dipVerticesNum;
        uint32_t dipIndiciesNum;

		uint32_t baseVertexIndex;

		uint32_t flags;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool hasRenderObjectFlag( const RenderObject * ro, uint32_t _flag )
	{
		return (ro->flags & _flag) > 0;
	}
	//////////////////////////////////////////////////////////////////////////
	struct RenderPass
	{
		uint32_t beginRenderObject;
		uint32_t countRenderObject;

		const RenderViewportInterface * viewport;
		const RenderCameraInterface * camera;
		const RenderClipplaneInterface * clipplane;
		const RenderTargetInterface * target;
				
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
			int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount ) override;

	public:
		void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen ) override;

	public:
		void addRenderObject( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _verticesNum 
			, const RenderIndices * _indices, uint32_t _indicesNum 
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
		RenderIndexBufferInterfacePtr createIndicesBuffer( const RenderIndices * _buffer, uint32_t _count );
		
		bool updateVertexBuffer( const RenderVertexBufferInterfacePtr & _vb, const RenderVertex2D * _vertexies, uint32_t _verticesNum );
		bool updateIndicesBuffer( const RenderIndexBufferInterfacePtr & _ib, const RenderIndices * _buffer, uint32_t _count );

	public:
		void screenshot( const RenderTextureInterfacePtr & _renderTargetImage, const mt::vec4f & _rect ) override;

    public:
		void clear( uint32_t _color ) override;
		void setSeparateAlphaBlendMode() override;

	public:
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
		
		void onWindowActive( bool _active );
		void onWindowClose() override;

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
		const RenderDebugInfo & getDebugInfo() const override;
		void resetFrameCount() override;

	public:
		void setVSync( bool _vSync ) override;
		bool getVSync() const override;

	protected:			
		void restoreTextureStage_( uint32_t _stage );

		void setRenderSystemDefaults_();
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
        void insertRenderPasses_( RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize );
        void insertRenderObjects_( RenderPass * _pass, RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos );
		bool insertRenderObject_( const RenderObject * _renderObject, RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t _vbPos, uint32_t _ibPos ) const;

		void flushRender_();

		bool create2DBuffers_();

    protected:
        void calcQuadSquare_( const RenderVertex2D * _vertex, uint32_t _vertexNum );
        void calcMeshSquare_( const RenderVertex2D * _vertex, uint32_t _verteNum, const RenderIndices * _indices, uint32_t _indicesNum );

    protected:
        bool createNullTexture_();
		bool createWhitePixelTexture_();

	protected:
		void updateTexture_( uint32_t _stageId, const RenderTextureInterfacePtr & _texture );
		void updateMaterial_( RenderMaterial * _material );
		void updateStage_( const RenderStage * _stage );

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
		uint32_t m_renderIndicesCount;

		ERenderBatchMode m_batchMode;

		uint32_t m_maxVertexCount;
		uint32_t m_maxIndexCount;
						
		RenderVertexBufferInterfacePtr m_currentVBHandle;
		RenderIndexBufferInterfacePtr m_currentIBHandle;

		uint32_t m_currentTextureStages;
		RenderTextureStage m_currentTextureStage[MENGE_MAX_TEXTURE_STAGES];
		
		uint32_t m_currentMaterialId;
		const RenderStage * m_currentStage;
		mt::mat4f m_mvpMat;
		

		uint32_t m_currentTexturesID[MENGE_MAX_TEXTURE_STAGES];

        EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;
		EBlendOp m_currentBlendOp;

		const RenderViewportInterface * m_currentRenderViewport;
		const RenderCameraInterface * m_currentRenderCamera;
		const RenderClipplaneInterface * m_currentRenderClipplane;
		const RenderTargetInterface * m_currentRenderTarget;

		RenderProgramInterfacePtr m_currentProgram;

		RenderDebugInfo m_debugInfo;	    // debug info

		typedef stdex::heap_array<RenderIndices> TArrayRenderIndices;
		TArrayRenderIndices m_indicesQuad;
        TArrayRenderIndices m_indicesLine;

		Viewport m_renderViewport;
       
        typedef stdex::heap_array<RenderObject> TArrayRenderObject;
        TArrayRenderObject m_renderObjects;

        typedef stdex::heap_array<RenderPass> TArrayRenderPass;
        TArrayRenderPass m_renderPasses;

		typedef stdex::vector<RenderVertex2D> TArrayRenderVertex2D;
		typedef stdex::list<TArrayRenderVertex2D> TListDebugVertices;
		TListDebugVertices m_debugVertices;
              
		bool m_depthBufferWriteEnable;
		bool m_alphaBlendEnable;

        bool m_debugStepRenderMode;
		bool m_debugRedAlertMode;

		bool m_stopRenderObjects;
		uint32_t m_limitRenderObjects;
		uint32_t m_iterateRenderObjects;
		
	protected:
		void batchRenderObjectNormal_( TArrayRenderObject::iterator _begin, TArrayRenderObject::iterator _end, RenderObject * _ro, RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos );
		void batchRenderObjectSmart_( RenderPass * _renderPass, TArrayRenderObject::iterator _begin, RenderObject * _ro, RenderVertex2D * _vertexBuffer, RenderIndices * _indicesBuffer, uint32_t _vbSize, uint32_t _ibSize, uint32_t & _vbPos, uint32_t & _ibPos );
	};
}
