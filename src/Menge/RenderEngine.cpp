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
	bool RenderEngine::createRenderWindow( int _width, int _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle /* = 0  */,
											int _FSAAType, int _FSAAQuality )
	{
		m_fullscreen = _fullscreen;
		m_viewportWidth = _width;
		m_viewportHeight = _height;
		m_windowCreated = m_interface->createRenderWindow( _width, _height, _bits, _fullscreen, _winHandle,
															_FSAAType, _FSAAQuality );


		m_renderArea.x = 0.0f;
		m_renderArea.y = 0.0f;
		m_renderArea.z = _width;
		m_renderArea.w = _height;

		float contentAspect = m_contentResolution.x / m_contentResolution.y;
		float dw = m_contentResolution.x / _width;
		float dh = m_contentResolution.y / _height;
		float aspect = static_cast<float>( _width ) / _height;

		if( dw > 1.0f )
		{
			dw = 1.0f;
			dh = _width / contentAspect / _height;
		}

		if( dh > 1.0f )
		{
			dh = 1.0f;
			dw = _height * contentAspect / _width;
		}

		//dw += ( 1.0f - dw ) * 0.8f;
		//dh += ( 1.0f - dh ) * 0.8f;

		float areaWidth = dw * _width;
		float areaHeight = dh * _height;

		m_renderArea.x = ( _width - areaWidth ) * 0.5f;
		m_renderArea.y = ( _height - areaHeight ) * 0.5f;
		m_renderArea.z = m_renderArea.x + areaWidth;
		m_renderArea.w = m_renderArea.y + areaHeight;

		m_renderFactor = m_renderArea.x / aspect / m_renderArea.y;

		areaHeight += m_renderFactor * m_renderArea.y * 2;
		areaWidth = areaHeight * contentAspect;
		if( areaWidth > _width )
		{
			areaWidth = _width;
		}

		m_renderArea.x = ( _width - areaWidth ) * 0.5f;
		m_renderArea.y = ( _height - areaHeight ) * 0.5f;
		m_renderArea.z = m_renderArea.x + areaWidth;
		m_renderArea.w = m_renderArea.y + areaHeight;
	
		mt::ident_m3( m_renderTransform );
		m_renderTransform[2][0] = m_renderArea.x;
		m_renderTransform[2][1] = m_renderArea.y;
		m_renderTransform[0][0] = areaWidth / m_contentResolution.x;
		m_renderTransform[1][1] = areaHeight / m_contentResolution.y;

		mt::ident_m4( m_renderTransform4 );
		//m_renderTransform4[3][0] = m_renderArea.x;
		//m_renderTransform4[3][1] = m_renderArea.y;
		m_renderTransform4[0][0] = areaWidth / _width;
		m_renderTransform4[1][1] = areaHeight / _height;

		m_overlays[0] = mt::vec2f( 0.0f, 0.0f );
		m_overlays[1] = mt::vec2f( _width, 0.0f );
		m_overlays[2] = mt::vec2f( _width, m_renderArea.y );
		m_overlays[3] = mt::vec2f( 0.0f, m_renderArea.y );

		m_overlays[4] = mt::vec2f( 0.0f, m_renderArea.y );
		m_overlays[5] = mt::vec2f( m_renderArea.x, m_renderArea.y );
		m_overlays[6] = mt::vec2f( m_renderArea.x, m_renderArea.y + areaHeight );
		m_overlays[7] = mt::vec2f( 0.0f, m_renderArea.y + areaHeight );

		m_overlays[8] = mt::vec2f( m_renderArea.x + areaWidth, m_renderArea.y );
		m_overlays[9] = mt::vec2f( _width, m_renderArea.y );
		m_overlays[10] = mt::vec2f( _width, m_renderArea.y + areaHeight );
		m_overlays[11] = mt::vec2f( m_renderArea.x + areaWidth, m_renderArea.y + areaHeight );

		m_overlays[12] = mt::vec2f( 0.0f, m_renderArea.y + areaHeight );
		m_overlays[13] = mt::vec2f( _width, m_renderArea.y + areaHeight);
		m_overlays[14] = mt::vec2f( _width, _height );
		m_overlays[15] = mt::vec2f( 0.0f, _height );

		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( RenderImageInterface* _image, const int* rect )
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
		//m_interface->setContentResolution( _resolution.m );
		m_contentResolution = _resolution;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderViewport( const Viewport & _viewport )
	{
		m_renderViewport = _viewport;
		m_renderViewport.begin.x = ::floorf( m_renderViewport.begin.x + 0.5f );
		m_renderViewport.begin.y = ::floorf( m_renderViewport.begin.y + 0.5f );
		m_renderViewport.end.x = ::floorf( m_renderViewport.end.x + 0.5f );
		m_renderViewport.end.y = ::floorf( m_renderViewport.end.y + 0.5f );
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
		mt::mat4f mat;
		mt::mul_m4_m4( mat, _view, m_renderTransform4 );
		return m_interface->setViewMatrix( mat.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setWorldMatrix( const mt::mat4f& _world )
	{
		//mt::mat4f mat;
		//mt::mul_m4_m4( mat, _world, m_renderTransform4 );
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
	RenderImageInterface * RenderEngine::loadImage( const std::string & _filename, unsigned int _filter )
	{
		RenderImageInterface * image = m_interface->getImage(_filename.c_str());

		if(image == 0)
		{

			Image cimage;
			cimage.load( _filename );

			TextureDesc	textureDesc;
			textureDesc.buffer = cimage.getData();
			textureDesc.size = cimage.getSize();
			textureDesc.width = cimage.getWidth();
			textureDesc.height = cimage.getHeight();
			textureDesc.pixelFormat = cimage.getFormat();
			textureDesc.name = _filename.c_str();
			textureDesc.filter = _filter;

			if( textureDesc.buffer == 0 )
			{
				MENGE_LOG( "Error: Image from file '%s' not loader\n", _filename.c_str() );
				return 0;
			}	

			image = loadImage( textureDesc );

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
		transform.v2.v2 += -m_renderViewport.begin;
		mt::mat3f tr = transform;
		if( m_renderViewport.m_camera == "defaultCamera" )
		{
			mt::mul_m3_m3( tr, transform, m_renderTransform );
		}
		m_interface->renderImage(
			tr.m,
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
		mt::vec2f begin = _begin - m_renderViewport.begin;
		mt::vec2f end = _end - m_renderViewport.begin;
		m_interface->renderLine( _color, begin.m, end.m );		
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
	mt::vec2f RenderEngine::getBestDisplayResolution( int _defWidth, int _defHeigth, float _aspect )
	{
		int * rl;
		unsigned int count = m_interface->getResolutionList( &rl );

		float needWidth = _defHeigth * _aspect;

		int bestWidth = _defWidth;
		int bestHeight = _defHeigth;

		typedef std::vector<int> TResolutionVector;
		typedef std::map< int, TResolutionVector > TResolutionMap;
		TResolutionMap resMap;
		for( unsigned int i = 0; i < count / 2; i++ )
		{
			if( fabsf( ( static_cast<float>( rl[2*i] ) / rl[2*i+1] ) - _aspect ) < 0.01f )
			{
				resMap[ rl[2*i + 1] ].push_back( rl[2*i] );
			}
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
		m_interface->setRenderArea( m_renderArea.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endScene()
	{
		m_interface->beginLayer2D();
		mt::vec4f v_zero = mt::vec4f::zero_v4;
		m_interface->setRenderArea( v_zero.m );
		for( int i = 0; i < 4; i++ )
		{
			mt::mat3f mt;
			mt::ident_m3( mt );
			mt::vec4f uv( 0.0f, 0.0f, 1.0f, 1.0f );
			m_interface->renderImage( mt.m, m_overlays[i*4].m,
										m_overlays[i*4+1].m,
										m_overlays[i*4+2].m,
										m_overlays[i*4+3].m,
										uv.m,
										0xFF000000,
										0,
										BF_ONE,
										BF_ZERO );
		}
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
	void RenderEngine::setRenderArea( const mt::vec4f& _renderArea )
	{
		mt::vec4f renderArea = m_renderArea + _renderArea;
		m_interface->setRenderArea( renderArea.m );
	}
	//////////////////////////////////////////////////////////////////////////
}
