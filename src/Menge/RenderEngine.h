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
	//class Camera3D;

	class RenderEngine
		: public RenderSystemListener
	{
	public:
		RenderEngine( RenderSystemInterface * _interface );
		
	public:

		bool initialize();
		bool createRenderWindow( std::size_t _resolution[2], int _bits, bool _fullscreen, WindowHandle _winHandle,
								int _FSAAType , int _FSAAQuality );

		void screenshot( RenderImageInterface* _renderTargetImage, const mt::vec4f & _rect );
		void render();

		void setContentResolution( const std::size_t  _resolution [2] );
		void getBestDisplayResolution( std::size_t _bestResolution[2], const std::size_t  _resolution [2], float _aspect );

		RenderImageInterface * createImage( const String & _name, float _width, float _height );
		RenderImageInterface * createRenderTargetImage( const String & _name, const mt::vec2f & _resolution );
		RenderImageInterface * loadImage( const TextureDesc & _desc );
		RenderImageInterface * loadImage( const String & _filename, unsigned int _filter );
		RenderVideoStreamInterface * loadImageVideoStream( const String& _filename );


		void renderImage(		
			const mt::vec2f * _vertices,
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
		void	setViewportDimensions( std::size_t _resolution[2], float _renderFactor = 0.0f );

		CameraInterface * createCamera( const String & _name );
		EntityInterface * createEntity( const String & _name, const String & _meshName );
		LightInterface * createLight( const String & _name );
		SceneNodeInterface * createSceneNode( const String & _name );

		void releaseCamera( CameraInterface * _camera );
		void releaseEntity( EntityInterface * _entity );
		void releaseLight( LightInterface * _light );
		void releaseSceneNode( SceneNodeInterface * _interface );

		SceneNodeInterface * getRootSceneNode() const;

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

		int getNumDIP() const;

		const mt::mat4f& getViewTransform() const;

	protected:
		Menge::RenderSystemInterface * m_interface;
		Viewport m_renderViewport;
		bool m_windowCreated;
		bool m_fullscreen;
		float m_viewportWidth;
		float m_viewportHeight;
		float m_renderFactor;
		
		std::size_t m_contentResolution[2];
		std::size_t m_windowResolution[2];

		mt::vec4f m_renderArea;
		float m_rendFactPix;
		
		String m_currentRenderTarget;

		bool m_layer3D;
		mt::mat4f m_viewTransform;
		void recalcRenderArea_( const std::size_t resolution [2] );
	private:
		struct Line
		{
			unsigned int color;
			mt::vec2f begin;
			mt::vec2f end;
		};

		std::list<Line> lines;
	};
}
