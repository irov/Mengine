
#	include "HGERenderSystem.h"
#	include "HGE.h"
#	include "HGETexture.h"

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
{

}
//////////////////////////////////////////////////////////////////////////
HGERenderSystem::~HGERenderSystem()
{
	m_hge->System_Shutdown();
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
		initialized = true;
	}
	return initialized;
}
//////////////////////////////////////////////////////////////////////////
bool HGERenderSystem::createRenderWindow( float _width, float _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle, int _FSAAType, int _FSAAQuality )
{
	m_hge->System_SetState( HGE_SCREENWIDTH, _width );
	m_hge->System_SetState( HGE_SCREENHEIGHT, _height );
	m_hge->System_SetState( HGE_SCREENBPP, _bits );
	m_hge->System_SetState( HGE_WINDOWED, !_fullscreen );
	m_hge->System_SetState( HGE_HWND, (HWND)_winHandle );
	bool ret = false;
	ret = m_hge->System_Initiate( m_logSystem );
	return ret;
}
//////////////////////////////////////////////////////////////////////////
unsigned int HGERenderSystem::getResolutionList( float ** )
{
	return 0;
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
void HGERenderSystem::render( RenderImageInterface* _image, const int* rect /*= 0 */ )
{

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

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setProjectionMatrix( const float * _projection )
{
	
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setViewMatrix( const float * _view )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setWorldMatrix( const float * _world )
{

}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface * HGERenderSystem::createImage( const char* _name, unsigned int _width, unsigned int _height )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface * HGERenderSystem::createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height, const char* _camera )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface * HGERenderSystem::loadImage( const TextureDesc& _desc )
{
	HGETexture* texture = new HGETexture( m_hge );
	texture->load( _desc );
	return texture;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseImage( RenderImageInterface * _image )
{
	HGETexture* texture = static_cast<HGETexture*>( _image );
	delete texture;
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface * HGERenderSystem::getImage( const char * _desc ) const 
{
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
void HGERenderSystem::renderImage( const char * _camera,
								  const float * _transform, // матрица 3 на 3 
								  const float * _offset, // смещение, вектор2 
								  const float * _uv, // текстурные координаты, вектор4, u0, v0, u1, v1 
								  const float * _size, // размер изображения, вектор2 
								  unsigned int _color, // цвет, порядок ARGB
								  const RenderImageInterface * _image, 
								  EBlendFactor _src, 
								  EBlendFactor _dst ) 
{
	hgeQuad quad;

	quad.v[0].x = _transform[0] * _offset[0] + _transform[3] * _offset[1] + _transform[6];
	quad.v[0].y = _transform[1] * _offset[0] + _transform[4] * _offset[1] + _transform[7];
	quad.v[0].z = m_layer;
	quad.v[0].col = _color;

	quad.v[1].x = quad.v[0].x + _transform[0] * _size[0];
	quad.v[1].y = quad.v[0].y + _transform[1] * _size[0];
	quad.v[1].z = m_layer;
	quad.v[1].col = _color;

	quad.v[2].x = quad.v[1].x + _transform[3] * _size[1];
	quad.v[2].y = quad.v[1].y + _transform[4] * _size[1];
	quad.v[2].z = m_layer;
	quad.v[2].col = _color;

	quad.v[3].x = _transform[0] * (_offset[0]) + _transform[3] * (_offset[1] + _size[1]) + _transform[6];
	quad.v[3].y = _transform[1] * (_offset[0]) + _transform[4] * (_offset[1] + _size[1]) + _transform[7];
	quad.v[3].z = m_layer;
	quad.v[3].col = _color;

	quad.blend = BLEND_DEFAULT;

	quad.v[0].tx = _uv[0]; quad.v[0].ty = _uv[1];
	quad.v[1].tx = _uv[2]; quad.v[1].ty = _uv[1];
	quad.v[2].tx = _uv[2]; quad.v[2].ty = _uv[3];
	quad.v[3].tx = _uv[0]; quad.v[3].ty = _uv[3];

	quad.tex = static_cast<const HGETexture*>( _image )->getHandle();
	
	m_hge->Gfx_RenderQuad( &quad );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::renderImage( const char * _camera, 
								  const float * _transform,  
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

	quad.v[0].x = _transform[0] * _a[0] + _transform[3] * _a[1] + _transform[6];
	quad.v[0].y = _transform[1] * _a[0] + _transform[4] * _a[1] + _transform[7];
	quad.v[0].z = m_layer;
	quad.v[0].col = _color;

	quad.v[1].x = _transform[0] * _b[0] + _transform[6] * _b[1] + _transform[6];
	quad.v[1].y = _transform[1] * _b[0] + _transform[4] * _b[1] + _transform[7];
	quad.v[1].z = m_layer;
	quad.v[1].col = _color;

	quad.v[2].x = _transform[0] * _c[0] + _transform[3] * _c[1] + _transform[6];
	quad.v[2].y = _transform[1] * _c[0] + _transform[4] * _c[1] + _transform[7];
	quad.v[2].z = m_layer;
	quad.v[2].col = _color;

	quad.v[3].x = _transform[0] * _d[0] + _transform[3] * _d[1] + _transform[6];
	quad.v[3].y = _transform[1] * _d[0] + _transform[4] * _d[1] + _transform[7];
	quad.v[3].z = m_layer;
	quad.v[3].col = _color;

	quad.blend = BLEND_DEFAULT;

	quad.v[0].tx = _uv[0]; quad.v[0].ty = _uv[1];
	quad.v[1].tx = _uv[2]; quad.v[1].ty = _uv[1];
	quad.v[2].tx = _uv[2]; quad.v[2].ty = _uv[3];
	quad.v[3].tx = _uv[0]; quad.v[3].ty = _uv[3];

	quad.tex = static_cast<const HGETexture*>( _image )->getHandle();

	m_hge->Gfx_RenderQuad( &quad );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::renderLine( const char * _camera, unsigned int _color, const float * _begin, const float * _end )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::beginScene()
{
	m_layer = 1.0f;
	m_hge->Gfx_BeginScene();
	m_hge->Gfx_Clear( 255 );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::endScene()
{
	m_hge->Gfx_EndScene();
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::beginLayer()
{
	//m_hge->Gfx_BeginScene();
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::endLayer()
{
	//m_hge->Gfx_EndScene();
	m_layer -= 0.001f;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setFullscreenMode( float _width, float _height, bool _fullscreen )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setViewportDimensions( float _width, float _height, float _renderFactor )
{

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
