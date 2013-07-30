#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#	include "Core/Viewport.h"
#	include "Core/Resolution.h"
#	include "Core/ConstString.h"

#   include "stdex/static_array.h"

#   include "Factory/FactoryPool.h"

#	include "math/mat4.h"
#	include "math/vec4.h"

#	include <vector>

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

//#	include "ColourValue.h"


#   ifndef MENGINE_RENDER_OBJECTS_MAX
#   define MENGINE_RENDER_OBJECTS_MAX 4000
#   endif

#   ifndef MENGINE_RENDER_PASS_MAX
#   define MENGINE_RENDER_PASS_MAX 200
#   endif

#   ifndef MENGINE_RENDER_INDICES_QUAD
#   define MENGINE_RENDER_INDICES_QUAD 12000
#   endif

#   ifndef MENGINE_RENDER_INDICES_LINE
#   define MENGINE_RENDER_INDICES_LINE 4000
#   endif


namespace Menge
{
	//struct Material;
	class Camera;

	struct RenderVertex2D;

	struct ImageCodecDataInfo;

	struct RenderObject
    {
		const RenderMaterial * material;

		size_t textureStages;
		RenderTextureInterface * textures[MENGE_MAX_TEXTURE_STAGES];

		EPrimitiveType primitiveType;

		const RenderVertex2D * vertexData;
		size_t verticesNum;

        const uint16 * indicesData;
        size_t indicesNum;

		size_t minIndex;
		size_t startIndex;

        size_t dipVerticesNum;
        size_t dipIndiciesNum;        

		IBHandle ibHandle;
        VBHandle vbHandle;
		size_t baseVertexIndex;
	};
	//////////////////////////////////////////////////////////////////////////
	struct RenderPass
	{
		size_t beginRenderObject;
		size_t countRenderObject;

		const RenderCameraInterface * camera;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderEngine
		: public RenderServiceInterface
        , public RenderSystemListener
	{
	public:
		RenderEngine();
		~RenderEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _provider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize( size_t _maxVertexCount, size_t _maxIndexCount ) override;
		void finalize() override;

	public:
		bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, int _bits, bool _fullscreen, 
									WindowHandle _winHandle, int _FSAAType , int _FSAAQuality ) override;

	public:
		void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, bool _fullscreen ) override;

	public:
		void addRenderObject( const RenderCameraInterface * _camera, const RenderMaterial * _material, const RenderTextureInterfacePtr * _textures, size_t _texturesNum
            , EPrimitiveType _type
            , const RenderVertex2D * _vertices, size_t _verticesNum 
			, const uint16 * _indices, size_t _indicesNum ) override;

        void addRenderQuad( const RenderCameraInterface * _camera, const RenderMaterial* _material, const RenderTextureInterfacePtr * _textures, size_t _texturesNum
            , const RenderVertex2D * _vertices, size_t _verticesNum ) override;

        void addRenderLine( const RenderCameraInterface * _camera, const RenderMaterial* _material, const RenderTextureInterfacePtr * _textures, size_t _texturesNum
            , const RenderVertex2D * _vertices, size_t _verticesNum ) override;

	public:
		VBHandle createVertexBuffer( const RenderVertex2D * _vertexies, size_t _verticesNum );
		IBHandle createIndicesBuffer( const unsigned short * _buffer, size_t _count );

		void releaseVertexBuffer( VBHandle _handle );
		void releaseIndicesBuffer( IBHandle _handle );

		bool updateVertexBuffer( VBHandle _handle, const RenderVertex2D * _vertexies, size_t _verticesNum );
		bool updateIndicesBuffer( IBHandle _handle, const uint16 * _buffer, size_t _count );

	public:
		void screenshot( const RenderTextureInterfacePtr & _renderTargetImage, const mt::vec4f & _rect ) override;

    public:
		void setRenderTargetTexture( const RenderTextureInterfacePtr & _image, bool _clear ) override;
		void clear( uint32 _color ) override;
		void setSeparateAlphaBlendMode() override;

        void enableDebugMode( bool _enable ) override;
        
		
    public:
		//void	setProjectionMatrix( const mt::mat4f& _projection );
		//void	setViewMatrix( const mt::mat4f& _view );
		//void	setWorldMatrix( const mt::mat4f& _world );

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;

        void onRenderSystemDeviceLost() override;
		bool onRenderSystemDeviceRestored() override;

		void onWindowActive( bool _active );
		void onWindowClose() override;

		void setRenderTarget( const ConstString & _target, bool _clear = true );
		const ConstString & getRenderTarget() const;

