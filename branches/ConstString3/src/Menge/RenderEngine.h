#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Core/Viewport.h"
#	include "Core/Resolution.h"
#	include "Core/Polygon.h"
#	include "Core/Holder.h"
#	include "Core/ConstString.h"
#	include "Core/Pool.h"

#	include "math/mat4.h"
#	include "math/vec4.h"

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

		LPT_MESH,

		LPT_FORCE_DWORD = 0x7FFFFFFF
	};

	struct RenderObject;
	struct RenderPass;

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
		bool initialize( size_t _maxQuadCount );

	public:
		bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, int _bits, bool _fullscreen, 
									WindowHandle _winHandle, int _FSAAType , int _FSAAQuality );

		void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, bool _fullscreen );

	public:
		void renderObject2D( const Material* _material, const Texture** _textures, mt::mat4f * const * _matrixUV, int _texturesNum,
			const Vertex2D * _vertices, size_t _verticesNum,
			ELogicPrimitiveType _type, size_t _indicesNum = 0, IBHandle ibHandle = 0 );

	public:
		VBHandle createVertexBuffer( const Vertex2D * _vertexies, std::size_t _verticesNum );
		IBHandle createIndicesBuffer( const unsigned short * _buffer, std::size_t _count );

		void releaseVertexBuffer( VBHandle _handle );
		void releaseIndicesBuffer( IBHandle _handle );

		void updateVertexBuffer( VBHandle _handle, const Vertex2D * _vertexies, std::size_t _verticesNum );
		void updateIndicesBuffer( IBHandle _handle, const unsigned short * _buffer, std::size_t _count );

	public:
		void screenshot( Texture* _renderTargetImage, const mt::vec4f & _rect );

		Resolution getBestDisplayResolution( const Resolution & _resolution, float _aspect );

		bool createMaterialGroup( const ConstString & _name, const Material & _material );
		const MaterialGroup * getMaterialGroup( const ConstString & _name );
		void removeMaterialGroup( const ConstString & _name );

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
		
		LightInterface * createLight( const ConstString & _name );
		void releaseLight( LightInterface * _light );

		void onDeviceRestored();

		void onWindowActive( bool _active );
		void onWindowClose();

		void setRenderTarget( const ConstString & _target, bool _clear = true );
		const ConstString & getRenderTarget() const;

		void newRenderPass( const Viewport & _renderViewport, const mt::mat3f & _wm );
		bool getCurrentRenderPass( Viewport & _viewport, mt::mat3f & _wm ) const;

		bool isWindowCreated() const;

		void setCamera( Camera * _camera );

		RenderPass * createRenderPass_();
		void setRenderPass( RenderPass * _pass );
		RenderPass * getRenderPass() const;
		

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
		void renderPass_( RenderObject* _renderObject );
		void disableTextureStage_( std::size_t _stage );

		void orthoOffCenterLHMatrix_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf );
		void setRenderSystemDefaults_( size_t _maxQuadCount );
		void restoreRenderSystemStates_();
		void render_();
		size_t makeBatch_( size_t _offset );
		bool makeBatches_( bool & _overflow );
		size_t batchRenderObjects_( RenderPass * _pass, size_t _startVertexPos );
		size_t insertRenderObjects_( RenderPass * _pass, Vertex2D * _vertexBuffer, size_t _offset );
		void flushRender_();
		void prepare2D_();
		void prepare3D_();
		void releaseRenderPass_( RenderPass* _renderCamera );
		size_t refillIndexBuffer2D_();
		bool recreate2DBuffers_( std::size_t _maxIndexCount );		

	private:
		RenderSystemInterface * m_interface;

		bool m_windowCreated;
		bool m_vsync;
		Resolution m_windowResolution;
		bool m_fullscreen;
		
		//Viewport m_currentRenderViewport;
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

		std::size_t m_currentTextureStages;
		TextureStage m_currentTextureStage[MENGE_MAX_TEXTURE_STAGES];
		const mt::mat4f * m_currentMatrixUV[MENGE_MAX_TEXTURE_STAGES];
		const mt::mat4f * m_currentMaskUV[MENGE_MAX_TEXTURE_STAGES];

		const Material * m_currentMaterial;

		int m_currentTexturesID[MENGE_MAX_TEXTURE_STAGES];
		//const mt::mat4f* m_uvMask[MENGE_MAX_TEXTURE_STAGES];
		EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;

		Camera * m_camera;

		typedef PoolVector<RenderPass> TPoolRenderPass;
		TPoolRenderPass m_poolRenderPass;

		typedef std::vector<RenderPass*> TVectorRenderPass;
		TVectorRenderPass m_passes;

		RenderPass* m_currentPass;

		typedef std::map<ConstString, Texture*> TMapTextures;
		TMapTextures m_textures;
		TMapTextures m_renderTargets;

		Texture* m_nullTexture;	// white pixel

		DebugInfo m_debugInfo;	// debug info

		typedef Pool<RenderObject> TPoolRenderObject;
		TPoolRenderObject m_renderObjectPool;

		typedef std::map<ConstString, MaterialGroup *> TMapMaterialGroup;
		TMapMaterialGroup m_mapMaterialGroup;

		uint16 m_primitiveIndexStart[LPT_PRIMITIVE_COUNT];
		uint16 m_primitiveIndexStride[LPT_PRIMITIVE_COUNT];
		uint16 m_primitiveVertexStride[LPT_PRIMITIVE_COUNT];
		uint16 m_primitiveCount[LPT_PRIMITIVE_COUNT];

		std::size_t m_vbPos;

		mt::vec2f m_renderScale;
		mt::vec2f m_renderOffset;

		Viewport m_viewport;

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
