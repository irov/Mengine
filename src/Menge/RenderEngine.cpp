#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include <map>

#	include "Application.h"
#	include "Game.h"

#	include "Image.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( ::RenderSystemInterface * _interface )
		: m_interface( _interface )
		, m_renderViewport( mt::vec2f(0.f,0.f), mt::vec2f(1024.f,768.f) )
		, m_renderCamera(0)
		, m_windowCreated( false )
		, m_renderFactor( 1.0f )
		, m_viewportWidth(1024.f)
		, m_viewportHeight(768.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize( const String& _driver )
	{
		bool result = m_interface->initialize( Holder<LogEngine>::hostage()->getInterface() );
		m_interface->setEventListener( this );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( float _width, float _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle /* = 0  */,
											int _FSAAType, int _FSAAQuality )
	{
		m_fullscreen = _fullscreen;
		m_viewportWidth = _width;
		m_viewportHeight = _height;
		m_windowCreated = m_interface->createRenderWindow( _width, _height, _bits, _fullscreen, _winHandle,
															_FSAAType, _FSAAQuality );
		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render( RenderImageInterface* _image, const int* rect )
	{
		m_interface->screenshot( _image, rect );
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
		m_interface->setRenderTarget( m_renderViewport.getCamera() );
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
	void RenderEngine::beginLayer2D()
	{
		m_interface->beginLayer2D();
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::endLayer2D()
	{
		m_interface->endLayer2D();
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
	RenderImageInterface * RenderEngine::createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height, const char* _camera  )
	{
		RenderImageInterface * image = m_interface->createRenderTargetImage( _name, _width, _height, _camera );
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
	RenderImageInterface * RenderEngine::loadImage( const std::string & _filename, unsigned int _filter )
	{
		RenderImageInterface * image = m_interface->getImage(_filename.c_str());

		if(image == 0)
		{
			/*static std::vector<char> s_buff;

			unsigned int buff_size = file->size();
			s_buff.resize( buff_size );
			file->read( &s_buff[0], buff_size );*/
			/*Image cimage;
			cimage.load( _filename );

			TextureDesc	textureDesc;
			textureDesc.buffer = cimage.getData();
			textureDesc.size = cimage.getSize();
			textureDesc.width = cimage.getWidth();
			textureDesc.height = cimage.getHeight();
			textureDesc.pixelFormat = cimage.getFormat();
			//textureDesc.buffer = &s_buff[0];
			//textureDesc.size = buff_size;
			textureDesc.name = _filename.c_str();
			textureDesc.filter = _filter;*/

			DataStreamInterface* file = Holder<FileEngine>::hostage()->openFile( _filename );
			TextureDesc textureDesc;

			if( file )
			{
				textureDesc.buffer = file->getBuffer();
				textureDesc.name = _filename.c_str();
				textureDesc.size = file->size();
			}
			if( !file || textureDesc.buffer == 0 )
			{
				MENGE_LOG( "Error: Image from file '%s' not loader\n", _filename.c_str() );
				return 0;
			}	


			image = loadImage( textureDesc );

			Holder<FileEngine>::hostage()->closeStream( file );

			if( image == 0 )
			{
				MENGE_LOG( "Error: Image from file '%s' not loader\n", _filename.c_str() );
				return 0;
			}	
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
			m_renderViewport.getCamera().c_str(),
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
			m_renderViewport.getCamera().c_str(),
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
	void RenderEngine::renderLine(	
		unsigned int _color,
		const mt::vec2f & _begin,
		const mt::vec2f & _end)
	{
		m_interface->renderLine( m_renderViewport.getCamera().c_str(), _color,_begin.m,_end.m );		
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseImage( RenderImageInterface * _image )
	{
		m_interface->releaseImage( _image );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseImageVideoStream( RenderVideoStreamInterface* _image )
	{
		m_interface->releaseImageVideoStream( _image );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::setFullscreenMode( bool _fullscreen )
	{
		if( m_fullscreen == _fullscreen ) return;

		m_fullscreen = _fullscreen;
		float width = Holder<Game>::hostage()->getWidth();
		float height = Holder<Game>::hostage()->getHeight();
		m_interface->setFullscreenMode( width, height, _fullscreen );

		Holder<Application>::hostage()->notifyWindowModeChanged( width, height, m_fullscreen );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setViewportDimensions( float _width, float _height, float _renderFactor )
	{
		m_viewportWidth = _width;
		m_viewportHeight = _height;
		m_renderFactor = _renderFactor;
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
	SceneNodeInterface * RenderEngine::getRootSceneNode() const
	{
		return m_interface->getRootSceneNode();
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
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f RenderEngine::getBestDisplayResolution( std::size_t _defWidth, std::size_t _defHeigth, float _aspect )
	{
		int * rl;
		unsigned int count = m_interface->getResolutionList( &rl );

		float needWidth = _defHeigth * _aspect;

		std::size_t bestWidth = _defWidth;
		std::size_t bestHeight = _defHeigth;

		typedef std::vector<int> TResolutionVector;
		typedef std::map< int, TResolutionVector > TResolutionMap;
		TResolutionMap resMap;
		for( unsigned int i = 0; i < count / 2; i++ )
		{
			resMap[ rl[2*i + 1] ].push_back( rl[2*i] );
		}
		bool done = false;
		for( TResolutionMap::iterator 
			it = resMap.begin(),
			it_end = resMap.end();
		it != it_end; 
		++it )
		{
			if( it->first < _defHeigth ) 
			{
				continue;
			}

			for( TResolutionVector::size_type 
				i = 0,
				i_end = it->second.size();
			i != i_end; 
			++i)
			{
				if( it->second[i] >= needWidth )
				{
					bestWidth = it->second[i];
					bestHeight = it->first;
					done = true;
					break;
				}
			}
			if( done ) break;
		}
		return mt::vec2f( bestWidth, bestHeight );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::getFullscreenMode()
	{
		return m_fullscreen;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowMovedOrResized()
	{
		if( m_windowCreated )
		{
			m_interface->setViewportDimensions( m_viewportWidth, m_viewportHeight, m_renderFactor );
			m_interface->onWindowMovedOrResized();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onDeviceRestored()
	{
		float width = Holder<Game>::hostage()->getWidth();
		float height = Holder<Game>::hostage()->getHeight();
		Holder<Application>::hostage()->notifyWindowModeChanged( width, height, m_fullscreen );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowActive( bool _active )
	{
		if( m_windowCreated )
		{
			m_interface->onWindowActive( _active );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowClose()
	{
		if( m_windowCreated )
		{
			m_interface->onWindowClose();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setTextureFiltering( bool _filter )
	{
		m_interface->setTextureFiltering( _filter );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addResourceLocation( const std::string& _path )
	{
		m_interface->addResourceLocation( _path.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::initResources()
	{
		m_interface->initResources();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginScene()
	{
		m_interface->beginScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endScene()
	{
		m_interface->endScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderMesh( const std::vector<TVertex>& _vertexData, 
									const std::vector<uint16>& _indexData,
									TMaterial* _material )
	{
		m_interface->renderMesh( &(_vertexData[0]), _vertexData.size(), 
								&(_indexData[0]), _indexData.size(),
								_material );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer3D()
	{
		m_interface->beginLayer3D();
	}
	//////////////////////////////////////////////////////////////////////////
}
