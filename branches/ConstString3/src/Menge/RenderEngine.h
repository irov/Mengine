#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Core/Viewport.h"
#	include "Core/Resolution.h"
#	include "Core/Holder.h"
#	include "Core/ConstString.h"
#	include "Core/Pool.h"

#	include "math/mat4.h"
#	include "math/vec4.h"
#	include "math/polygon.h"

#	include <vector>
#	include <map>

#	include "Material.h"
#	include "Core/ConstString.h"
//#	include "ColourValue.h"

namespace Menge
{
	//struct Material;
	class Camera;
	class Texture;
	struct Vertex2D;

	enum ELogicPrimitiveType
	{
		LPT_QUAD = 0,
		LPT_TRIANGLE,
		LPT_LINE,
		LPT_RECTANGLE,

		LPT_PRIMITIVE_COUNT,
		LPT_FORCE_DWORD = 0x7FFFFFFF
	};

	struct RenderObject
	{
		const Material * material;

		std::size_t textureStages;
		const Texture* textures[MENGE_MAX_TEXTURE_STAGES];
		
		mt::mat4f * matrixUV[MENGE_MAX_TEXTURE_STAGES];

		ELogicPrimitiveType logicPrimitiveType;
		EPrimitiveType primitiveType;

		unsigned char* vertexData;
		size_t verticesNum;
		size_t minIndex;
		size_t startIndex;
		size_t dipIndiciesNum;
		size_t dipVerticesNum;
		
	};

	typedef std::vector<RenderObject*> TVectorRenderObject;

	struct RenderCamera
	{
		Camera* camera;
		TVectorRenderObject solidObjects;
		TVectorRenderObject blendObjects;

		RenderCamera()
			: camera( NULL )
		{
		}
	};

	class RenderEngine
		: public Holder<RenderEngine>
		, public RenderSystemListener
	{
	public:
		struct DebugInfo
		{
			size_t frameCount;
			size_t dips;
			size_t textureMemory;
		};

	public:
		RenderEngine();
		~RenderEngine();

	public:
		bool initialize( int _maxQuadCount );

		bool createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, 
									WindowHandle _winHandle, int _FSAAType , int _FSAAQuality );

		void screenshot( Texture* _renderTargetImage, const mt::vec4f & _rect );

		Resolution getBestDisplayResolution( const Resolution & _resolution, float _aspect );

		bool createMaterialGroup( const ConstString & _name, const Material & _material );
		const MaterialGroup * getMaterialGroup( const ConstString & _name );
		void removeMaterialGroup( const ConstString & _name );

		void renderObject2D( const Material* _material, const Texture** _textures, mt::mat4f ** _matrixUV, int _texturesNum,
								Vertex2D* _vertices, size_t _verticesNum,
								ELogicPrimitiveType _type, bool _solid );

		bool hasTexture( const ConstString & _name );

		Texture* createTexture( const ConstString & _name, size_t _width, size_t _height, PixelFormat _format );
		Texture* createTexture_( const ConstString & _name, size_t _width, size_t _height, PixelFormat _format );

		Texture* createRenderTargetTexture( const ConstString & _name, const mt::vec2f & _resolution );
		Texture* loadTexture( const ConstString& _pakName, const ConstString& _filename, const ConstString& _codec );
		bool saveImage( Texture* _image, const ConstString& _fileSystemName, const String & _filename );

		void releaseTexture( const Texture* _texture );
	
		//void	setProjectionMatrix( const mt::mat4f& _projection );
		//void	setViewMatrix( const mt::mat4f& _view );
		//void	setWorldMatrix( const mt::mat4f& _world );

		bool beginScene();
		void endScene();
		void swapBuffers();
		void beginLayer2D();
		void endLayer2D();
		void beginLayer3D();
		void endLayer3D();

		void applyRenderViewport( const Viewport & _renderViewport );

		void changeWindowMode( const Resolution & _resolution, bool _fullscreen );
		void setViewportDimensions( const Resolution & _resolution, float _renderFactor = 0.0f );

		LightInterface * createLight( const ConstString & _name );
		void releaseLight( LightInterface * _light );

		void onDeviceRestored();

		void onWindowActive( bool _active );
		void onWindowClose();

		void setRenderTarget( const ConstString & _target, bool _clear = true );
		const ConstString & getRenderTarget() const;
		void setRenderViewport( const Viewport & _renderViewport );
		const Viewport & getRenderViewport() const;

		const mt::mat4f& getViewTransform() const;

