#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include <map>

#	include "Application.h"
#	include "Game.h"

#	include "Image.h"
#	include "Player.h"
#	include "Arrow.h"
#	include "Math/box2.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( ::RenderSystemInterface * _interface )
		: m_interface( _interface )
		, m_renderViewport( mt::vec2f(0.f,0.f), mt::vec2f(1024.f,768.f) )
		, m_renderCamera(0)
		, m_windowCreated( false )
		, m_renderFactor( 1.0f )
		//, m_viewportWidth(1024.f)
		//, m_viewportHeight(768.f)
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
		m_windowWidth = _width;
		m_windowHeight = _height;
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

		float areaWidth = dw * _width;
		float areaHeight = dh * _height;

		m_renderArea.x = ( _width - areaWidth ) * 0.5f;
		m_renderArea.y = ( _height - areaHeight ) * 0.5f;
		m_renderArea.z = m_renderArea.x + areaWidth;
		m_renderArea.w = m_renderArea.y + areaHeight;

		m_rendFactPix = m_renderArea.y;

		if( m_renderArea.y > 0.0f )
		{
			m_renderFactor = m_renderArea.x / aspect / m_renderArea.y;

		}
		else
		{
			m_renderFactor = 0.0f;
		}
		setRenderFactor( m_renderFactor );
	
		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( RenderImageInterface* _image, const int* rect )
	{
		mt::vec2f offset;
		mt::mul_v2_m3( offset, mt::vec2f( rect[0], rect[1] ), m_renderTransform );
		int w = rect[2] - rect[0];
		int h = rect[3] - rect[1];
		int shotrect[4];
		shotrect[0] = offset.x;
		shotrect[1] = offset.y;
		shotrect[2] = offset.x + w;
		shotrect[3] = offset.y + h;
		m_interface->screenshot( _image, &(shotrect[0]) );
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
		if( m_currentRenderTarget == "defaultCamera" )
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
	void RenderEngine::renderTriple(		
			const mt::mat3f & _transform, 
			const mt::vec2f & _a,
			const mt::vec2f & _b,
			const mt::vec2f & _c,
			const mt::vec2f & _uv0,
			const mt::vec2f & _uv1,
			const mt::vec2f & _uv2,
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
		m_interface->renderTriple(
			tr.m,
			_a.m,
			_b.m,
			_c.m,
			_uv0.m,
			_uv1.m,
			_uv2.m,
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
		mt::vec2f rb, re;
		mt::mul_v2_m3( rb, begin, m_renderTransform );
		mt::mul_v2_m3( re, end, m_renderTransform );
		m_interface->renderLine( _color, rb.m, re.m );		
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
		/*if( m_windowCreated )
		{
			m_interface->setViewportDimensions( m_viewportWidth, m_viewportHeight, m_renderFactor );
			m_interface->onWindowMovedOrResized();
		}*/
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
		//m_interface->beginLayer2D();

		//Holder<Player>::hostage()->getArrow()->render( false );

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
		mt::vec4f renderArea = _renderArea;
		if( m_currentRenderTarget == "defaultCamera" )
		{
			mt::vec2f size = _renderArea.v2_1 - _renderArea.v2_0;
			if( size == mt::vec2f::zero_v2 )
			{
				renderArea = m_renderArea;
			}
			else
			{
				mt::box2f area;
				mt::set_box_from_oriented_extent( area, _renderArea.v2_0, _renderArea.v2_1 - _renderArea.v2_0, m_renderTransform );
				renderArea.v2_0 = area.min;
				renderArea.v2_1 = area.max;
			}
		}
		m_interface->setRenderArea( renderArea.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderFactor( float _factor )
	{
		m_renderFactor = _factor;
		if( m_renderFactor < 0.0f )
		{
			MENGE_LOG("Warning: RenderFactor Value < 0. Setting to 0");
			m_renderFactor = 0.0f;
		}
		else if( m_renderFactor > 1.0f )
		{
			MENGE_LOG("Warning: RenderFactor Value > 1. Setting to 1");
			m_renderFactor = 1.0f;
		}

		float areaWidth = m_renderArea.z - m_renderArea.x;
		float areaHeight = m_renderArea.w - m_renderArea.y;
		
		if( m_rendFactPix > 0.0f )
		{
			areaHeight += m_renderFactor * m_rendFactPix * 2;
			areaWidth = areaHeight * m_contentResolution.x / m_contentResolution.y;
			if( areaWidth > m_windowWidth )
			{
				areaWidth = m_windowWidth;
			}

			m_renderArea.x = ( m_windowWidth - areaWidth ) * 0.5f;
			m_renderArea.y = ( m_windowHeight - areaHeight ) * 0.5f;
			m_renderArea.z = m_renderArea.x + areaWidth;
			m_renderArea.w = m_renderArea.y + areaHeight;

			/*m_renderArea.x = ::floorf( m_renderArea.x + 0.5f );
			m_renderArea.y = ::floorf( m_renderArea.y + 0.5f );
			m_renderArea.z = ::floorf( m_renderArea.z + 0.5f );
			m_renderArea.w = ::floorf( m_renderArea.w + 0.5f );*/
		}

		mt::ident_m3( m_renderTransform );
		m_renderTransform[2][0] = m_renderArea.x;
		m_renderTransform[2][1] = m_renderArea.y;
		m_renderTransform[0][0] = areaWidth / m_contentResolution.x;
		m_renderTransform[1][1] = areaHeight / m_contentResolution.y;

		mt::ident_m4( m_renderTransform4 );
		//m_renderTransform4[3][0] = m_renderArea.x;
		//m_renderTransform4[3][1] = m_renderArea.y;
		m_renderTransform4[0][0] = areaWidth / m_windowWidth;
		m_renderTransform4[1][1] = areaHeight / m_windowHeight;

	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const String& _target )
	{
		if( m_currentRenderTarget != _target )
		{
			m_currentRenderTarget = _target;
			m_interface->setRenderTarget( m_currentRenderTarget );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& RenderEngine::getRenderArea() const
	{
		return m_renderArea;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f& RenderEngine::getRenderTransform() const
	{
		return m_renderTransform;
	}
	//////////////////////////////////////////////////////////////////////////
}
