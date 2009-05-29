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
		LPT_MESH_40_30,

		LPT_PRIMITIVE_COUNT,
		LPT_FORCE_DWORD = 0x7FFFFFFF
	};

	struct RenderObject
	{
		Material* material;
		ELogicPrimitiveType logicPrimitiveType;
		unsigned char* vertexData;
		size_t verticesNum;
		size_t minIndex;
		size_t startIndex;
		size_t dipIndiciesNum;
		size_t dipVerticesNum;
		EPrimitiveType primitiveType;
	};

	typedef std::vector<RenderObject*> TVectorRenderObject;

	struct RenderCamera
	{
		Camera* camera;
		TVectorRenderObject solidObjects;
		TVectorRenderObject blendObjects;

		RenderCamera::RenderCamera()
			: camera( NULL )
		{
		}
	};

	template<class T>
	class RenderPool
	{
		typedef std::vector<T*> TVectorPool;

	public:
		RenderPool( bool _placement = true )
			: m_placement(_placement)
		{
		}

		~RenderPool()
		{
			for( typename TVectorPool::iterator 
				it = m_pool.begin(), 
				it_end = m_pool.end();
				it != it_end;
				++it )
			{
				delete (*it);
			}
		}

	public:
		T * get()
		{
			if( m_pool.empty() == false )
			{
				T * element = m_pool.back();
				m_pool.pop_back();
				return element;
			}

			return new T();
		}

		void release( T * _element )
		{
			if( _element == 0 )
			{
				return;
			}

			if( m_placement ) 
			{
				_element->~T();
				new (_element) T();
			}

			m_pool.push_back( _element );
		}

	protected:
		TVectorPool m_pool;
		bool m_placement;
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

		size_t batch_( TVectorRenderObject & _objects, size_t _startVertexPos, bool textureSort );
		bool checkForBatch_( RenderObject* _prev, RenderObject* _next );
		void renderPass_( RenderObject* _renderObject );
		void enableTextureStage_( std::size_t _stage, bool _enable );

		void orthoOffCenterLHMatrix_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf );
		void setRenderSystemDefaults_();
		void render_();
		void makeBatches_();
		size_t insertRenderObjects_( unsigned char* _vertexBuffer, size_t _offset, TVectorRenderObject& _renderObjects );
		void flushRender_();
		void prepare2D_();
		void prepare3D_();
		void releaseRenderCamera_( RenderCamera* _renderCamera );
		size_t refillIndexBuffer2D_();

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
		size_t m_maxVertices2D;

		VBHandle m_vbHandle3D;
		IBHandle m_ibHandle3D;

		VBHandle m_currentVBHandle;
		VBHandle m_currentIBHandle;

		RenderObject* m_batchedObject;
		float m_layerZ;

		std::size_t m_currentTextureStages;
		TextureStage m_currentTextureStage[MENGE_MAX_TEXTURE_STAGES];
		EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;

		typedef std::vector<RenderCamera*> TVectorRenderCamera;
		TVectorRenderCamera m_cameras;
		RenderCamera* m_activeCamera;

		typedef std::map< String, Texture* > TTextureMap;
		TTextureMap m_textures;
		TTextureMap m_renderTargets;

		Texture* m_nullTexture;	// white pixel

		mt::vec4f m_currentRenderArea;

		DebugInfo m_debugInfo;	// debug info

		typedef RenderPool<RenderCamera> TPoolRenderCamera;
		TPoolRenderCamera m_renderCameraPool;

		typedef RenderPool<RenderObject> TPoolRenderObject;
		TPoolRenderObject m_renderObjectPool;

		typedef RenderPool<Material> TPoolRenderMaterial;
		TPoolRenderMaterial m_renderMaterialPool;

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