		bool isWindowCreated() const;

		void setActiveCamera( Camera* _camera );
		Camera* getActiveCamera() const;

		void setProjectionMatrix2D_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf );

		const DebugInfo& getDebugInfo() const;
		void resetFrameCount();
		void enableTextureFiltering( bool _enable );
		bool isTextureFilteringEnabled() const;

		void setVSync( bool _vSync );
		bool getVSync() const;

		bool supportA8() const;

	private:
		void destroyTexture_( const Texture* _texture );

		size_t batch_( TVectorRenderObject & _objects, size_t _startVertexPos, bool textureSort );
		bool checkForBatch_( RenderObject* _prev, RenderObject* _next );
		void renderPass_( RenderObject* _renderObject );
		void enableTextureStage_( std::size_t _stage, bool _enable );

		void orthoOffCenterLHMatrix_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf );
		void setRenderSystemDefaults_( int _maxQuadCount );
		void restoreRenderSystemStates_();
		void render_();
		void makeBatches_();
		size_t insertRenderObjects_( unsigned char* _vertexBuffer, size_t _offset, TVectorRenderObject& _renderObjects );
		void flushRender_();
		void prepare2D_();
		void prepare3D_();
		void releaseRenderCamera_( RenderCamera* _renderCamera );
		size_t refillIndexBuffer2D_();
		bool recreate2DBuffers_( std::size_t _maxIndexCount );		

	private:
		RenderSystemInterface * m_interface;

		bool m_windowCreated;
		bool m_vsync;
		Resolution m_windowResolution;
		bool m_fullscreen;
		
		Viewport m_renderViewport;
		Viewport m_currentRenderViewport;
		Resolution m_renderTargetResolution;
		
		ConstString m_currentRenderTarget;

		bool m_layer3D;
		mt::mat4f m_renderAreaProj;
		mt::mat4f m_worldTransform;
		mt::mat4f m_viewTransform;
		mt::mat4f m_projTransform;


		VBHandle m_vbHandle2D;
		IBHandle m_ibHandle2D;
		size_t m_maxVertices2D;

		VBHandle m_vbHandle3D;
		IBHandle m_ibHandle3D;

		VBHandle m_currentVBHandle;
		VBHandle m_currentIBHandle;

		RenderObject* m_batchedObject;
		float m_layerZ;

		std::size_t m_currentTextureStages;
		TextureStage m_currentTextureStage[MENGE_MAX_TEXTURE_STAGES];
		const mt::mat4f * m_currentMatrixUV[MENGE_MAX_TEXTURE_STAGES];
		const mt::mat4f * m_currentMaskUV[MENGE_MAX_TEXTURE_STAGES];

		int m_currentTexturesID[MENGE_MAX_TEXTURE_STAGES];
		//const mt::mat4f* m_uvMask[MENGE_MAX_TEXTURE_STAGES];
		EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;

		typedef std::vector<RenderCamera*> TVectorRenderCamera;
		TVectorRenderCamera m_cameras;
		RenderCamera* m_activeCamera;

		typedef std::map<ConstString, Texture*> TMapTextures;
		TMapTextures m_textures;
		TMapTextures m_renderTargets;

		Texture* m_nullTexture;	// white pixel

		DebugInfo m_debugInfo;	// debug info

		typedef Pool<RenderCamera, PoolPlacementPolicyNone> TPoolRenderCamera;
		TPoolRenderCamera m_renderCameraPool;

		typedef Pool<RenderObject> TPoolRenderObject;
		TPoolRenderObject m_renderObjectPool;

		typedef std::map<ConstString, MaterialGroup *> TMapMaterialGroup;
		TMapMaterialGroup m_mapMaterialGroup;

		std::size_t m_primitiveIndexStart[LPT_PRIMITIVE_COUNT];
		std::size_t m_primitiveIndexStride[LPT_PRIMITIVE_COUNT];
		std::size_t m_primitiveVertexStride[LPT_PRIMITIVE_COUNT];
		std::size_t m_primitiveCount[LPT_PRIMITIVE_COUNT];

		std::size_t m_maxIndexCount;

		std::size_t m_vbPos;
		std::size_t m_vbVertexSize;

		uint32 m_currentVertexDeclaration;

		size_t m_dipCount;
		bool m_depthBufferWriteEnable;
		bool m_alphaBlendEnable;
		bool m_alphaTestEnable;
		bool m_textureFiltering;

		bool m_supportA8;

		int m_idEnumerator;
	};
}
