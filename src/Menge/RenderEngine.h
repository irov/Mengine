#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Viewport.h"

#	include "Holder.h"

#	include "math/mat4.h"
#	include "math/vec4.h"

#	include <string>
#	include <vector>

namespace Menge
{
	class FileEngine;
	class Camera3D;

	class RenderEngine
		: public RenderSystemListener
	{
	public:
		RenderEngine( ::RenderSystemInterface * _interface );
		
	public:

		bool initialize( const String& _driver );
		bool createRenderWindow( int _width, int _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle = 0,
								int _FSAAType = 0, int _FSAAQuality = 0 );

		void addResourceLocation( const std::string& _path );
		void initResources();

		void screenshot( RenderImageInterface* _renderTargetImage, const int* rect = NULL );
		void render();

		void setContentResolution( const mt::vec2f _resolution );
		mt::vec2f getBestDisplayResolution( int _defWidth, int _defHeigth, float _aspect );

		void setRenderViewport( const Viewport & _viewport );
		const Viewport & getRenderViewport() const;

		void setRenderCamera( Camera3D * _camera );
		Camera3D * getRenderCamera();

		RenderImageInterface * createImage( const char* _name, unsigned int _width, unsigned int _height );
		RenderImageInterface * createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height );
		RenderImageInterface * loadImage( const TextureDesc & _desc );
		RenderImageInterface * loadImage( const std::string & _filename, unsigned int _filter );
		RenderVideoStreamInterface * loadImageVideoStream( const std::string& _filename );


		void renderImage(		
			const mt::mat3f & _transform, 
			const mt::vec2f & _a,
			const mt::vec2f & _b,
			const mt::vec2f & _c,
			const mt::vec2f & _d,
			const mt::vec4f & _uv,
			unsigned int _color, 
			const RenderImageInterface* _image,
			EBlendFactor _src = BF_SOURCE_ALPHA,
			EBlendFactor _dst = BF_ONE_MINUS_SOURCE_ALPHA);

		void renderTriple(		
			const mt::mat3f & _transform, 
			const mt::vec2f & _a,
			const mt::vec2f & _b,
			const mt::vec2f & _c,
			const mt::vec2f & _uv0,
			const mt::vec2f & _uv1,
			const mt::vec2f & _uv2,
			unsigned int _color, 
			const RenderImageInterface* _image,
			EBlendFactor _src = BF_SOURCE_ALPHA,
			EBlendFactor _dst = BF_ONE_MINUS_SOURCE_ALPHA);

		void renderMesh(
			const std::vector<TVertex>& _vertexData,
			const std::vector<uint16>& _indexData, 
			TMaterial* _material );

		void renderLine(
			unsigned int _color,
			const mt::vec2f & _begin,
			const mt::vec2f & _end);

		void	releaseImage( RenderImageInterface * _image );
		void	releaseImageVideoStream( RenderVideoStreamInterface* _image );

		void	setProjectionMatrix( const mt::mat4f& _projection );
		void	setViewMatrix( const mt::mat4f& _view );
		void	setWorldMatrix( const mt::mat4f& _world );

		void	beginScene();
		void	endScene();
		void	beginLayer2D();
		void	endLayer2D();
		void	beginLayer3D();
		void	endLayer3D();

		void	setRenderArea( const mt::vec4f& _renderArea );

		void	setFullscreenMode( bool _fullscreen );
		bool	getFullscreenMode();
		void	setViewportDimensions( float _width, float _height, float _renderFactor = 0.0f );

		CameraInterface * createCamera( const std::string& _name );
		EntityInterface * createEntity( const std::string& _name, const std::string& _meshName );
		LightInterface * createLight( const std::string& _name );
		SceneNodeInterface * createSceneNode( const std::string & _name );

		void releaseCamera( CameraInterface * _camera );
		void releaseEntity( EntityInterface * _entity );
		void releaseLight( LightInterface * _light );
		void releaseSceneNode( SceneNodeInterface * _interface );

		SceneNodeInterface * getRootSceneNode() const;

		void frameStarted();
		void frameEnded();

		void setTextureFiltering( bool _filter );
		void onDeviceRestored() override;

		void onWindowMovedOrResized();
		void onWindowActive( bool _active );
		void onWindowClose();

		void setRenderFactor( float _factor );
		void setRenderTarget( const String& _target );
		const mt::vec4f& getRenderArea() const;
		const mt::mat3f& getRenderTransform() const;

	protected:
		::RenderSystemInterface * m_interface;
		Viewport m_renderViewport;
		Camera3D * m_renderCamera;
		bool m_windowCreated;
		bool m_fullscreen;
		float m_viewportWidth;
		float m_viewportHeight;
		float m_renderFactor;
		mt::vec2f m_contentResolution;
		mt::vec4f m_renderArea;
		mt::mat3f m_renderTransform;
		mt::mat4f m_renderTransform4;
		float m_rendFactPix;
		int m_windowWidth;
		int m_windowHeight;
		String m_currentRenderTarget;
	};
}
