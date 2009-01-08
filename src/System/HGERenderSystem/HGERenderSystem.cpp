#	include "HGERenderSystem.h"
#	include "HGETexture.h"
#	include "HGERenderFont.h"
#	include "Interface/LogSystemInterface.h"
#	include "libs/Math/mat4.h"
#	include <sstream>
#	include "Menge/LogEngine.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::RenderSystemInterface ** _ptrInterface )
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
void releaseInterfaceSystem( Menge::RenderSystemInterface* _ptrInterface )
{
	delete static_cast<HGERenderSystem*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
hgeBlendState s_blendMengeToHGE( Menge::EBlendFactor _blend )
{
	switch( _blend )
	{
	case Menge::BF_ONE:
		return BLEND_ONE;
	case Menge::BF_ZERO:
		return BLEND_ZERO;
	case Menge::BF_DEST_COLOUR:
		return BLEND_DESTCOLOR;
	case Menge::BF_SOURCE_COLOUR:
		return BLEND_SRCCOLOR;
	case Menge::BF_ONE_MINUS_DEST_COLOUR:
		return BLEND_INVDESTCOLOR;
	case Menge::BF_ONE_MINUS_SOURCE_COLOUR:
		return BLEND_INVSRCCOLOR;
	case Menge::BF_DEST_ALPHA:
		return BLEND_DESTALPHA;
	case Menge::BF_SOURCE_ALPHA:
		return BLEND_SRCALPHA;
	case Menge::BF_ONE_MINUS_DEST_ALPHA:
		return BLEND_INVDESTALPHA;
	case Menge::BF_ONE_MINUS_SOURCE_ALPHA:
		return BLEND_INVSRCALPHA;
	}
	return BLEND_ZERO;
}
//////////////////////////////////////////////////////////////////////////
Menge::uint32 s_firstPO2From( Menge::uint32 _n )
{
	--_n;            
	_n |= _n >> 16;
	_n |= _n >> 8;
	_n |= _n >> 4;
	_n |= _n >> 2;
	_n |= _n >> 1;
	++_n;
	return _n;
}
//////////////////////////////////////////////////////////////////////////
HGERenderSystem::HGERenderSystem()
: m_hge( NULL )
, m_layer( 1.0f )
, m_inRender( false )
, m_layer3D( false )
#ifdef _DEBUG
, m_clearColor( 255 )
#else
, m_clearColor( 0 )
#endif
, m_renderX( 1.0f )
, m_renderY( 1.0f )
, m_systemFont(0)
{

}
//////////////////////////////////////////////////////////////////////////
HGERenderSystem::~HGERenderSystem()
{
	delete m_systemFont;
	if( m_hge != NULL )
	{
		m_hge->Release();
		m_hge = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
int HGERenderSystem::getNumDIP() const
{
	return m_hge->GetNumDips();
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
bool HGERenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, Menge::WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
{
	m_hge->System_SetState( HGE_SCREENWIDTH, _width );
	m_hge->System_SetState( HGE_SCREENHEIGHT, _height );
	m_hge->System_SetState( HGE_SCREENBPP, _bits );
	m_hge->System_SetState( HGE_WINDOWED, !_fullscreen );
	m_hge->System_SetState( HGE_HWND, (HWND)_winHandle );
	m_hge->System_SetState( HGE_ZBUFFER, true );
	m_hge->Gfx_SetListener( this );
	//m_hge->System_SetState( HGE_FPS, HGEFPS_VSYNC );
	//m_hge->System_SetState( HGE_TEXTUREFILTER, false );]
	m_currentRenderTarget = "defaultCamera";

	HTARGET * voidTarget = 0;
	HGETexture* voidTexture = 0;
	RenderTargetInfo rtgtInfo;
	rtgtInfo.dirty = true;
	rtgtInfo.handle = voidTarget;
	rtgtInfo.texture = voidTexture;
	m_targetMap.insert( std::make_pair( m_currentRenderTarget, rtgtInfo ) );
	
	bool ret = m_hge->Gfx_CreateRenderWindow();

	if( ret == false )
	{
		return false;
	}

	m_systemFont = new HGERenderFont(m_hge);
	m_systemFont->fontGenerate( "Verdana", 16, false, true, false );

	return true;
}
//////////////////////////////////////////////////////////////////////////
const std::vector<int> & HGERenderSystem::getResolutionList()
{
	const std::vector<int> & list = m_hge->Gfx_GetModeList();
	return list;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::screenshot( Menge::RenderImageInterface* _image, const float * _rect /*= 0 */ )
{
	RECT rect;
	if( _rect )
	{
		rect.left = (LONG)_rect[0];
		rect.top = (LONG)_rect[1];
		rect.right = (LONG)_rect[2];
		rect.bottom = (LONG)_rect[3];
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
void HGERenderSystem::setContentResolution( const std::size_t * _resolution )
{
	m_contentResolution[0] = _resolution[0];
	m_contentResolution[1] = _resolution[1];
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
Menge::RenderImageInterface * HGERenderSystem::createImage( const Menge::String & _name,
													std::size_t _width, std::size_t _height, Menge::PixelFormat _format )
{
	if( _format == Menge::PF_R8G8B8 )
	{
		_format = Menge::PF_X8R8G8B8;
	}
	std::size_t image_width = _width;
	std::size_t image_height = _height;
	if( ( _width & ( _width - 1 ) ) != 0
	|| ( _height & ( _height - 1 ) ) != 0 )
	{
		bool npot = supportNPOT();
		if( npot == false )	// we're all gonna die
		{
			_width = s_firstPO2From( _width );
			_height = s_firstPO2From( _height );
		}
	}

	HGETexture* texture = new HGETexture( m_hge, _name, image_width, image_height, _width, _height, _format );
	m_textureMap.insert( std::make_pair( _name, static_cast<Menge::RenderImageInterface*>( texture ) ) );
	texture->incRef();
	return texture;
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * HGERenderSystem::createRenderTargetImage( const Menge::String & _name, std::size_t _width, std::size_t _height )
{
	std::size_t image_width = _width;
	std::size_t image_height = _height;
	if( ( _width & ( _width - 1 ) ) != 0
	|| ( _height & ( _height - 1 ) ) != 0 )
	{
		bool npot = supportNPOT();
		if( npot == false )	// we're all gonna die
		{
			_width = s_firstPO2From( _width );
			_height = s_firstPO2From( _height );
		}
	}

	HTARGET htgt = m_hge->Target_Create( (int)_width, (int)_height, true );
	HGETexture* texture = new HGETexture( m_hge, m_hge->Target_GetTexture( htgt ), _name, image_width, image_height );
	RenderTargetInfo rtgtInfo;
	rtgtInfo.dirty = true;
	rtgtInfo.handle = htgt;
	rtgtInfo.texture = texture;
	m_targetMap.insert( std::make_pair( _name, rtgtInfo ) );
	return texture;
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * HGERenderSystem::loadImage( const Menge::String& _name, std::size_t _width, std::size_t _height, const Menge::TextureDesc& _desc )
{
	HGETexture* texture = new HGETexture( m_hge );
	texture->load( _name, _width, _height, _desc );
	m_textureMap.insert( std::make_pair( _desc.name, static_cast<Menge::RenderImageInterface*>( texture ) ) );
	texture->incRef();
	return texture;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseImage( Menge::RenderImageInterface * _image )
{
	HGETexture* texture = static_cast<HGETexture*>( _image );
	if( !_image )
	{
		return;
	}
	TTargetMap::iterator it = m_targetMap.find( texture->getDescription() );
	if( it != m_targetMap.end() )
	{
		m_hge->Target_Free( it->second.handle );
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
Menge::RenderImageInterface* HGERenderSystem::getImage( const Menge::String& _desc ) const 
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
void HGERenderSystem::renderImage(const float * _renderVertex, 
								  const float * _uv, 
								  unsigned int _color,  
								  const Menge::RenderImageInterface * _image, 
								  Menge::EBlendFactor _srcBlend, 
								  Menge::EBlendFactor _dstBlend )
{
	/*std::size_t iw = 1;
	std::size_t ih = 1;
	if( _image )
	{
		iw = _image->getWidth();
		ih = _image->getHeight();
	}
	const float texOffs[2] = { 0.1f / iw, 0.1f / ih };
	float uv[4] = { _uv[0] + texOffs[0], _uv[1] + texOffs[1], _uv[2] - texOffs[0], _uv[3] - texOffs[1] };*/

	hgeQuad quad;

	quad.v[0].x = _renderVertex[0];
	quad.v[0].y = _renderVertex[1];
	quad.v[0].z = m_layer;
	quad.v[0].col = _color;

	quad.v[1].x = _renderVertex[2];
	quad.v[1].y = _renderVertex[3];
	quad.v[1].z = m_layer;
	quad.v[1].col = _color;

	quad.v[2].x = _renderVertex[4];
	quad.v[2].y = _renderVertex[5];
	quad.v[2].z = m_layer;
	quad.v[2].col = _color;

	quad.v[3].x = _renderVertex[6];
	quad.v[3].y = _renderVertex[7];
	quad.v[3].z = m_layer;
	quad.v[3].col = _color;

	for( int i = 0; i < 4; i++ )
	{
		quad.v[i].x = ::floorf( quad.v[i].x + 0.5f );
		quad.v[i].y = ::floorf( quad.v[i].y + 0.5f );
	}

	//quad.blend = BLEND_DEFAULT;

	/*if( _dst == BF_ONE )
	{
		quad.blend ^= BLEND_ALPHABLEND;
	}*/

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

	quad.srcBlend = s_blendMengeToHGE( _srcBlend );
	quad.dstBlend = s_blendMengeToHGE( _dstBlend );

	m_hge->Gfx_RenderQuad( &quad );
	//m_hge->Gfx_SetBlendState( s_blendMengeToHGE( _srcBlend ), s_blendMengeToHGE( _dstBlend ) );

	/*if( tex )
	{
		texmat[0][0] = 1.0f;
		texmat[1][1] = 1.0f;
		m_hge->Gfx_SetTextureMatrix( texmat.m );
	}*/
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::renderTriple(const float * _a, 
								  const float * _b, 
								  const float * _c, 
								  const float * _uv0, 
								  const float * _uv1,
								  const float * _uv2,
								  unsigned int _color,  
								  const Menge::RenderImageInterface * _image, 
								  Menge::EBlendFactor _src, 
								  Menge::EBlendFactor _dst )
{
	hgeTriple triangle;

	triangle.v[0].x = _a[0];
	triangle.v[0].y = _a[1];
	triangle.v[0].z = m_layer;
	triangle.v[0].col = _color;

	triangle.v[1].x = _b[0];
	triangle.v[1].y = _b[1];
	triangle.v[1].z = m_layer;
	triangle.v[1].col = _color;

	triangle.v[2].x = _c[0];
	triangle.v[2].y = _c[1];
	triangle.v[2].z = m_layer;
	triangle.v[2].col = _color;

	for( int i = 0; i < 3; i++ )
	{
		triangle.v[i].x = ::floorf( triangle.v[i].x + 0.5f );
		triangle.v[i].y = ::floorf( triangle.v[i].y + 0.5f );
	}

	triangle.blend = BLEND_DEFAULT;

	if( _dst == Menge::BF_ONE )
	{
		triangle.blend ^= BLEND_ALPHABLEND;
	}

	triangle.v[0].tx = _uv0[0];
	triangle.v[0].ty = _uv0[1];

	triangle.v[1].tx = _uv1[0];
	triangle.v[1].ty = _uv1[1];

	triangle.v[2].tx = _uv2[0];
	triangle.v[2].ty = _uv2[1];

	const HGETexture* tex = static_cast<const HGETexture*>( _image );
	//mt::mat4f texmat;
	//mt::ident_m4( texmat );
	if( tex )
	{
		const mt::vec2f& uvMask = tex->getUVMask();

		//texmat[0][0] = uvMask.x;
		//texmat[1][1] = uvMask.y;
		for( int i = 0; i < 3; i++ )
		{
			triangle.v[i].tx *= uvMask.x;
			triangle.v[i].ty *= uvMask.y;
		}

		//m_hge->Gfx_SetTextureMatrix( texmat.m );

		triangle.tex = tex->getHandle();
	}
	else
	{
		triangle.tex = 0;
	}

	m_hge->Gfx_RenderTriple( &triangle );

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
	if( m_inRender )
	{
		m_hge->Gfx_EndScene();
	}
	if( !m_hge->Gfx_BeginScene() )
	{
		m_logSystem->logMessage( "Error: D3D8 Failed to BeginScene" );
	}
	m_hge->Gfx_SetClipping();
	m_hge->Gfx_Clear( m_clearColor );

	m_inRender = true;
	m_currentRenderTarget = "defaultCamera";

	// set render targets dirty to clear one time before rendering into one
	for( TTargetMap::iterator it = m_targetMap.begin(), it_end = m_targetMap.end();
		it != it_end;
		it++ )
	{
		it->second.dirty = true;
	}

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::endScene()
{
	m_hge->Gfx_EndScene();
	m_inRender = false;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::swapBuffers()
{
	m_hge->Gfx_SwapBuffers();
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
void HGERenderSystem::renderText(const Menge::String & _text, const float * _pos, unsigned long _color)
{
	m_systemFont->renderText(_pos[0],_pos[1],_color,_text);
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
void HGERenderSystem::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
{
	// todo: backup render targets content

	m_hge->Gfx_ChangeMode( (int)_width, (int)_height, 32, _fullscreen );
}
//////////////////////////////////////////////////////////////////////////
Menge::CameraInterface * HGERenderSystem::createCamera( const Menge::String & _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
Menge::EntityInterface * HGERenderSystem::createEntity( const Menge::String & _name, const Menge::String & _meshName )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
Menge::LightInterface * HGERenderSystem::createLight( const Menge::String & _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseCamera( Menge::CameraInterface * _camera )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseEntity( Menge::EntityInterface * _entity )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::releaseLight( Menge::LightInterface * _light )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setTextureFiltering( bool _filter )
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::onWindowMovedOrResized()
{

}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::onWindowClose()
{
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::renderMesh( const Menge::TVertex* _vertices, std::size_t _verticesNum,
								 const Menge::uint16*	_indices, std::size_t _indicesNum,
								 Menge::TMaterial* _material )
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
			mtex.v0.x = tex->getUVMask().x;
			mtex.v1.y = tex->getUVMask().y;
			m_hge->Gfx_SetTextureMatrix( mtex.buff() );
			htex = tex->getHandle();
		}
	}
	m_hge->Gfx_RenderMesh( &(vtx[0]), _verticesNum, _indices, _indicesNum, htex );

	mt::mat4f ident;
	mt::ident_m4( ident );
	m_hge->Gfx_SetTextureMatrix( ident.buff() );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setRenderTarget( const Menge::String& _name, bool _clear )
{
	TTargetMap::iterator it = m_targetMap.find( _name );
	if( it != m_targetMap.end() )
	{
		if( m_inRender )
		{
			m_hge->Gfx_EndScene();
		}
		else
		{
			m_inRender = true;
		}
		m_currentRenderTarget = _name;
		m_hge->Gfx_BeginScene( it->second.handle );
		if( it->second.dirty && _clear )
		{
			m_hge->Gfx_Clear( m_clearColor );
			it->second.dirty = false;
		}

	}
	else
	{
		MENGE_LOG_WARNING( "Warning: Invalid Render Target name \"%s\""
			, _name.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::setRenderArea( const float* _renderArea )
{
	float w = _renderArea[2] - _renderArea[0];
	float h = _renderArea[3] - _renderArea[1];

	m_hge->Gfx_SetClipping( (int)_renderArea[0], (int)_renderArea[1], (int)w, (int)h );
}
//////////////////////////////////////////////////////////////////////////
void HGERenderSystem::onRestoreDevice()
{
	// restoring render targets
	for( TTargetMap::iterator it = m_targetMap.begin(), it_end = m_targetMap.end();
		it != it_end;
		it++ )
	{
		if( it->second.texture != 0 )
		{
			HTEXTURE htex = m_hge->Target_GetTexture( it->second.handle );
			it->second.texture->restore( htex );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool HGERenderSystem::supportNPOT()
{
	return m_hge->Gfx_SupportNPOT();
	//return false;
}
//////////////////////////////////////////////////////////////////////////
