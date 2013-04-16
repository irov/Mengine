#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Core/Viewport.h"
#	include "Core/Resolution.h"
#	include "Core/ConstString.h"
#	include "Core/Pool.h"

#	include "math/mat4.h"
#	include "math/vec4.h"

#	include <vector>
#	include <map>

#	include "Core/ConstString.h"

//#	include "ColourValue.h"

namespace Menge
{
	//struct Material;
	class Camera;
	class RenderTexture;

	class Megatextures;

	struct Vertex2D;

	struct ImageCodecDataInfo;

	struct RenderObject
	{
		const RenderMaterial * material;

		size_t textureStages;
		const RenderTextureInterface * textures[MENGE_MAX_TEXTURE_STAGES];

		ELogicPrimitiveType logicPrimitiveType;
		EPrimitiveType primitiveType;

		const Vertex2D * vertexData;
		size_t verticesNum;
		size_t minIndex;
		size_t startIndex;

		size_t dipIndiciesNum;
		size_t dipVerticesNum;

		IBHandle ibHandle;
		size_t baseVertexIndex;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<RenderObject> TVectorRenderObject;
	//////////////////////////////////////////////////////////////////////////
	struct RenderPass
	{
		TVectorRenderObject::size_type beginRenderObject;
		TVectorRenderObject::size_type countRenderObject;

		const RenderCameraInterface * camera;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<RenderPass> TVectorRenderPass;
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
		bool initialize( size_t _maxQuadCount ) override;
		void finalize() override;

	public:
		bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, int _bits, bool _fullscreen, 
									WindowHandle _winHandle, int _FSAAType , int _FSAAQuality ) override;

	protected:
		void createWhitePixelTexture_();

	public:
		void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, bool _fullscreen ) override;

	public:
		void addRenderObject2D( const RenderCameraInterface * _camera, const RenderMaterial* _material, const RenderTextureInterface* const * _textures, size_t _texturesNum,
			const Vertex2D * _vertices, size_t _verticesNum, 
			ELogicPrimitiveType _type, size_t _indicesNum = 0, IBHandle ibHandle = 0 ) override;

	public:
		VBHandle createVertexBuffer( const Vertex2D * _vertexies, size_t _verticesNum );
		IBHandle createIndicesBuffer( const unsigned short * _buffer, size_t _count );

		void releaseVertexBuffer( VBHandle _handle );
		void releaseIndicesBuffer( IBHandle _handle );

		bool updateVertexBuffer( VBHandle _handle, const Vertex2D * _vertexies, size_t _verticesNum );
		bool updateIndicesBuffer( IBHandle _handle, const unsigned short * _buffer, size_t _count );

	public:
		void screenshot( RenderTextureInterface * _renderTargetImage, const mt::vec4f & _rect ) override;

		bool createMaterialGroup( const ConstString & _name, const RenderMaterial & _material );
		const RenderMaterialGroup * getMaterialGroup( const ConstString & _name ) const override;
		void removeMaterialGroup( const ConstString & _name );

		bool hasTexture( const FilePath & _filename ) const override;
		
		RenderTextureInterface * createTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
        //RenderTextureInterface * createPoolTexture( size_t _width, size_t _height, PixelFormat _format );

		RenderTextureInterface * createDynamicTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		//RenderTextureInterface * createMegatexture( size_t _width, size_t _height, PixelFormat _format );
		//RenderTextureInterface * createSubTexture( RenderTextureInterface * _texture, const Rect & _rect, RenderTextureInterfaceListener * _listener ) override;
		RenderTextureInterface * createRenderTargetTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		void releaseTexture( RenderTextureInterface* _texture ) override;
				
		void setRenderTargetTexture( RenderTextureInterface * _image, bool _clear ) override;
		void clear( uint32 _color ) override;
		void setSeparateAlphaBlendMode() override;

		//RenderTextureInterface * getTexture( const ConstString & _name ) const;
		//bool validTexture( const ConstString& _pakName, const WString& _filename, const ConstString& _codec );

        void enableDebugMode( bool _enable ) override;

	public:
		bool loadTextureRectImageData( RenderTextureInterface * _texture, const Rect & _rect, ImageDecoderInterface * _imageDecoder ) override;
		
	public:
		//void sweezleAlpha( RenderTextureInterface * _texture, unsigned char * _textureBuffer, size_t _texturePitch ) override;
		void imageQuality( RenderTextureInterface * _texture, unsigned char * _textureBuffer, size_t _texturePitch ) override;

	public:
		void cacheFileTexture( const FilePath& _filename, RenderTextureInterface* _texture ) override;

		RenderTextureInterface* loadTexture( const ConstString& _pakName, const FilePath& _filename, const ConstString& _codec ) override;
		RenderTextureInterface* loadMegatexture( const ConstString& _pakName, const FilePath& _filename, const ConstString& _codec );
						
