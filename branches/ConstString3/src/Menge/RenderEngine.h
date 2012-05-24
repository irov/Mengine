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

#	include "RenderMaterial.h"
#	include "Core/ConstString.h"

//#	include "ColourValue.h"

namespace Menge
{
	//struct Material;
	class Camera;
	class RenderTexture;
	struct Vertex2D;

	enum ELogicPrimitiveType
	{
		LPT_QUAD = 0,
		//LPT_TRIANGLE,
		LPT_LINE,
		//LPT_RECTANGLE,

		LPT_PRIMITIVE_COUNT,

		LPT_MESH,

		LPT_FORCE_DWORD = 0x7FFFFFFF
	};

	struct RenderObject;
	struct RenderPass;

	class RenderEngine
		: public Holder<RenderEngine>
		, public RenderSystemListener
		, public RenderServiceInterface
	{
	public:
		struct DebugInfo
		{
			size_t frameCount;
			size_t dips;
			size_t textureMemory;
			size_t textureCount;
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
		void renderObject2D( const RenderCameraInterface * _camera, const RenderMaterial* _material, const RenderTextureInterface* const * _textures, mt::mat4f * const * _matrixUV, int _texturesNum,
			const Vertex2D * _vertices, size_t _verticesNum, 
			ELogicPrimitiveType _type, size_t _indicesNum = 0, IBHandle ibHandle = 0 );

	public:
		VBHandle createVertexBuffer( const Vertex2D * _vertexies, size_t _verticesNum );
		IBHandle createIndicesBuffer( const unsigned short * _buffer, size_t _count );

		void releaseVertexBuffer( VBHandle _handle );
		void releaseIndicesBuffer( IBHandle _handle );

		void updateVertexBuffer( VBHandle _handle, const Vertex2D * _vertexies, size_t _verticesNum );
		void updateIndicesBuffer( IBHandle _handle, const unsigned short * _buffer, size_t _count );

	public:
		void screenshot( RenderTextureInterface * _renderTargetImage, const mt::vec4f & _rect ) override;

		Resolution getBestDisplayResolution( const Resolution & _resolution, float _aspect );

		bool createMaterialGroup( const ConstString & _name, const RenderMaterial & _material );
		const RenderMaterialGroup * getMaterialGroup( const ConstString & _name );
		void removeMaterialGroup( const ConstString & _name );

		bool hasTexture( const WString & _filename );

		RenderTextureInterface * createTexture( size_t _width, size_t _height, PixelFormat _format ) override;
		void releaseTexture( const RenderTextureInterface* _texture ) override;

		void cacheFileTexture( const WString& _filename, RenderTextureInterface* _texture );
		
		//Astralax
		RenderTextureInterface * createRenderTargetTexture( size_t _width, size_t _height, PixelFormat _format ) override;
		
		void setRenderTargetTexture( RenderTextureInterface * _image, bool _clear ) override;
		void clear( uint32 _color ) override;
		void setSeparateAlphaBlendMode() override;

		//RenderTextureInterface * getTexture( const ConstString & _name ) const;
		//bool validTexture( const ConstString& _pakName, const WString& _filename, const ConstString& _codec );

		RenderTextureInterface* loadTexture( const ConstString& _pakName, const WString& _filename, const ConstString& _codec );
		RenderTextureInterface* loadTextureCombineRGBAndAlpha( const ConstString& _pakName, const WString & _fileNameRGB, const WString & _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha );
				
		bool saveImage( RenderTextureInterface* _image, const ConstString& _fileSystemName, const WString & _filename );
		
		
		//void	setProjectionMatrix( const mt::mat4f& _projection );
		//void	setViewMatrix( const mt::mat4f& _view );
		//void	setWorldMatrix( const mt::mat4f& _world );

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void beginLayer2D();
		void endLayer2D();
		void beginLayer3D();
		void endLayer3D();

		void onDeviceRestored();

		void onWindowActive( bool _active );
		void onWindowClose();

		void setRenderTarget( const ConstString & _target, bool _clear = true );
		const ConstString & getRenderTarget() const;

		bool isWindowCreated() const;
		
		void makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport );
		void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf );
		void makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport );

		const DebugInfo& getDebugInfo() const;
		void resetFrameCount();
		void enableTextureFiltering( bool _enable );
		bool isTextureFilteringEnabled() const;

		bool isResolutionAppropriate() const;

		void setVSync( bool _vSync ) override;
		bool getVSync() const override;

		bool supportA8() const;

	private:
		RenderPass * createRenderPass_();
		void destroyTexture_( const RenderTextureInterface * _texture );

		void renderPass_( const RenderObject* _renderObject );
		void disableTextureStage_( size_t _stage );

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
		bool recreate2DBuffers_( size_t _maxIndexCount );		
		
	private:
		RenderSystemInterface * m_interface;

		size_t m_maxQuadCount;

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

		size_t m_currentTextureStages;
		RenderTextureStage m_currentTextureStage[MENGE_MAX_TEXTURE_STAGES];
		const mt::mat4f * m_currentMatrixUV[MENGE_MAX_TEXTURE_STAGES];
		const mt::mat4f * m_currentMaskUV[MENGE_MAX_TEXTURE_STAGES];

		const RenderMaterial * m_currentMaterial;

		int m_currentTexturesID[MENGE_MAX_TEXTURE_STAGES];
		//const mt::mat4f* m_uvMask[MENGE_MAX_TEXTURE_STAGES];
		EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;

		typedef PoolVector<RenderPass> TPoolRenderPass;
		TPoolRenderPass m_poolRenderPass;

		typedef std::vector<RenderPass*> TVectorRenderPass;
		TVectorRenderPass m_passes;

		RenderPass* m_currentPass;
		const RenderCameraInterface * m_currentRenderCamera;

		typedef std::map<WString, RenderTextureInterface*> TMapTextures;
		TMapTextures m_textures;

		RenderTexture* m_nullTexture;	// dummy white pixel

		DebugInfo m_debugInfo;	// debug info

		//typedef Pool<RenderObject> TPoolRenderObject;
		//TPoolRenderObject m_renderObjectPool;

		typedef std::map<ConstString, RenderMaterialGroup *> TMapMaterialGroup;
		TMapMaterialGroup m_mapMaterialGroup;

		uint16 m_primitiveIndexStart[LPT_PRIMITIVE_COUNT];
		uint16 m_primitiveIndexStride[LPT_PRIMITIVE_COUNT];
		uint16 m_primitiveVertexStride[LPT_PRIMITIVE_COUNT];
		uint16 m_primitiveCount[LPT_PRIMITIVE_COUNT];

		size_t m_vbPos;

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
