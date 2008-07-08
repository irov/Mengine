
#	include "HGERenderSystem.h"
//#	include "HGE.h"
#	include "HGETexture.h"
#	include "Interface/LogSystemInterface.h"
#	include "libs/Math/mat4.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( RenderSystemInterface ** _ptrInterface )
{
	try
	{
		*_ptrInterface = new HGERenderSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( RenderSystemInterface* _ptrInterface )
{
	delete static_cast<HGERenderSystem*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
HGERenderSystem::HGERenderSystem()
: m_hge( NULL )
, m_layer( 1.0f )
, m_contentResolution( 1024.0f, 768.0f )
, m_inRender( false )
, m_layer3D( false )
#ifdef _DEBUG
, m_clearColor( 255 )
#else
, m_clearColor( 0 )
#endif
, m_renderX( 1.0f )
, m_renderY( 1.0f )
{

}
//////////////////////////////////////////////////////////////////////////
HGERenderSystem::~HGERenderSystem()
{
	m_hge->Release();
}
//////////////////////////////////////////////////////////////////////////
bool HGERenderSystem::initialize( Menge::LogSystemInterface* _logSystem )
{
	m_logSystem = _logSystem;
	m_hge = hgeCreate( HGE_VERSION );

	bool initialized = false;
	if( m_hge )
	{
		initialized =  m_hge->System_Initiate( m_logSystem );
	}
	return initialized;
}
//////////////////////////////////////////////////////////////////////////
bool HGERenderSystem::createRenderWindow( int _width, int _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle, int _FSAAType, int _FSAAQuality )
{
	m_hge->System_SetState( HGE_SCREENWIDTH, _width );
	m_hge->System_SetState( HGE_SCREENHEIGHT, _height );
	m_hge->System_SetState( HGE_SCREENBPP, _bits );
	m_hge->System_SetState( HGE_WINDOWED, !_fullscreen );
	m_hge->System_SetState( HGE_HWND, (HWND)_winHandle );
	m_hge->System_SetState( HGE_ZBUFFER, true );
	m_hge->System_SetState( HGE_FPS, HGEFPS_VSYNC );
	//m_hge->System_SetState( HGE_TEXTUREFILTER, false );
	m_currentRenderTarget = "defaultCamera";
	m_viewport = mt::box2f( mt::vec2f( 0.0f, 0.0f ), mt::vec2f( _width, _height ) );

	HTARGET * voidTarget = 0;
	m_targetMap.insert( std::make_pair( m_currentRenderTarget, voidTarget ) );
	bool ret = false;
	ret = m_hge->Gfx_CreateRenderWindow();
	return ret;
}
//////////////////////////////////////////////////////////////////////////
unsigned int HGERenderSystem::getResolutionList( int ** _list )
{
	static std::vector<int> list = m_hge->Gfx_GetModeList();
	*_list = &(list[0]);
	return list.size();
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::addResourceLocation( const char* _path )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::initResources()
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::screenshot( RenderImageInterface* _image, const int* _rect /*= 0 */ )
{
	RECT rect;
	if( _rect )
	{
		rect.left = _rect[0];
		rect.top = _rect[1];
		rect.right = _rect[2];
		rect.bottom = _rect[3];
	}
	else
	{
		rect.left = 0;
		rect.top = 0;
		rect.right = m_hge->System_GetState( HGE_SCREENWIDTH );
		rect.bottom = m_hge->System_GetState( HGE_SCREENHEIGHT );
	}
	HGETexture* tex = static_cast<HGETexture*>( _image );
	m_hge->Gfx_Snapshot( tex->getHandle(), rect );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::render()
{
	//m_hge->Gfx_Clear( 255 );
	//m_layer = 1.0f;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setContentResolution( const float * _resolution )
{
	m_contentResolution.x = _resolution[0];
	m_contentResolution.y = _resolution[1];
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setProjectionMatrix( const float * _projection )
{
	m_hge->Gfx_SetProjectionMatrix( _projection );	
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setViewMatrix( const float * _view )
{
	m_hge->Gfx_SetViewMatrix( _view );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setWorldMatrix( const float * _world )
{
	m_hge->Gfx_SetWorldMatrix( _world );
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface * HGERenderSystem::createImage( const char* _name,
													unsigned int _width, unsigned int _height )
{
	HGETexture* texture = new HGETexture( m_hge, _name, _width, _height );
	m_textureMap.insert( std::make_pair( _name, static_cast<RenderImageInterface*>( texture ) ) );
	texture->incRef();
	return texture;
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface * HGERenderSystem::createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height )
{
	HTARGET htgt = m_hge->Target_Create( _width, _height, true );
	m_targetMap.insert( std::make_pair( _name, htgt ) );
	HGETexture* texture = new HGETexture( m_hge, m_hge->Target_GetTexture( htgt ), _name, _width, _height );
	return texture;
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface * HGERenderSystem::loadImage( const TextureDesc& _desc )
{
	HGETexture* texture = new HGETexture( m_hge );
	texture->load( _desc );
	m_textureMap.insert( std::make_pair( _desc.name, static_cast<RenderImageInterface*>( texture ) ) );
	texture->incRef();
	return texture;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseImage( RenderImageInterface * _image )
{
	HGETexture* texture = static_cast<HGETexture*>( _image );
	if( !_image )
	{
		return;
	}
	TTargetMap::iterator it = m_targetMap.find( texture->getDescription() );
	if( it != m_targetMap.end() )
	{
		m_hge->Target_Free( it->second );
		m_targetMap.erase( it );
		delete texture;
	}
	else
	{
		TTextureMap::iterator itt = m_textureMap.find( texture->getDescription() );
		if( itt != m_textureMap.end() )
		{
			if( texture->decRef() == 0 )
			{
				m_textureMap.erase( itt );
				delete texture;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* HGERenderSystem::getImage( const Menge::String& _desc ) const 
{
	TTextureMap::const_iterator it = m_textureMap.find( _desc );
	if( it != m_textureMap.end() )
	{
		static_cast<HGETexture*>( it->second )->incRef();
		return it->second;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
RenderVideoStreamInterface* HGERenderSystem::loadImageVideoStream( const char* _filename )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseImageVideoStream( RenderVideoStreamInterface* _image )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::renderImage(const float * _transform,  
								  const float * _a, 
								  const float * _b, 
								  const float * _c, 
								  const float * _d, 
								  const float * _uv, 
								  unsigned int _color,  
								  const RenderImageInterface * _image, 
								  EBlendFactor _src, 
								  EBlendFactor _dst )
{
	hgeQuad quad;

	quad.v[0].x = _transform[0] * _a[0] + _transform[3] * _a[1] + _transform[6] + m_viewport.min.x;
	quad.v[0].y = _transform[1] * _a[0] + _transform[4] * _a[1] + _transform[7] + m_viewport.min.y;
	quad.v[0].z = m_layer;
	quad.v[0].col = _color;

	quad.v[1].x = _transform[0] * _b[0] + _transform[3] * _b[1] + _transform[6] + m_viewport.min.x;
	quad.v[1].y = _transform[1] * _b[0] + _transform[4] * _b[1] + _transform[7] + m_viewport.min.y;
	quad.v[1].z = m_layer;
	quad.v[1].col = _color;

	quad.v[2].x = _transform[0] * _c[0] + _transform[3] * _c[1] + _transform[6] + m_viewport.min.x;
	quad.v[2].y = _transform[1] * _c[0] + _transform[4] * _c[1] + _transform[7] + m_viewport.min.y;
	quad.v[2].z = m_layer;
	quad.v[2].col = _color;

	quad.v[3].x = _transform[0] * _d[0] + _transform[3] * _d[1] + _transform[6] + m_viewport.min.x;
	quad.v[3].y = _transform[1] * _d[0] + _transform[4] * _d[1] + _transform[7] + m_viewport.min.y;
	quad.v[3].z = m_layer;
	quad.v[3].col = _color;

	quad.v[0].x *= m_renderX;
	quad.v[0].y *= m_renderY;
	quad.v[1].x *= m_renderX;
	quad.v[1].y *= m_renderY;
	quad.v[2].x *= m_renderX;
	quad.v[2].y *= m_renderY;
	quad.v[3].x *= m_renderX;
	quad.v[3].y *= m_renderY;

	quad.blend = BLEND_DEFAULT;

	if( _dst == BF_ONE )
	{
		quad.blend ^= BLEND_ALPHABLEND;
	}

	quad.v[0].tx = _uv[0];
	quad.v[0].ty = _uv[1];
	quad.v[1].tx = _uv[2];
	quad.v[1].ty = _uv[1];
	quad.v[2].tx = _uv[2];
	quad.v[2].ty = _uv[3];
	quad.v[3].tx = _uv[0]; 
	quad.v[3].ty = _uv[3];

	const HGETexture* tex = static_cast<const HGETexture*>( _image );
	//mt::mat4f texmat;
	//mt::ident_m4( texmat );
	if( tex )
	{
		const mt::vec2f& uvMask = tex->getUVMask();

		//texmat[0][0] = uvMask.x;
		//texmat[1][1] = uvMask.y;
		for( int i = 0; i < 4; i++ )
		{
			quad.v[i].tx *= uvMask.x;
			quad.v[i].ty *= uvMask.y;
		}

		//m_hge->Gfx_SetTextureMatrix( texmat.m );

		quad.tex = tex->getHandle();
	}
	else
	{
		quad.tex = 0;
	}

	m_hge->Gfx_RenderQuad( &quad );

	/*if( tex )
	{
		texmat[0][0] = 1.0f;
		texmat[1][1] = 1.0f;
		m_hge->Gfx_SetTextureMatrix( texmat.m );
	}*/
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::renderLine( unsigned int _color, 
								 const float * _begin, 
								 const float * _end )
{
	m_hge->Gfx_RenderLine( _begin[0], _begin[1], _end[0], _end[1], _color, m_layer );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::beginScene()
{
	m_layer = 1.0f;
	if( !m_hge->Gfx_BeginScene() )
	{
		m_logSystem->logMessage("Error: D3D8 Failed to BeginScene");
	}
	//m_hge->Gfx_SetClipping( 0, 0, m_hge->System_GetState( HGE_SCREENWIDTH ), m_hge->System_GetState( HGE_SCREENHEIGHT ) );
	// clear entire screen
	m_hge->Gfx_SetClipping();
	m_hge->Gfx_Clear( m_clearColor );
	//m_hge->Gfx_SetClipping( m_viewport.min.x, m_viewport.min.y, m_viewport.max.x, m_viewport.max.y );

	//m_renderX = (m_viewport.max - m_viewport.min).x / m_contentResolution.x;
	//m_renderY = (m_viewport.max - m_viewport.min).y / m_contentResolution.y;

	m_inRender = true;
	m_currentRenderTarget = "defaultCamera";
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::endScene()
{
	m_hge->Gfx_EndScene();
	m_inRender = false;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::beginLayer2D()
{
	if( m_layer3D == false ) return;	// already 2D
	m_hge->Gfx_Prepare2D();
	m_layer3D = false;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::endLayer2D()
{
	m_layer -= 0.001f;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::beginLayer3D()
{
	if( m_layer3D ) return;	// already 3D
	m_hge->Gfx_Prepare3D();
	m_layer3D = true;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::endLayer3D()
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setFullscreenMode( float _width, float _height, bool _fullscreen )
{
	m_hge->Gfx_ChangeMode( _width, _height, 32, _fullscreen );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setViewportDimensions( float _width, float _height, float _renderFactor )
{
	float realWidth = m_hge->System_GetState( HGE_SCREENWIDTH );
	float realHeight = m_hge->System_GetState( HGE_SCREENHEIGHT );
	float aspect = _width / _height;
	float width = _width / realWidth;
	float height = _height / realHeight;

	if( width > 1.0f )
	{
		width = 1.0f;
		height = realWidth / aspect / realHeight;
	}

	if( height > 1.0f )
	{
		height = 1.0f;
		width = realHeight * aspect / realWidth;
	}

	if( _renderFactor )
	{
		width += ( 1.0f - width ) * _renderFactor;
		height += ( 1.0f - height ) * _renderFactor;
	}

	/*m_viewportDimensions[0] = 0.5f - width / 2.0f;
	m_viewportDimensions[1] = 0.5f - height / 2.0f;
	m_viewportDimensions[2] = width;
	m_viewportDimensions[3] = height;*/

	m_viewport.min.x = ( 0.5f - width / 2.0f ) * realWidth;
	m_viewport.min.y = ( 0.5f - height / 2.0f ) * realHeight;
	m_viewport.max.x = width * realWidth;
	m_viewport.max.y = height * realHeight;

	/*m_hge->Gfx_Clear( 255 );
	m_hge->Gfx_SetClipping( x, y, w, h );*/


	//m_viewport->setDimensions( m_viewportDimensions[0], m_viewportDimensions[1], m_viewportDimensions[2], m_viewportDimensions[3] );

}
//////////////////////////////////////////////////////////////////////////
CameraInterface * HGERenderSystem::createCamera( const char * _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
EntityInterface * HGERenderSystem::createEntity( const char * _name, const char * _meshName )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
LightInterface * HGERenderSystem::createLight( const char * _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
SceneNodeInterface * HGERenderSystem::createSceneNode( const std::string & _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseCamera( CameraInterface * _camera )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseEntity( EntityInterface * _entity )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseLight( LightInterface * _light )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseSceneNode( SceneNodeInterface * _interface )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setTextureFiltering( bool _filter )
{

}
//////////////////////////////////////////////////////////////////////////
SceneNodeInterface * HGERenderSystem::getRootSceneNode() const 
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setEventListener( RenderSystemListener* _listener )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::onWindowMovedOrResized()
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::onWindowActive( bool _active )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::onWindowClose()
{
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::renderMesh( const TVertex* _vertices, std::size_t _verticesNum,
								 const Menge::uint16*	_indices, std::size_t _indicesNum,
								 TMaterial* _material )
{
	static std::vector<mengeVertex> vtx;
	vtx.clear();
	vtx.reserve( _verticesNum );
	mengeVertex v;
	for( size_t i = 0; i < _verticesNum; i++ )
	{
		v.x = _vertices[i].pos[0];
		v.y = _vertices[i].pos[1];
		v.z = _vertices[i].pos[2];
		v.nx = _vertices[i].n[0];
		v.ny = _vertices[i].n[1];
		v.nz = _vertices[i].n[2];
		v.tx = _vertices[i].uv[0];
		v.ty = _vertices[i].uv[1];
		v.col = _material->color;
		//v.col = _vertices[i].col;
		vtx.push_back( v );
	}
	HTEXTURE htex = 0;
	if( _material )
	{
		HGETexture* tex = static_cast<HGETexture*>( _material->texture );
		if( tex )
		{
			mt::mat4f mtex;
			mt::ident_m4( mtex );
			mtex[0][0] = tex->getUVMask().x;
			mtex[1][1] = tex->getUVMask().y;
			m_hge->Gfx_SetTextureMatrix( mtex.m );
			htex = tex->getHandle();
		}
	}
	m_hge->Gfx_RenderMesh( &(vtx[0]), _verticesNum, _indices, _indicesNum, htex );

	mt::mat4f ident;
	mt::ident_m4( ident );
	m_hge->Gfx_SetTextureMatrix( ident.m );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setRenderTarget( const Menge::String& _name )
{
	if( m_currentRenderTarget == _name ) return;
	TTargetMap::iterator it = m_targetMap.find( _name );
	if( it != m_targetMap.end() )
	{
		if( m_inRender )
		{
			m_hge->Gfx_EndScene( false );
		}
		m_currentRenderTarget = _name;
		m_hge->Gfx_BeginScene( it->second );
		m_hge->Gfx_Clear( m_clearColor );

		if( it->second )
		{
			//m_renderX =  m_hge->Texture_GetWidth( m_hge->Target_GetTexture( it->second ) ) / m_contentResolution.x;
			//m_renderY = m_hge->Texture_GetHeight( m_hge->Target_GetTexture( it->second ) ) / m_contentResolution.y;
		}
		else
		{
			//m_renderX = (m_viewport.max - m_viewport.min).x / m_contentResolution.x;
			//m_renderY = (m_viewport.max - m_viewport.min).y / m_contentResolution.y;
		}
	}
	else
	{
		m_logSystem->logMessage( "Warning: Invalid Render Target name ", false, false, true );
		m_logSystem->logMessage( _name, false, true, false );
	}
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setRenderArea( const float* _renderArea )
{
	int w = _renderArea[2] - _renderArea[0];
	int h = _renderArea[3] - _renderArea[1];

	m_hge->Gfx_SetClipping( _renderArea[0], _renderArea[1], w, h );
}
//////////////////////////////////////////////////////////////////////////