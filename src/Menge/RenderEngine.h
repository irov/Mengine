#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Viewport.h"

#	include "Resolution.h"

#	include "Holder.h"

#	include "math/mat4.h"
#	include "math/vec4.h"
#	include "math/polygon.h"

#	include <vector>

namespace Menge
{
	class RenderEngine
		: public RenderSystemListener
	{
	public:
		RenderEngine( RenderSystemInterface * _interface );
		
	public:

		bool initialize();
		bool createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
								int _FSAAType , int _FSAAQuality );

		void screenshot( RenderImageInterface* _renderTargetImage, const mt::vec4f & _rect );
		void render();

		void setContentResolution( const Resolution & _resolution );
		Resolution getBestDisplayResolution( const Resolution & _resolution, float _aspect );

		RenderImageInterface * createImage( const String & _name, float _width, float _height, PixelFormat _format );
		RenderImageInterface * createRenderTargetImage( const String & _name, const mt::vec2f & _resolution );
		RenderImageInterface * loadImage( const String & _filename, unsigned int _filter );
		bool saveImage( RenderImageInterface* _image, const String& _filename );


		void renderImage(		
			const mt::vec2f * _vertices,
			const mt::vec4f & _uv,
			unsigned int _color, 
			const RenderImageInterface* _image,
			EBlendFactor _src = BF_SOURCE_ALPHA,
			EBlendFactor _dst = BF_ONE_MINUS_SOURCE_ALPHA);

		void renderTriple(		
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

		void renderRect(	
			unsigned int _color,
			const mt::vec2f & _begin,
			const mt::vec2f & _end);

		void renderPoly( 
			unsigned int _color,
			const mt::polygon & poly,
			const mt::mat3f& mtx );

		void	releaseImage( RenderImageInterface * _image );

		void	setProjectionMatrix( const mt::mat4f& _projection );
		void	setViewMatrix( const mt::mat4f& _view );
		void	setWorldMatrix( const mt::mat4f& _world );

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

		CameraInterface * createCamera( const String & _name );
		EntityInterface * createEntity( const String & _name, const String & _meshName );
		LightInterface * createLight( const String & _name );

		void releaseCamera( CameraInterface * _camera );
		void releaseEntity( EntityInterface * _entity );
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

		int getNumDIP() const;

		const mt::mat4f& getViewTransform() const;

		void renderText(const Menge::String & _text, const mt::vec2f & _pos, unsigned long _color);

		bool isWindowCreated() const;

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
		mt::mat4f m_viewTransform;

		void recalcRenderArea_( const Resolution & resolution );
	};
}
