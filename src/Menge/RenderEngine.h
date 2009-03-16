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

#	include "RenderObject.h"
//#	include "ColourValue.h"

namespace Menge
{
	class RenderObject;
	struct RenderPass;

	class Camera;

	class Texture;

	struct RenderCamera
	{
		Camera* camera;
		std::vector<RenderObject*> solidObjects;
		std::vector<RenderObject*> blendObjects;
	};

	class RenderEngine
		: public RenderSystemListener
	{
	public:
		RenderEngine( RenderSystemInterface * _interface );
		~RenderEngine();

	public:

		bool initialize();
		bool createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
								int _FSAAType , int _FSAAQuality );

		void screenshot( Texture* _renderTargetImage, const mt::vec4f & _rect );
		void render();

		void setContentResolution( const Resolution & _resolution );
		Resolution getBestDisplayResolution( const Resolution & _resolution, float _aspect );

		RenderObject* createRenderObject();
		void releaseRenderObject( RenderObject* _renderObject );
		void renderObject( RenderObject* _renderObject );

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

		void setTextureFiltering( bool _filter );
		void onDeviceRestored();

		void onWindowMovedOrResized();
		void onWindowActive( bool _active );
		void onWindowClose();

		void setRenderFactor( float _factor );
		void setRenderTarget( const String & _target, bool _clear = true );
		const mt::vec4f& getRenderArea() const;

		std::size_t getNumDIP() const;

		const mt::mat4f& getViewTransform() const;

		bool isWindowCreated() const;

		void setActiveCamera( Camera* _camera );
		Camera* getActiveCamera() const;

		void setProjectionMatrix2D_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf );

	protected:
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

		void recalcRenderArea_( const Resolution & resolution );

		VBHandle m_vbHandle2D;
		IBHandle m_ibHandle2D;

		VBHandle m_vbHandle3D;
		IBHandle m_ibHandle3D;

		VBHandle m_currentVBHandle;
		VBHandle m_currentIBHandle;

		std::vector<RenderObject*> m_renderObjects;
		RenderObject* m_batchedObject;
		float m_layerZ;

		//RenderImageInterface* m_currentTexture;
		std::size_t m_currentTextureStages;
		TextureStage m_currentTextureStage[MENGE_MAX_TEXTURE_STAGES];
		EBlendFactor m_currentBlendSrc;
		EBlendFactor m_currentBlendDst;
		//ColourValue m_currentColor;

		bool checkForBatch_( RenderObject* _prev, RenderObject* _next );

		std::size_t m_dipCount;

		void renderPass_( RenderPass* _pass, std::size_t _vertexIndex, std::size_t _verticesNum );
		void enableTextureStage_( std::size_t _stage, bool _enable );
		void prepareBuffers_();
		void fillBuffers_( std::vector<RenderObject*>& _objects,
			std::size_t& _vbPos2D, std::size_t& _ibPos2D
			,std::size_t& _vbPos3D, std::size_t& _ibPos3D 
			, TVertex* _vertexBuffer2D, uint16* _indexBuffer2D 
			, TVertex* _vertexBuffer3D, uint16* _indexBuffer3D );

		void orthoOffCenterLHMatrix_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf );
		void setRenderSystemDefaults_();

		//std::vector<RenderObject*>* m_currentCameraObjects;
		std::vector<RenderCamera> m_cameras;
		RenderCamera* m_activeCamera;
		//Camera* m_activeCamera;

		typedef std::map< String, Texture* > TTextureMap;
		TTextureMap m_textures;
		TTextureMap m_renderTargets;

		Texture* m_nullTexture;	// white pixel

		mt::vec4f m_currentRenderArea;

	private:
		class FindCamera
		{
		public:
			FindCamera( Camera* _find )
				: m_find( _find )
			{
			}
			bool operator()( const RenderCamera& _rc )
			{
				if( m_find == _rc.camera )
				{
					return true;
				}
				return false;
			}
		private:
			Camera* m_find;
		};

		//class RemoveEmptyCamera
		//{
		//public:
		//	bool operator()( const RenderCamera& _rc )
		//	{
		//		return !( _rc.blendObjects.empty() || _rc.solidObjects.empty() )
		//	}
		//}

	};
}
