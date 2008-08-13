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
	RenderEngine::RenderEngine( RenderSystemInterface * _interface )
		: m_interface( _interface )
		, m_contentResolution( 1024.f, 768.f )
		, m_windowResolution( 1024.f, 768.f )
		, m_windowCreated( false )
		, m_renderFactor( 1.0f )
		, m_layer3D( false )
	{
		mt::ident_m4( m_viewTransform );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize()
	{
		LogSystemInterface * system = Holder<LogEngine>::hostage()->getInterface();

		bool result = m_interface->initialize( system );
		//m_interface->setEventListener( this );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int RenderEngine::getNumDIP() const
	{
		return m_interface->getNumDIP();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const mt::vec2f & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
											int _FSAAType, int _FSAAQuality )
	{
		m_fullscreen = _fullscreen;

		m_windowCreated = m_interface->createRenderWindow( (int)_resolution.x, (int)_resolution.y, _bits, _fullscreen, _winHandle,
															_FSAAType, _FSAAQuality );


		recalcRenderArea_( _resolution );
	
		setRenderTarget( "defaultCamera" );

		//m_renderViewport.set( mt::vec2f::zero_v2, m_contentResolution );

		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( RenderImageInterface* _image, const mt::vec4f & _rect )
	{
		m_interface->screenshot( _image, _rect.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render()
	{
		m_interface->render();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setContentResolution( const mt::vec2f _resolution )
	{
		m_contentResolution = _resolution;

		//m_renderViewport.set( mt::vec2f::zero_v2, m_contentResolution );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer2D()
	{
		m_interface->beginLayer2D();
		m_layer3D = false;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::endLayer2D()
	{
		m_interface->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer3D()
	{
		m_interface->beginLayer3D();
		m_layer3D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setProjectionMatrix( const mt::mat4f& _projection )
	{
		return m_interface->setProjectionMatrix( _projection.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setViewMatrix( const mt::mat4f& _view )
	{
		mt::mat4f view = _view;
		if( m_currentRenderTarget == "defaultCamera" && m_layer3D == false )
		{
			mt::mul_m4_m4( view, _view, m_viewTransform );
		}
		return m_interface->setViewMatrix( view.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setWorldMatrix( const mt::mat4f& _world )
	{
		return m_interface->setWorldMatrix( _world.m );
	}	
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::createImage( const String& _name, float _width, float _height )
	{
		RenderImageInterface * image = m_interface->createImage( _name, _width, _height );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::createRenderTargetImage( const String& _name, const mt::vec2f & _resolution )
	{
		RenderImageInterface * image = m_interface->createRenderTargetImage( _name, _resolution.x, _resolution.y );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderEngine::loadImage( const TextureDesc & _desc )
	{
		RenderImageInterface * image = m_interface->loadImage( _desc );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVideoStreamInterface* RenderEngine::loadImageVideoStream( const String & _filename )
	{
		RenderVideoStreamInterface * image = m_interface->loadImageVideoStream( _filename );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::loadImage( const String & _filename, unsigned int _filter )
	{
		RenderImageInterface * image = m_interface->getImage( _filename );

		if(image == 0)
		{

			Image cimage;
			cimage.load( _filename );

			TextureDesc	textureDesc;
			textureDesc.buffer = cimage.getData();
			textureDesc.size = cimage.getSize();
			textureDesc.width = (float)cimage.getWidth();
			textureDesc.height = (float)cimage.getHeight();
			textureDesc.pixelFormat = cimage.getFormat();
			textureDesc.name = _filename.c_str();
			textureDesc.filter = _filter;

			if( textureDesc.buffer == 0 )
			{
				MENGE_LOG( "Error: Image from file '%s' not loader\n"
					, _filename.c_str() 
					);

				return 0;
			}	

			image = loadImage( textureDesc );

			if( image == 0 )
			{
				MENGE_LOG( "Error: Image from file '%s' not loader\n"
					, _filename.c_str() 
					);

				return 0;
			}	
		}

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImage(		
			const mt::vec2f * _vertices,
			const mt::vec4f & _uv,
			unsigned int _color, 
			const RenderImageInterface* _image,
			EBlendFactor _src,
			EBlendFactor _dst)
	{
		m_interface->renderImage(
			_vertices->m,
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
		m_interface->renderTriple(
			_transform.m,
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
		m_interface->renderLine( _color, _begin.m, _end.m );		
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
		if( m_fullscreen == _fullscreen )
		{
			return;
		}

		m_fullscreen = _fullscreen;

		const mt::vec2f & resolution = ( m_fullscreen == true )
			? Holder<Application>::hostage()->getDesktopResolution() 
			: Holder<Game>::hostage()->getResolution();
			
		m_interface->setFullscreenMode( resolution.x, resolution.y, m_fullscreen );

		Holder<Application>::hostage()->notifyWindowModeChanged( resolution.x, resolution.y, m_fullscreen );

		recalcRenderArea_( resolution );
	}
	//////////////////////////////////////////////////////////////////////////
	CameraInterface * RenderEngine::createCamera( const std::string& _name )
	{
		return m_interface->createCamera( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	EntityInterface * RenderEngine::createEntity( const std::string& _name, const std::string& _meshName )
	{
		return m_interface->createEntity( _name, _meshName );
	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * RenderEngine::createLight( const std::string& _name )
	{
		return m_interface->createLight( _name );
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
	mt::vec2f RenderEngine::getBestDisplayResolution( const mt::vec2f & _resolution, float _aspect )
	{
		const std::vector<int> & resolutionList = m_interface->getResolutionList();

		float needWidth = _resolution.y * _aspect;

		typedef std::vector<int> TResolutionVector;
		typedef std::map< int, TResolutionVector > TResolutionMap;

		TResolutionMap resMap;

		std::size_t count = resolutionList.size() / 2;

		for( std::size_t i = 0; i < count; i++ )
		{
			if( fabsf( ( static_cast<float>( resolutionList[2*i] ) / resolutionList[2*i+1] ) - _aspect ) < 0.01f )
			{
				resMap[ resolutionList[2*i + 1] ].push_back( resolutionList[2*i] );
			}
		}

		bool done = false;

		mt::vec2f bestResolution;

		for( TResolutionMap::iterator 
			it = resMap.begin(),
			it_end = resMap.end();
		it != it_end; 
		++it )
		{
			if( it->first < static_cast<int>(_resolution.y) ) 
			{
				continue;
			}

			for( TResolutionVector::size_type 
				i = 0,
				i_end = it->second.size();
			i != i_end; 
			++i)
			{
				if( it->second[i] >= static_cast<int>(needWidth) )
				{
					bestResolution.x = static_cast<const float>(it->second[i]);
					bestResolution.y = static_cast<const float>(it->first);
					done = true;
					break;
				}
			}
			if( done ) break;
		}

		return bestResolution;
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
		const mt::vec2f & resolution = Holder<Game>::hostage()
			->getResolution();

		Holder<Application>::hostage()
			->notifyWindowModeChanged( resolution.x, resolution.y, m_fullscreen );
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
			/*else
			{
				mt::box2f area;
				mt::set_box_from_oriented_extent( area, _renderArea.v2_0, _renderArea.v2_1 - _renderArea.v2_0, m_renderTransform );
				renderArea.v2_0 = area.min;
				renderArea.v2_1 = area.max;
			}*/
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
			if( areaWidth > m_windowResolution.x )
			{
				areaWidth = m_windowResolution.x;
			}

			m_renderArea.x = ( m_windowResolution.x - areaWidth ) * 0.5f;
			m_renderArea.y = ( m_windowResolution.y - areaHeight ) * 0.5f;
			m_renderArea.z = m_renderArea.x + areaWidth;
			m_renderArea.w = m_renderArea.y + areaHeight;

			m_renderArea.x = ::floorf( m_renderArea.x + 0.5f );
			m_renderArea.y = ::floorf( m_renderArea.y + 0.5f );
			m_renderArea.z = ::floorf( m_renderArea.z + 0.5f );
			m_renderArea.w = ::floorf( m_renderArea.w + 0.5f );

			areaWidth = m_renderArea.z - m_renderArea.x;
			areaHeight = m_renderArea.w - m_renderArea.y;
		}

		m_viewTransform.m[0] = areaWidth / m_contentResolution.x;
		m_viewTransform.m[5] = areaHeight / m_contentResolution.y;
		m_viewTransform.m[12] = m_renderArea.x;
		m_viewTransform.m[13] = m_renderArea.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const String& _target, bool _clear )
	{
		if( m_currentRenderTarget != _target )
		{
			m_currentRenderTarget = _target;
			m_interface->setRenderTarget( m_currentRenderTarget, _clear );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& RenderEngine::getRenderArea() const
	{
		return m_renderArea;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::recalcRenderArea_( const mt::vec2f & resolution )
	{
		m_renderArea.x = 0.0f;
		m_renderArea.y = 0.0f;
		m_renderArea.z = resolution.x;
		m_renderArea.w = resolution.y;

		float one_div_width = 1.f / resolution.x;
		float one_div_height = 1.f / resolution.y;

		float contentAspect = m_contentResolution.x / m_contentResolution.y;
		float dw = m_contentResolution.x * one_div_width;
		float dh = m_contentResolution.y * one_div_height;
		float aspect = resolution.x * one_div_height;

		if( dw > 1.0f )
		{
			dw = 1.0f;
			dh = resolution.x / contentAspect * one_div_height;
		}

		if( dh > 1.0f )
		{
			dh = 1.0f;
			dw = resolution.y * contentAspect * one_div_width;
		}

		float areaWidth = dw * resolution.x;
		float areaHeight = dh * resolution.y;

		m_renderArea.x = ( resolution.x - areaWidth ) * 0.5f;
		m_renderArea.y = ( resolution.y - areaHeight ) * 0.5f;
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

		m_windowResolution = resolution;

		setRenderFactor( m_renderFactor );
	}
	//////////////////////////////////////////////////////////////////////////
}