		bool saveImage( RenderTextureInterface* _image, const ConstString& _fileSystemName, const FilePath & _filename ) override;
		
		RenderTextureInterface* getTexture( const FilePath& _filename ) override;
		//void	setProjectionMatrix( const mt::mat4f& _projection );
		//void	setViewMatrix( const mt::mat4f& _view );
		//void	setWorldMatrix( const mt::mat4f& _world );

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;

		bool onRenderSystemDeviceRestored() override;

		void onWindowActive( bool _active );
		void onWindowClose() override;

		void setRenderTarget( const ConstString & _target, bool _clear = true );
		const ConstString & getRenderTarget() const;

		bool isWindowCreated() const override;
		
		void makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
		void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
		void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float zn, float zf ) override;
		void makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport ) override;

		const RenderDebugInfo & getDebugInfo() const override;
		void resetFrameCount() override;
		void enableTextureFiltering( bool _enable ) override;
		bool isTextureFilteringEnabled() const override;

		bool isResolutionAppropriate() const;

		void setVSync( bool _vSync ) override;
		bool getVSync() const override;

	private:
		RenderPass * createRenderPass_();
		void destroyTexture_( RenderTextureInterface * _texture );
				
		void disableTextureStage_( size_t _stage );

		void setRenderSystemDefaults_( size_t _maxQuadCount );
		void restoreRenderSystemStates_();
		
		void renderPasses_();
		void renderPass_( const RenderPass & _renderPass );
		void renderObjects_( const RenderPass & _renderPass );
		void renderObject_( const RenderObject* _renderObject );

		size_t makeBatch_( size_t _offset );
		bool makeBatches_( bool & _overflow );
		size_t batchRenderObjects_( RenderPass * _pass, size_t _startVertexPos );
		bool batchRenderObject_( RenderObject * _renderObject, RenderObject * _batchedObject, size_t & _verticesNum ) const;
		size_t insertRenderObjects_( RenderPass * _pass, Vertex2D * _vertexBuffer, size_t _offset );
		size_t insertRenderObject_( RenderObject * _renderObject, Vertex2D * _vertexBuffer, size_t _offset ) const;
		void flushRender_();
		void prepare2D_();
		void prepare3D_();

		bool refillIndexBuffer2D_( size_t & _maxVertices );
		bool recreate2DBuffers_( size_t _maxIndexCount );

		const Viewport & getRenderViewport() const;
		
    private:
        void calcQuadSquare_( const Vertex2D * _vertex, size_t _num );

	private:
		RenderSystemInterface * m_renderSystem;

        ServiceProviderInterface * m_serviceProvider;

		size_t m_maxQuadCount;

		bool m_windowCreated;
		bool m_vsync;
		Resolution m_windowResolution;
		bool m_fullscreen;
		
		Resolution m_renderTargetResolution;

		Resolution m_contentResolution;
		
		ConstString m_currentRenderTarget;

		bool m_layer3D;
		mt::mat4f m_renderAreaProj;

		VBHandle m_vbHandle2D;
		IBHandle m_ibHandle2D;

		typedef std::vector<VBHandle> TVectorVertexBuffer;
		TVectorVertexBuffer m_vertexBuffer;

		typedef std::vector<IBHandle> TVectorIndexBuffer;
		TVectorIndexBuffer m_indexBuffer;

		size_t m_maxPrimitiveVertices2D;
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

		typedef std::map<FilePath, RenderTextureInterface*> TMapTextures;
		TMapTextures m_textures;

		RenderTextureInterface * m_nullTexture;	// dummy white pixel

		RenderDebugInfo m_debugInfo;	    // debug info

		//typedef Pool<RenderObject> TPoolRenderObject;
		//TPoolRenderObject m_renderObjectPool;

		typedef std::map<ConstString, RenderMaterialGroup *> TMapMaterialGroup;
		TMapMaterialGroup m_mapMaterialGroup;

		size_t m_primitiveIndexStart[LPT_PRIMITIVE_COUNT];
		size_t m_primitiveIndexStride[LPT_PRIMITIVE_COUNT];
		size_t m_primitiveVertexStride[LPT_PRIMITIVE_COUNT];
		size_t m_primitiveCount[LPT_PRIMITIVE_COUNT];

		mutable size_t m_vbPos;

		//mt::vec2f m_renderScale;
		//mt::vec2f m_renderOffset;

		Viewport m_renderViewport;

		uint32 m_currentVertexDeclaration;
        
        int m_idEnumerator;

        TVectorRenderObject m_renderObjects;
        TVectorRenderPass m_renderPasses;

        RenderPass * m_currentRenderPass;
        
		bool m_depthBufferWriteEnable;
		bool m_alphaBlendEnable;
		bool m_alphaTestEnable;
		bool m_textureFiltering;

        bool m_debugMode;
	};
}