        const Viewport & getRenderViewport() const;

		bool isWindowCreated() const override;
		
		void makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
		void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
		void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float zn, float zf ) override;
		void makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport ) override;
        void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _at, const mt::vec3f & _up ) override;

		const RenderDebugInfo & getDebugInfo() const override;
		void resetFrameCount() override;

		bool isResolutionAppropriate() const;

		void setVSync( bool _vSync ) override;
		bool getVSync() const override;

        size_t getMemorySize( size_t _width, size_t _height, size_t _depth, PixelFormat _format ) const override;

	private:			
		void disableTextureStage_( size_t _stage );

		void setRenderSystemDefaults_();
		void restoreRenderSystemStates_();
		
		void renderPasses_();
		void renderPass_( const RenderPass & _renderPass );

		void renderObjects_( const RenderPass & _renderPass );
		void renderObject_( const RenderObject* _renderObject );

        bool makeBatches_();
		void makeBatch_( size_t & _vbSize, size_t & _ibSize );		
		void batchRenderObjects_( RenderPass * _pass, size_t & _vbSize, size_t & _ibSize );
		void batchRenderObject_( RenderObject * _renderObject, RenderObject ** _batchedObject, size_t & _vbSize, size_t & _ibSize ) const;
		
        void insertRenderPasses_( RenderVertex2D * _vertexBuffer, uint16 * _indeciesBuffer );
        void insertRenderObjects_( RenderPass * _pass, RenderVertex2D * _vertexBuffer, uint16 * _indeciesBuffer, size_t & _vbPos, size_t & _ibPos );
		void insertRenderObject_( RenderObject * _renderObject, RenderVertex2D * _vertexBuffer, uint16 * _indeciesBuffer, size_t & _vbPos, size_t & _ibPos ) const;
		void flushRender_();
		void prepare2D_();

		bool recreate2DBuffers_();
        		
    private:
        void calcQuadSquare_( const RenderVertex2D * _vertex, size_t _vertexNum );
        void calcMeshSquare_( const RenderVertex2D * _vertex, size_t _verteNum, const uint16 * _indices, size_t _indicesNum );

    protected:
        bool createNullTexture_();

	private:
        ServiceProviderInterface * m_serviceProvider;

		bool m_windowCreated;
		bool m_vsync;
		Resolution m_windowResolution;
		bool m_fullscreen;
		
		Resolution m_renderTargetResolution;

		Resolution m_contentResolution;
		
		ConstString m_currentRenderTarget;
        ConstString m_defaultRenderTarget;

		mt::mat4f m_renderAreaProj;
        
        RenderTextureInterfacePtr m_nullTexture;	// dummy white pixel

        RenderShaderInterface * m_shader;

		VBHandle m_vbHandle2D;
		IBHandle m_ibHandle2D;

		typedef std::vector<VBHandle> TVectorVertexBuffer;
		TVectorVertexBuffer m_vertexBuffer;

		typedef std::vector<IBHandle> TVectorIndexBuffer;
		TVectorIndexBuffer m_indexBuffer;

		size_t m_maxVertexCount;
		size_t m_maxIndexCount;
		
		VBHandle m_currentVBHandle;
		VBHandle m_currentIBHandle;

		size_t m_currentBaseVertexIndex;

		size_t m_currentTextureStages;
		RenderTextureStage m_currentTextureStage[MENGE_MAX_TEXTURE_STAGES];

		const RenderMaterial * m_currentMaterial;

		size_t m_currentTexturesID[MENGE_MAX_TEXTURE_STAGES];

        EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;

		const RenderCameraInterface * m_currentRenderCamera;

		RenderDebugInfo m_debugInfo;	    // debug info

		uint16 m_indicesQuad[MENGINE_RENDER_INDICES_QUAD];
        uint16 m_indicesLine[MENGINE_RENDER_INDICES_LINE];

		Viewport m_renderViewport;

		uint32 m_currentVertexDeclaration;
        
        typedef stdex::static_array<RenderObject, MENGINE_RENDER_OBJECTS_MAX> TArrayRenderObject;
        TArrayRenderObject m_renderObjects;

        typedef stdex::static_array<RenderPass, MENGINE_RENDER_PASS_MAX> TArrayRenderPass;
        TArrayRenderPass m_renderPasses;
                      
		bool m_depthBufferWriteEnable;
		bool m_alphaBlendEnable;
		bool m_alphaTestEnable;

        bool m_debugMode;
	};
}
