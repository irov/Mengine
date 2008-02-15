#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( RenderSystemInterface * _interface )
		: m_interface( _interface )
		, m_renderViewport( mt::vec2f(0.f,0.f), mt::vec2f(1024.f,768.f) )
		, m_renderCamera(0)
	{
		Holder<RenderEngine>::keep( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render( RenderImageInterface* _image, const int* rect )
	{
		m_interface->render( _image, rect );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render()
	{
		m_interface->render();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setContentResolution( const mt::vec2f _resolution )
	{
		m_interface->setContentResolution( _resolution.m );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderViewport( const Viewport & _viewport )
	{
		m_renderViewport = _viewport;
	}
	////////////////////////////////////////////////////////////////////////////
	const Viewport & RenderEngine::getRenderViewport() const
	{
		return m_renderViewport;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderCamera( Camera3D * _camera )
	{
		m_renderCamera = _camera;
	}
	////////////////////////////////////////////////////////////////////////////
	Camera3D * RenderEngine::getRenderCamera()
	{
		return m_renderCamera;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer()
	{
		m_interface->beginLayer();
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::endLayer()
	{
		m_interface->endLayer();
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setProjectionMatrix( const mt::mat4f& _projection )
	{
		return m_interface->setProjectionMatrix( _projection.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setViewMatrix( const mt::mat4f& _view )
	{
		return m_interface->setViewMatrix( _view.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setWorldMatrix( const mt::mat4f& _world )
	{
		return m_interface->setWorldMatrix( _world.m );
	}	
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::createImage( const char* _name, unsigned int _width, unsigned int _height )
	{
		RenderImageInterface * image = m_interface->createImage( _name, _width, _height );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height )
	{
		RenderImageInterface * image = m_interface->createRenderTargetImage( _name, _width, _height );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderEngine::loadImage( const TextureDesc & _desc )
	{
		RenderImageInterface * image = m_interface->loadImage( _desc );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVideoStreamInterface* RenderEngine::loadImageVideoStream( const std::string& _filename )
	{
		RenderVideoStreamInterface * image = m_interface->loadImageVideoStream( _filename.c_str() );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::loadImage( const std::string & _filename, size_t _filter )
	{
		FileDataInterface * file = Holder<FileEngine>::hostage()->openFile( _filename );

		if( file == 0 )
		{
			MENGE_LOG( "Error: Image can't open resource file '%s'\n", _filename.c_str() );
			return 0;
		}

		static std::vector<char> s_buff;

		size_t buff_size = file->size();
		s_buff.resize( buff_size );
		file->read( &s_buff[0], buff_size );

		TextureDesc	textureDesc;
		textureDesc.buffer = &s_buff[0];
		textureDesc.size = buff_size;
		textureDesc.name = _filename.c_str();
		textureDesc.filter = _filter;

		RenderImageInterface * image = loadImage( textureDesc );

		Holder<FileEngine>::hostage()->closeFile( file );

		if( image == 0 )
		{
			MENGE_LOG( "Error: Image from file '%s' not loader\n", _filename.c_str() );

			return 0;
		}	

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImage(
		const mt::mat3f & _transform, 
		const mt::vec2f & _offset,
		const mt::vec4f & _uv,
		const mt::vec2f & _size,
		unsigned int _color, 
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst)
	{
		mt::mat3f transform = _transform;
		transform.v2.v2 -= m_renderViewport.begin;
		m_interface->renderImage(
			transform.m,
			_offset.m,
			_uv.m,
			_size.m,
			_color,
			_image,
			_src,
			_dst);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImage(		
			const mt::mat3f & _transform, 
			const mt::vec2f & _a,
			const mt::vec2f & _b,
			const mt::vec2f & _c,
			const mt::vec2f & _d,
			const mt::vec4f & _uv,
			unsigned int _color, 
			const RenderImageInterface* _image,
			EBlendFactor _src,
			EBlendFactor _dst)
	{
		mt::mat3f transform = _transform;
		transform.v2.v2 -= m_renderViewport.begin;
		m_interface->renderImage(
			transform.m,
			_a.m,
			_b.m,
			_c.m,
			_d.m,
			_uv.m,
			_color,
			_image,
			_src,
			_dst);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseImage( RenderImageInterface * _image )
	{
		m_interface->releaseImage( _image );
	}
	void RenderEngine::releaseImageVideoStream( RenderVideoStreamInterface* _image )
	{
		m_interface->releaseImageVideoStream( _image );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::setFullscreenMode( bool _fullscreen )
	{
		m_interface->setFullscreenMode( _fullscreen );
	}
	////////////////////////////////////////////////////////////////////////////
	bool RenderEngine::getFullscreenMode( )
	{
		return m_interface->getFullscreenMode();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setViewportDimensions( float _width, float _height, float _renderFactor )
	{
		m_interface->setViewportDimensions( _width, _height, _renderFactor );
	}
	//////////////////////////////////////////////////////////////////////////
	CameraInterface * RenderEngine::createCamera( const std::string& _name )
	{
		return m_interface->createCamera(_name.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	EntityInterface * RenderEngine::createEntity( const std::string& _name, const std::string& _meshName )
	{
		return m_interface->createEntity(_name.c_str(), _meshName.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * RenderEngine::createLight( const std::string& _name )
	{
		return m_interface->createLight(_name.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseCamera( CameraInterface * _camera )
	{
		return m_interface->releaseCamera( _camera );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseEntity( EntityInterface * _entity )
	{
		return m_interface->releaseEntity( _entity );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseLight( LightInterface * _light )
	{
		return m_interface->releaseLight( _light );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::frameStarted()
	{
		m_interface->render();
	}
	//////////////////////////////////////////////////////////////////////////
	SceneNodeInterface * RenderEngine::createSceneNode( const std::string & _name )
	{
		return m_interface->createSceneNode( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseSceneNode( SceneNodeInterface * _interface )
	{
		return m_interface->releaseSceneNode( _interface );
	}
}
