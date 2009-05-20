#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Viewport.h"

#	include "Resolution.h"

#	include "Holder.h"

#	include "math/mat4.h"
#	include "math/vec4.h"
#	include "math/polygon.h"

#	include <vector>
#	include <map>

#	include "Material.h"
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

		LPT_MESH,

		LPT_PRIMITIVE_COUNT,
		LPT_FORCE_DWORD = 0x7FFFFFFF
	};

	struct RenderObject
	{
		Material* material;
		ELogicPrimitiveType logicPrimitiveType;
		unsigned char* vertexData;
		size_t verticesNum;
		size_t startIndex;
		size_t dipIndiciesNum;
		size_t dipVerticesNum;
		EPrimitiveType primitiveType;
	};
	typedef std::vector<RenderObject*> TRenderObjectVector;

	struct RenderCamera
	{
		Camera* camera;
		TRenderObjectVector solidObjects;
		TRenderObjectVector blendObjects;

		RenderCamera::RenderCamera()
			: camera( NULL )
		{
		}
	};


	class RenderEngine
		: public RenderSystemListener
	{
	public:
		struct DebugInfo
		{
			size_t frameCount;
			size_t dips;
			size_t textureMemory;
		};

	public:
		RenderEngine( RenderSystemInterface * _interface );
		~RenderEngine();

	public:
		bool initialize();

		bool createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
								bool _waitForVSync, int _FSAAType , int _FSAAQuality );

		void screenshot( Texture* _renderTargetImage, const mt::vec4f & _rect );
		void render();

		void setContentResolution( const Resolution & _resolution );
		Resolution getBestDisplayResolution( const Resolution & _resolution, float _aspect );

		Material* createMaterial();
		void releaseMaterial( Material* _material );
		void renderObject2D( Material* _material, Vertex2D* _vertices, size_t _verticesNum, ELogicPrimitiveType _type );

		bool hasTexture( const String& _name );
		Texture* createTexture( const String & _name, size_t _width, size_t _height, PixelFormat _format );
		Texture* createRenderTargetTexture( const String & _name, const mt::vec2f & _resolution );
		Texture* loadTexture( const String & _filename );
		bool saveImage( Texture* _image, const String& _filename );

		void	releaseTexture( Texture* _texture );

		//void	setProjectionMatrix( const mt::mat4f& _projection );
		//void	setViewMatrix( const mt::mat4f& _view );
		//void	setWorldMatrix( const mt::mat4f& _world );

		void	beginScene();
		void	endScene();
		void	swapBuffers();
		void	beginLayer2D();
		void	endLayer2D();
		void	beginLayer3D();
		void	endLayer3D();

		void	setRenderArea( const mt::vec4f& _renderArea );

		void	setFullscreenMode( bool _fullscreen );
		bool	getFullscreenMode();
		void	setViewportDimensions( const Resolution & _resolution, float _renderFactor = 0.0f );

		LightInterface * createLight( const String & _name );
		void releaseLight( LightInterface * _light );

		void frameStarted();
		void frameEnded();

		void onDeviceRestored();

		void onWindowMovedOrResized();
		void onWindowActive( bool _active );
		void onWindowClose();

		void setRenderFactor( float _factor );
		void setRenderTarget( const String & _target, bool _clear = true );
		const mt::vec4f& getRenderArea() const;

		const mt::mat4f& getViewTransform() const;

		bool isWindowCreated() const;

		void setActiveCamera( Camera* _camera );
		Camera* getActiveCamera() const;

		void setProjectionMatrix2D_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf );

		const DebugInfo& getDebugInfo() const;
		void resetFrameCount();

	private:
		void recalcRenderArea_( const Resolution & resolution );
		void batch_( std::vector<RenderObject*>& _objects, bool textureSort );
		bool checkForBatch_( RenderObject* _prev, RenderObject* _next );
		void renderPass_( RenderObject* _renderObject );
		void enableTextureStage_( std::size_t _stage, bool _enable );

		void orthoOffCenterLHMatrix_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf );
		void setRenderSystemDefaults_();
		RenderCamera* getRenderCamera_();
		void releaseRenderCamera_( RenderCamera* _renderCamera );
		void render_();
		RenderObject* getRenderObject_();
		void releaseRenderObject( RenderObject* _renderObject );
		void makeBatches_( bool _2d );
		void insertRenderObjects_( void* _vertexBuffer, TRenderObjectVector& _renderObjects );
		void flushRender_();
		void prepare2D_();
		void prepare3D_();

	private:
		Menge::RenderSystemInterface * m_interface;
		Viewport m_renderViewport;
		bool m_windowCreated;
		bool m_fullscreen;
		float m_viewportWidth;
		float m_viewportHeight;
		float m_renderFactor;
		
		Resolution m_contentResolution;
		Resolution m_windowResolution;

		mt::vec4f m_renderArea;
		float m_rendFactPix;
		
		String m_currentRenderTarget;

		bool m_layer3D;
		mt::mat4f m_renderAreaProj;
		mt::mat4f m_worldTransfrom;
		mt::mat4f m_viewTransform;
		mt::mat4f m_projTransform;


		VBHandle m_vbHandle2D;
		IBHandle m_ibHandle2D;

		VBHandle m_vbHandle3D;
		IBHandle m_ibHandle3D;

		VBHandle m_currentVBHandle;
		VBHandle m_currentIBHandle;

		typedef std::vector<RenderObject*> TVectorRenderObjects;
		TVectorRenderObjects m_renderObjects;

		RenderObject* m_batchedObject;
		float m_layerZ;

		std::size_t m_currentTextureStages;
		TextureStage m_currentTextureStage[MENGE_MAX_TEXTURE_STAGES];
		EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;

		std::vector<RenderCamera*> m_cameras;
		RenderCamera* m_activeCamera;

		typedef std::map< String, Texture* > TTextureMap;
		TTextureMap m_textures;
		TTextureMap m_renderTargets;

		Texture* m_nullTexture;	// white pixel

		mt::vec4f m_currentRenderArea;

		DebugInfo m_debugInfo;	// debug info

		typedef std::vector<RenderCamera*> TRenderCameraVector;
		TRenderCameraVector m_renderCameraPool;

		std::size_t m_primitiveIndexStart[LPT_PRIMITIVE_COUNT];
		std::size_t m_primitiveIndexStride[LPT_PRIMITIVE_COUNT];
		std::size_t m_primitiveVertexStride[LPT_PRIMITIVE_COUNT];
		std::size_t m_vbPos;
		std::size_t m_vbVertexSize;

		uint32 m_currentVertexDeclaration;

		size_t m_dipCount;

	private:
		class FindCamera
		{
		public:
			FindCamera( Camera* _find )
				: m_find( _find )
			{
			}
			bool operator()( RenderCamera* _rc )
			{
				if( m_find == _rc->camera )
				{
					return true;
				}
				return false;
			}
		private:
			Camera* m_find;
		};

		class RemoveEmptyCamera
		{
		public:
			bool operator()( RenderCamera* _rc )
			{
				return  ( _rc->blendObjects.empty() && _rc->solidObjects.empty() );
			}
		};

		class NotEmptyCamera
		{
		public:
			bool operator()( RenderCamera* _rc )
			{
				return  !( _rc->blendObjects.empty() && _rc->solidObjects.empty() );
			}
		};

		class TextureSortPredicate
		{
		public:
			bool operator()( RenderObject* const& _obj1, RenderObject* const& _obj2 )
			{
				return _obj1->material->textureStage[0].texture > _obj2->material->textureStage[0].texture;
			}
		};

	};
}
