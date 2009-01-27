/*
 *	DX8RenderSystem.cpp
 *
 *	Created by _Berserk_ on 22.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DX8RenderSystem.h"
#	include "DX8Texture.h"
#	include "DX8RenderTexture.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#	include <cmath>
#	include <algorithm>

#if defined(_MSC_VER)
#define snprintf _snprintf
#endif

#define VERTEX_BUFFER_SIZE 10000
#define D3DFVF_MENGE_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_NORMAL)

#define MENGE_PRIM_LINES		2
#define MENGE_PRIM_TRIPLES		3
#define MENGE_PRIM_QUADS		4
#define MENGE_PRIM_MESH			5

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::RenderSystemInterface ** _ptrInterface )
{
	*_ptrInterface = new Menge::DX8RenderSystem();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::RenderSystemInterface* _ptrInterface )
{
	delete static_cast<Menge::DX8RenderSystem*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	D3DFORMAT s_toD3DFormat( int _format )
	{
		switch( _format )
		{
		case 1:
			return D3DFMT_L8;
		case 3:
			return D3DFMT_A8;
		case 4:
			return D3DFMT_A4L4;
		case 5:
			return D3DFMT_A8L8; // Assume little endian here
		case 31:
			return D3DFMT_R3G3B2;
		case 9:
			return D3DFMT_A1R5G5B5;
		case 6:
			return D3DFMT_R5G6B5;
		case 8:
			return D3DFMT_A4R4G4B4;
		case 10:
			return D3DFMT_R8G8B8;
		case 12:
			return D3DFMT_A8R8G8B8;
		case 26:
			return D3DFMT_X8R8G8B8;
		case 34:
			return D3DFMT_G16R16;
		case 17:
			return D3DFMT_DXT1;
		case 18:
			return D3DFMT_DXT2;
		case 19:
			return D3DFMT_DXT3;
		case 20:
			return D3DFMT_DXT4;
		case 21:
			return D3DFMT_DXT5;
		case 0:
		default:
			return D3DFMT_UNKNOWN;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD s_toD3DBlend( EBlendFactor _blend )
	{
		switch( _blend )
		{
		case Menge::BF_ONE:
			return D3DBLEND_ONE;
		case Menge::BF_ZERO:
			return D3DBLEND_ZERO;
		case Menge::BF_DEST_COLOUR:
			return D3DBLEND_DESTCOLOR;
		case Menge::BF_SOURCE_COLOUR:
			return D3DBLEND_SRCCOLOR;
		case Menge::BF_ONE_MINUS_DEST_COLOUR:
			return D3DBLEND_INVDESTCOLOR;
		case Menge::BF_ONE_MINUS_SOURCE_COLOUR:
			return D3DBLEND_INVSRCCOLOR;
		case Menge::BF_DEST_ALPHA:
			return D3DBLEND_DESTALPHA;
		case Menge::BF_SOURCE_ALPHA:
			return D3DBLEND_SRCALPHA;
		case Menge::BF_ONE_MINUS_DEST_ALPHA:
			return D3DBLEND_INVDESTALPHA;
		case Menge::BF_ONE_MINUS_SOURCE_ALPHA:
			return D3DBLEND_INVSRCALPHA;
		}
		return D3DBLEND_ZERO;
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
	DX8RenderSystem::DX8RenderSystem()
		: m_logSystem( NULL )
		, m_pD3D( NULL )
		, m_pD3DDevice( NULL )
		, pVB( NULL )
		, pIB( NULL )
		, pVB3D( NULL )
		, pIB3D( NULL )
		, m_layer( 0.0f )
		, m_inRender( false )
#ifdef _DEBUG
		, m_clearColor( 255 )
#else
		, m_clearColor( 0 )
#endif
		, m_texFilter( true )
		, m_curRenderTexture( NULL )
		, m_syncTemp( NULL )
		, m_syncTempTex( NULL )
		, pScreenSurf( NULL )
		, pScreenDepth( NULL )
	{
		m_syncTargets[0] = NULL;
		m_syncTargets[1] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	DX8RenderSystem::~DX8RenderSystem()
	{
		gfx_done_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::initialize( LogSystemInterface* _logSystem )
	{
		m_logSystem = _logSystem;

		D3DADAPTER_IDENTIFIER8 AdID;
		D3DDISPLAYMODE Mode;
		D3DFORMAT Format = D3DFMT_UNKNOWN;
		UINT nModes, i;
		m_frames = 0;

		m_curTexture = NULL;

		// Init D3D
		log( "Initializing DX8RenderSystem..." );
		m_pD3D = Direct3DCreate8( D3D_SDK_VERSION ); // D3D_SDK_VERSION
		if( m_pD3D == NULL )
		{
			log_error( "Can't create D3D interface" );
			return false;
		}

		// Get adapter info
		m_pD3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, D3DENUM_NO_WHQL_LEVEL, &AdID );
		log( "D3D Driver: %s", AdID.Driver );
		log( "Description: %s", AdID.Description );
		log( "Version: %d.%d.%d.%d",
			HIWORD(AdID.DriverVersion.HighPart),
			LOWORD(AdID.DriverVersion.HighPart),
			HIWORD(AdID.DriverVersion.LowPart),
			LOWORD(AdID.DriverVersion.LowPart));

		// Set up Windowed presentation parameters
		if(FAILED( m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN ) 
		{
			log_error( "Can't determine desktop video mode" );
			return false;
		}
		UINT screenWidth = Mode.Width;
		UINT screenHeight = Mode.Height;

		ZeroMemory( &d3dppW, sizeof(d3dppW) );
		d3dppW.BackBufferFormat = Mode.Format;
		d3dppW.MultiSampleType  = D3DMULTISAMPLE_NONE;
		d3dppW.Windowed         = TRUE;
		d3dppW.Flags			= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		d3dppW.BackBufferCount  = 1;


		// Set up Full Screen presentation parameters
		nModes = m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT );

		//for(i=0; i<nModes; i++)
		//{
		//	m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &Mode);
		//	m_displayModes.push_back( Mode );
		//	m_resList.push_back( Mode.Width );
		//	m_resList.push_back( Mode.Height );
		//}

		for(i=0; i<nModes; i++)
		{
			//Mode = m_displayModes[i];
			m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, i, &Mode );
			if(Mode.Width != screenWidth || Mode.Height != screenHeight) continue;
			//if(nScreenBPP==16 && (_format_id(Mode.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
			if(format_id_(Mode.Format) > format_id_(Format)) Format=Mode.Format;
		}

		if(Format == D3DFMT_UNKNOWN)
		{
			log_error( "Can't find appropriate full screen video mode" );
			return false;
		}

		ZeroMemory( &d3dppFS, sizeof(d3dppFS) );
		d3dppFS.BackBufferFormat = Format;
		d3dppFS.BackBufferCount  = 1;
		d3dppFS.MultiSampleType  = D3DMULTISAMPLE_NONE;
		d3dppFS.Windowed         = FALSE;
		d3dppFS.SwapEffect       = D3DSWAPEFFECT_FLIP;
		d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dppFS.Flags			 = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		matIdent_( &m_matView );
		matIdent_( &m_matWorld );
		matIdent_( &m_matProj );
		matIdent_( &m_matTexture );

		D3DCAPS8 caps;
		m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );
		if( ( caps.TextureCaps & D3DPTEXTURECAPS_POW2 ) == 0 )
		{
			m_supportNPOT = true;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits, 
		bool _fullscreen, WindowHandle _winHandle,  int _FSAAType, int _FSAAQuality )
	{
		static const char *szFormats[]={ 
			"UNKNOWN"
			,"R5G6B5"
			,"X1R5G5B5"
			,"A1R5G5B5"
			,"X8R8G8B8"
			,"A8R8G8B8"
		};

		m_currentRenderTarget = "defaultCamera";

		m_screenResolution[0] = _width;
		m_screenResolution[1] = _height;
		m_fullscreen = _fullscreen;

		d3dppW.BackBufferWidth  = _width;
		d3dppW.BackBufferHeight = _height;
		d3dppW.hDeviceWindow    = (HWND)_winHandle;

		//d3dppW.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
		d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;

		d3dppW.EnableAutoDepthStencil = TRUE;
		d3dppW.AutoDepthStencilFormat = D3DFMT_D16;

		d3dppFS.BackBufferWidth  = _width;
		d3dppFS.BackBufferHeight = _height;
		d3dppFS.hDeviceWindow    = (HWND)_winHandle;


		//d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		d3dppFS.EnableAutoDepthStencil = TRUE;
		d3dppFS.AutoDepthStencilFormat = D3DFMT_D16;

		d3dpp = _fullscreen ? &d3dppFS : &d3dppW;

		if(format_id_(d3dpp->BackBufferFormat) < 4) m_screenBits = 16;
		else m_screenBits = 32;

		// Create D3D Device
		HRESULT hr;

		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)_winHandle,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			d3dpp, &m_pD3DDevice );

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)_winHandle,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				d3dpp, &m_pD3DDevice );
		}

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)_winHandle,
				D3DCREATE_MIXED_VERTEXPROCESSING,
				d3dpp, &m_pD3DDevice );

			if( FAILED( hr ) )
			{
				Sleep( 100 );
				hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)_winHandle,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					d3dpp, &m_pD3DDevice );
			}
		}

		if( FAILED ( hr ) )
		{
			log_error( "Can't create D3D device (hr:%d, hwnd:%d)", hr, (HWND)_winHandle );
			return false;
		}
		/*if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		d3dpp, &pD3DDevice ) ) )
		{
		_PostError("Can't create D3D device");
		return false;
		}*/

		//_AdjustWindow();

		log( "Mode: %d x %d x %s\n",_width,_height,szFormats[format_id_(d3dpp->BackBufferFormat)]);

		// Create vertex batch buffer
		VertArray = NULL;

		// Init all stuff that can be lost
		setProjectionMatrix_( _width, _height );

		m_renderTextureMap.insert( std::make_pair( 	m_currentRenderTarget, (DX8RenderTexture*)NULL ) );

		m_layer3D = false;	// starting with 2D
		if(!init_lost_()) 
		{
			return false;
		}

		clear( 0 );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::screenshot( RenderImageInterface* _image, const float * _rect )
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
			rect.right = m_screenResolution[0];
			rect.bottom = m_screenResolution[1];
		}
		DX8Texture* dxTexture = static_cast<DX8Texture*>( _image );

		LPDIRECT3DSURFACE8 surf;
		HRESULT hr = m_pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &surf );
		if( FAILED( hr ) )
		{
			log_error( "D3D Error: failed to GetBackBuffer" );
		}

		IDirect3DTexture8* dtext = dxTexture->getInterface();
		LPDIRECT3DSURFACE8 dsurf;
		D3DSURFACE_DESC desc;
		dtext->GetLevelDesc(0, &desc);
		hr = dtext->GetSurfaceLevel(0, &dsurf );
		if( FAILED( hr ) )
		{
			log_error( "D3D Error: failed to GetSurfaceLevel" );
		}

		RECT dest_rect;
		dest_rect.top = 0;
		dest_rect.left = 0;
		dest_rect.right = rect.right - rect.left;
		dest_rect.bottom = rect.bottom - rect.top;
		if( dest_rect.right > desc.Width )
		{
			dest_rect.right = desc.Width;
		}
		if( dest_rect.bottom > desc.Height )
		{
			dest_rect.bottom = desc.Height;
		}

		hr = loadSurfaceFromSurface_( dsurf, &dest_rect, surf, &rect );
		if( FAILED( hr ) )
		{
			log_error( "D3D Error: failed to loadSurfaceFromSurface_" );
		}
		surf->Release();
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::render()
	{
		// deprecated
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setContentResolution( const std::size_t * _resolution )
	{
		m_contentResolution[0] = _resolution[0];
		m_contentResolution[1] = _resolution[1];
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setProjectionMatrix( const float * _projection )
	{
		render_batch_( false );

		std::copy( _projection, _projection + 16, &(m_matProj._11) );
		m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setViewMatrix( const float * _view )
	{
		render_batch_( false );

		std::copy( _view, _view + 16, &(m_matView._11) );
		m_pD3DDevice->SetTransform( D3DTS_VIEW, &m_matView );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setWorldMatrix( const float * _world )
	{
		render_batch_( false );

		std::copy( _world, _world + 16, &(m_matWorld._11) );
		m_pD3DDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX8RenderSystem::createImage( const String & _name, std::size_t _width, std::size_t _height, PixelFormat _format )
	{
		if( _format == Menge::PF_R8G8B8 )
		{
			_format = Menge::PF_X8R8G8B8;
		}
		std::size_t tex_width = _width;
		std::size_t tex_height = _height;
		if( ( _width & ( _width - 1 ) ) != 0
			|| ( _height & ( _height - 1 ) ) != 0 )
		{
			bool npot = supportNPOT();
			if( npot == false )	// we're all gonna die
			{
				tex_width = s_firstPO2From( _width );
				tex_height = s_firstPO2From( _height );
			}
		}
		
		IDirect3DTexture8* dxTextureInterface = NULL;
		HRESULT hr = d3dCreateTexture_( tex_width, tex_height, 1, 0, 
			s_toD3DFormat( _format ), D3DPOOL_MANAGED, &dxTextureInterface );

		if( FAILED( hr ) )
		{
			log_error( "DX8RenderSystem: can't create texture %s %dx%d", _name.c_str(), _width, _height );
			return NULL;
		}

		DX8Texture* dxTexture = new DX8Texture( dxTextureInterface, _name, _width, _height, _format );

		m_textureMap.insert( std::make_pair( _name, dxTexture) );
		dxTexture->incref();

		return dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX8RenderSystem::createRenderTargetImage( const String & _name, std::size_t _width, std::size_t _height )
	{
		std::size_t tex_width = _width;
		std::size_t tex_height = _height;
		if( ( _width & ( _width - 1 ) ) != 0
			|| ( _height & ( _height - 1 ) ) != 0 )
		{
			bool npot = supportNPOT();
			if( npot == false )	// we're all gonna die
			{
				tex_width = s_firstPO2From( _width );
				tex_height = s_firstPO2From( _height );
			}
		}
		D3DFORMAT fmt = d3dpp->BackBufferFormat;

		IDirect3DTexture8* dxTextureInterface = NULL;
		HRESULT hr = d3dCreateTexture_( tex_width, tex_height, 1, D3DUSAGE_RENDERTARGET,
			fmt, D3DPOOL_DEFAULT, &dxTextureInterface );

		if( FAILED( hr ) )
		{
			log_error( "DX8RenderSystem: can't create RenderTexture %s %dx%d", _name.c_str(), _width, _height );
			return NULL;
		}

		IDirect3DSurface8* depthSurface = NULL;

		hr = m_pD3DDevice->CreateDepthStencilSurface( tex_width, tex_height,
			D3DFMT_D16, D3DMULTISAMPLE_NONE, &depthSurface );
		if( FAILED( hr ) )
		{   
			dxTextureInterface->Release();
			log_error( "Can't create render target depth buffer" );
			return NULL;
		}

		DX8RenderTexture* dxRenderTexture = new DX8RenderTexture( dxTextureInterface, depthSurface,
			_name, _width, _height, PF_X8R8G8B8 );

		m_renderTextureMap.insert( std::make_pair( _name, dxRenderTexture ) );
		dxRenderTexture->incref();

		return dxRenderTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX8RenderSystem::loadImage( const String& _name, ImageDecoderInterface* _imageDecoder )
	{
		const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( _imageDecoder->getCodecDataInfo() );
		RenderImageInterface* renderImage = createImage( _name, dataInfo->width, dataInfo->height, dataInfo->format );
		DX8Texture* texture = static_cast<DX8Texture*>( renderImage );
		if( texture == NULL )
		{
			return NULL;
		}
		
		texture->loadData( _imageDecoder );

		return renderImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::releaseImage( RenderImageInterface * _image )
	{
		DX8Texture* dxTexture = static_cast<DX8Texture*>( _image );
		if( dxTexture == NULL )
		{
			return;
		}

		TTextureMap::iterator it = m_textureMap.find( dxTexture->getDescription() );
		if( it != m_textureMap.end() )
		{
			if( dxTexture->decref() == 0 )
			{
				IDirect3DTexture8* pTex = dxTexture->getInterface();
				pTex->Release();
				m_textureMap.erase( it );
				delete dxTexture;
			}
			return;
		}

		TRenderTextureMap::iterator rit = m_renderTextureMap.find( dxTexture->getDescription() );
		if( rit != m_renderTextureMap.end() )
		{
			if( rit->second == NULL )
			{
				return;
			}
			DX8RenderTexture* rtextrue = (DX8RenderTexture*)dxTexture;
			if( rtextrue->decref() == 0 )
			{
				rtextrue->getInterface()->Release();
				rtextrue->getDepthInterface()->Release();
				m_renderTextureMap.erase( rit );
				delete rtextrue;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX8RenderSystem::getImage( const String& _desc ) const 
	{
		TTextureMap::const_iterator it = m_textureMap.find( _desc );
		if( it == m_textureMap.end() )
		{
			return NULL;
		}
		it->second->incref();
		return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::renderImage( const float * _renderVertex,
		const float * _uv,
		unsigned int _color,
		const RenderImageInterface * _image,
		EBlendFactor _srcBlend,
		EBlendFactor _dstBlend )
	{
		TQuad quad;

		quad.v[0].pos[0] = _renderVertex[0];
		quad.v[0].pos[1] = _renderVertex[1];
		quad.v[0].pos[2] = m_layer;
		quad.v[0].col = _color;

		quad.v[1].pos[0] = _renderVertex[2];
		quad.v[1].pos[1] = _renderVertex[3];
		quad.v[1].pos[2] = m_layer;
		quad.v[1].col = _color;

		quad.v[2].pos[0] = _renderVertex[4];
		quad.v[2].pos[1] = _renderVertex[5];
		quad.v[2].pos[2] = m_layer;
		quad.v[2].col = _color;

		quad.v[3].pos[0] = _renderVertex[6];
		quad.v[3].pos[1] = _renderVertex[7];
		quad.v[3].pos[2] = m_layer;
		quad.v[3].col = _color;

		for( int i = 0; i < 4; i++ )
		{
			quad.v[i].pos[0] = ::floorf( quad.v[i].pos[0] + 0.5f );
			quad.v[i].pos[1] = ::floorf( quad.v[i].pos[1] + 0.5f );

			quad.v[i].n[0] = 0.0f;
			quad.v[i].n[1] = 0.0f;
			quad.v[i].n[2] = 1.0f;
		}

		quad.v[0].uv[0] = _uv[0];
		quad.v[0].uv[1] = _uv[1];
		quad.v[1].uv[0] = _uv[2];
		quad.v[1].uv[1] = _uv[1];
		quad.v[2].uv[0] = _uv[2];
		quad.v[2].uv[1] = _uv[3];
		quad.v[3].uv[0] = _uv[0]; 
		quad.v[3].uv[1] = _uv[3];

		DX8Texture* tex = (DX8Texture*)( _image );
		if( tex )
		{
			const float* uvMask = tex->getUVMask();

			for( int i = 0; i < 4; i++ )
			{
				quad.v[i].uv[0] *= uvMask[0];
				quad.v[i].uv[1] *= uvMask[1];
			}

			quad.tex = tex->getInterface();
		}
		else
		{
			quad.tex = NULL;
		}

		quad.srcBlend = _srcBlend;
		quad.dstBlend = _dstBlend;

		render_quad_( &quad );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::renderTriple( const float * _a,
		const float * _b,
		const float * _c,
		const float * _uv0,
		const float * _uv1,
		const float * _uv2,
		unsigned int _color,
		const RenderImageInterface * _image,
		EBlendFactor _srcBlend, EBlendFactor _dstBlend )
	{
		TTriple triangle;

		triangle.v[0].pos[0] = _a[0];
		triangle.v[0].pos[1] = _a[1];
		triangle.v[0].pos[2] = m_layer;
		triangle.v[0].col = _color;

		triangle.v[1].pos[0] = _b[0];
		triangle.v[1].pos[1] = _b[1];
		triangle.v[1].pos[2] = m_layer;
		triangle.v[1].col = _color;

		triangle.v[2].pos[0] = _c[0];
		triangle.v[2].pos[1] = _c[1];
		triangle.v[2].pos[2] = m_layer;
		triangle.v[2].col = _color;

		for( int i = 0; i < 3; i++ )
		{
			triangle.v[i].pos[0] = ::floorf( triangle.v[i].pos[0] + 0.5f );
			triangle.v[i].pos[1] = ::floorf( triangle.v[i].pos[1] + 0.5f );

			triangle.v[i].n[0] = 0.0f;
			triangle.v[i].n[1] = 0.0f;
			triangle.v[i].n[2] = 1.0f;
		}

		triangle.srcBlend = _srcBlend;
		triangle.dstBlend = _dstBlend;

		triangle.v[0].uv[0] = _uv0[0];
		triangle.v[0].uv[1] = _uv0[1];

		triangle.v[1].uv[0] = _uv1[0];
		triangle.v[1].uv[1] = _uv1[1];

		triangle.v[2].uv[0] = _uv2[0];
		triangle.v[2].uv[1] = _uv2[1];

		DX8Texture* tex = (DX8Texture*)( _image );
		if( tex )
		{
			const float* uvMask = tex->getUVMask();

			for( int i = 0; i < 3; i++ )
			{
				triangle.v[i].uv[0] *= uvMask[0];
				triangle.v[i].uv[1] *= uvMask[1];
			}

			triangle.tex = tex->getInterface();
		}
		else
		{
			triangle.tex = NULL;
		}

		render_triple_( &triangle );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::renderMesh( const TVertex* _vertices,
		std::size_t _verticesNum,
		const uint16* _indices,
		std::size_t _indicesNum,
		TMaterial* _material )
	{
		IDirect3DTexture8* dxTexture = NULL;
		DX8Texture* texture	= NULL;
		D3DMATRIX mtex;
		if( _material )
		{
			texture = static_cast<DX8Texture*>( _material->texture );
			if( texture != NULL )
			{
				matIdent_( &mtex );
				mtex._11 = texture->getUVMask()[0];
				mtex._22 = texture->getUVMask()[1];
				setTextureMatrix( &mtex._11 );
				dxTexture = texture->getInterface();
			}
		}


		HRESULT hr;
		BYTE* vertexData = 0;
		const BYTE* dstData = reinterpret_cast<const BYTE*>( &(_vertices[0]) );
		hr = pVB3D->Lock( 0, 0, &vertexData, 0 );
		std::copy( _vertices, _vertices + _verticesNum, (TVertex *)vertexData );
		hr = pVB3D->Unlock();

		//const BYTE* dstIData = reinterpret_cast<const BYTE*>( &(_indices[0]) );
		hr = pIB3D->Lock( 0, 0, &vertexData, 0 );
		std::copy( _indices, _indices + _indicesNum, (unsigned short *)vertexData );
		hr = pIB3D->Unlock();

		hr = m_pD3DDevice->SetTexture( 0, dxTexture );
		m_curTexture = dxTexture;

		hr = m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, _verticesNum, 0, _indicesNum / 3 );

		matIdent_( &mtex );
		setTextureMatrix( &mtex._11 );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::renderLine( unsigned int _color, const float * _begin, const float * _end )
	{
		if( VertArray )
		{
			if( CurPrimType != MENGE_PRIM_LINES 
				|| nPrim >= VERTEX_BUFFER_SIZE/MENGE_PRIM_LINES 
				|| m_curTexture )
				//|| m_currSrcBlend != BF_DEST_ALPHA )
			{
				render_batch_( false );

				CurPrimType=MENGE_PRIM_LINES;
				//if(CurBlendMode != BLEND_DEFAULT) _SetBlendMode(BLEND_DEFAULT);
				if( m_curTexture != NULL ) 
				{ 
					m_pD3DDevice->SetTexture( 0, NULL );
					m_curTexture = NULL;
				}
			}

			int i = nPrim * MENGE_PRIM_LINES;
			VertArray[i].pos[0] = _begin[0]; VertArray[i+1].pos[0] = _end[0];
			VertArray[i].pos[1] = _begin[1]; VertArray[i+1].pos[1] = _end[1];
			VertArray[i].pos[2]     = VertArray[i+1].pos[2] = m_layer;
			VertArray[i].col   = VertArray[i+1].col = _color;
			VertArray[i].uv[0]    = VertArray[i+1].uv[0] =
			VertArray[i].uv[1]    = VertArray[i+1].uv[1] = 0.0f;

			VertArray[i].n[0] = VertArray[i].n[1] = VertArray[i+1].n[0] = VertArray[i+1].n[1] = 0.0f;
			VertArray[i].n[2] = VertArray[i+1].n[2] = 1.0f;

			nPrim++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::beginScene()
	{
		m_layer = 1.0f;
		if( m_inRender )
		{
			render_batch_( true );
			m_pD3DDevice->EndScene();
		}
		if( begin_scene_() == false )
		{
			log_error( "Error: D3D8 Failed to BeginScene" );
		}
		set_clipping_();
		clear( m_clearColor );

		m_inRender = true;
		m_currentRenderTarget = "defaultCamera";

		// set render targets dirty to clear one time before rendering into one
		for( TRenderTextureMap::iterator it = m_renderTextureMap.begin(), it_end = m_renderTextureMap.end();
			it != it_end;
			it++ )
		{
			if( it->second == NULL ) continue;

			it->second->setDirty( true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::endScene()
	{
		render_batch_( true );
		m_pD3DDevice->EndScene();

		m_inRender = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::swapBuffers()
	{
		// sync GPU with CPU
		syncCPU_();

		HRESULT hr = m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
		if( FAILED( hr ) )
		{
			log_error( "Error: D3D8 failed to swap buffers" );
		}
		m_frames++;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::beginLayer2D()
	{
		if( m_layer3D == false ) return;	// already 2D
		prepare2D_();
		m_layer3D = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::endLayer2D()
	{
		m_layer -= 0.001f;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::beginLayer3D()
	{
		if( m_layer3D ) return;	// already 3D
		prepare3D_();
		m_layer3D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::endLayer3D()
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setRenderArea( const float* _renderArea )
	{
		float w = _renderArea[2] - _renderArea[0];
		float h = _renderArea[3] - _renderArea[1];

		set_clipping_( (int)_renderArea[0], (int)_renderArea[1], (int)w, (int)h );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		m_fullscreen = _fullscreen;
		d3dpp = _fullscreen ? &d3dppFS : &d3dppW;
		d3dpp->BackBufferWidth = _width;
		d3dpp->BackBufferHeight = _height;

		m_screenResolution[0] = _width;
		m_screenResolution[1]= _height;
		//nScreenBPP = _bpp;

		if( !gfx_restore_() )
		{
			log_error( "Error: Graphics change mode failed\n" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setRenderTarget( const String& _name, bool _clear )
	{
		TRenderTextureMap::iterator it = m_renderTextureMap.find( _name );
		if( it != m_renderTextureMap.end() )
		{
			if( m_inRender )
			{
				render_batch_( true );
				m_pD3DDevice->EndScene();
			}
			else
			{
				m_inRender = true;
			}
			m_currentRenderTarget = _name;
			begin_scene_( it->second );
			if( it->second == NULL ) return;
			if( it->second->isDirty() && _clear )
			{
				clear( m_clearColor );
				it->second->setDirty( false );
			}

		}
		else
		{
			log_error( "Warning: Invalid Render Target name \"%s\""
				, _name.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CameraInterface * DX8RenderSystem::createCamera( const String & _name )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	EntityInterface * DX8RenderSystem::createEntity( const String & _name, const String & _meshName )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * DX8RenderSystem::createLight( const String & _name )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::releaseCamera( CameraInterface * _camera )
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::releaseEntity( EntityInterface * _entity )
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::releaseLight( LightInterface * _light )
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureFiltering( bool _filter )
	{
		m_texFilter = _filter;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::onWindowMovedOrResized()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::onWindowClose()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	int DX8RenderSystem::getNumDIP() const 
	{
		return NumDips;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::renderText( const String & _text, const float * _pos, unsigned long _color )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::log( const char* _message, ... )
	{
		if( m_logSystem == NULL )
		{
			return;
		}

		va_list argList;

		va_start(argList, _message);

		char str[1024];

		vsnprintf( str, 1024, _message, argList );

		va_end(argList);

		String message( str );
		message += '\n';

		m_logSystem->logMessage( message, LM_LOG );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::log_error( const char* _message, ... )
	{
		if( m_logSystem == NULL )
		{
			return;
		}

		va_list argList;

		va_start(argList, _message);

		char str[1024];

		vsnprintf( str, 1024, _message, argList );

		va_end(argList);

		String message( str );
		message += '\n';

		m_logSystem->logMessage( message, LM_ERROR );
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::supportNPOT() const
	{
		return m_supportNPOT;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::syncCPU_()
	{
		//_render_batch( false );
		m_pD3DDevice->BeginScene();

		m_pD3DDevice->SetRenderTarget( m_syncTargets[m_frames % 2], 0 );
		//m_syncTargets[m_frames % 2]->Release();
		setProjectionMatrix_( 2, 2 );
		m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
		matIdent_(&m_matView);
		m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_matView);

		TQuad quad;
		quad.srcBlend = BF_SOURCE_ALPHA;
		quad.dstBlend = BF_ONE_MINUS_SOURCE_ALPHA;
		quad.tex = NULL;
		quad.v[0].col = 0;
		quad.v[0].pos[0] = 0.0f;
		quad.v[0].pos[1] = 0.0f;
		quad.v[0].pos[2] = 0.0f;
		quad.v[1].col = 0;
		quad.v[1].pos[0] = 2.0f;
		quad.v[1].pos[1] = 0.0f;
		quad.v[1].pos[2] = 0.0f;
		quad.v[2].col = 0;
		quad.v[2].pos[0] = 2.0f;
		quad.v[2].pos[1] = 2.0f;
		quad.v[2].pos[2] = 0.0f;
		quad.v[3].col = 0;
		quad.v[3].pos[0] = 0.0f;
		quad.v[3].pos[1] = 2.0f;
		quad.v[3].pos[2] = 0.0f;
		render_quad_( &quad );
		render_batch_( false );

		if( m_curRenderTexture )
		{
			LPDIRECT3DSURFACE8 surf;
			m_curRenderTexture->getInterface()->GetSurfaceLevel( 0, &surf );
			m_pD3DDevice->SetRenderTarget( surf, m_curRenderTexture->getDepthInterface());
			surf->Release();
			setProjectionMatrix_( m_curRenderTexture->getWidth(), m_curRenderTexture->getHeight() );
		}
		else
		{
			m_pD3DDevice->SetRenderTarget( pScreenSurf, pScreenDepth );
			setProjectionMatrix_( m_screenResolution[0], m_screenResolution[1] );
		}

		m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
		matIdent_(&m_matView);
		m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_matView);

		HRESULT hr = loadSurfaceFromSurface_( m_syncTemp, NULL, m_syncTargets[(m_frames + 1) % 2], NULL );
		D3DLOCKED_RECT rect;
		hr = m_syncTemp->LockRect( &rect, NULL, D3DLOCK_READONLY );
		m_syncTemp->UnlockRect();

		m_pD3DDevice->EndScene();
	}
	//////////////////////////////////////////////////////////////////////////
	int DX8RenderSystem::format_id_( D3DFORMAT _format )
	{
		switch(_format) {
		case D3DFMT_R5G6B5:		return 1;
		case D3DFMT_X1R5G5B5:	return 2;
		case D3DFMT_A1R5G5B5:	return 3;
		case D3DFMT_X8R8G8B8:	return 4;
		case D3DFMT_A8R8G8B8:	return 5;
		default:				return 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::matIdent_( D3DMATRIX* _mtx )
	{
		_mtx->_12 = _mtx->_13 = _mtx->_14 = 
		_mtx->_21 = _mtx->_23 = _mtx->_24 = 
		_mtx->_31 = _mtx->_32 = _mtx->_34 = 
		_mtx->_41 = _mtx->_42 = _mtx->_43 = 0.0f;
		_mtx->_11 = _mtx->_22 = _mtx->_33 = _mtx->_44 = 1.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::matMul_( D3DMATRIX* _out, D3DMATRIX* _mtxl, D3DMATRIX* _mtxr )
	{
		_out->_11 = _mtxl->_11 * _mtxr->_11 + _mtxl->_12 * _mtxr->_21 + _mtxl->_13 * _mtxr->_31 + _mtxl->_14 * _mtxr->_41;
		_out->_12 = _mtxl->_11 * _mtxr->_12 + _mtxl->_12 * _mtxr->_22 + _mtxl->_13 * _mtxr->_32 + _mtxl->_14 * _mtxr->_42;
		_out->_13 = _mtxl->_11 * _mtxr->_13 + _mtxl->_12 * _mtxr->_23 + _mtxl->_13 * _mtxr->_33 + _mtxl->_14 * _mtxr->_43;
		_out->_14 = _mtxl->_11 * _mtxr->_14 + _mtxl->_12 * _mtxr->_24 + _mtxl->_13 * _mtxr->_34 + _mtxl->_14 * _mtxr->_44;
		_out->_21 = _mtxl->_21 * _mtxr->_11 + _mtxl->_22 * _mtxr->_21 + _mtxl->_23 * _mtxr->_31 + _mtxl->_24 * _mtxr->_41;
		_out->_22 = _mtxl->_21 * _mtxr->_12 + _mtxl->_22 * _mtxr->_22 + _mtxl->_23 * _mtxr->_32 + _mtxl->_24 * _mtxr->_42;
		_out->_23 = _mtxl->_21 * _mtxr->_13 + _mtxl->_22 * _mtxr->_23 + _mtxl->_23 * _mtxr->_33 + _mtxl->_24 * _mtxr->_43;
		_out->_24 = _mtxl->_21 * _mtxr->_14 + _mtxl->_22 * _mtxr->_24 + _mtxl->_23 * _mtxr->_34 + _mtxl->_24 * _mtxr->_44;
		_out->_31 = _mtxl->_31 * _mtxr->_11 + _mtxl->_32 * _mtxr->_21 + _mtxl->_33 * _mtxr->_31 + _mtxl->_34 * _mtxr->_41;
		_out->_32 = _mtxl->_31 * _mtxr->_12 + _mtxl->_32 * _mtxr->_22 + _mtxl->_33 * _mtxr->_32 + _mtxl->_34 * _mtxr->_42;
		_out->_33 = _mtxl->_31 * _mtxr->_13 + _mtxl->_32 * _mtxr->_23 + _mtxl->_33 * _mtxr->_33 + _mtxl->_34 * _mtxr->_43;
		_out->_34 = _mtxl->_31 * _mtxr->_14 + _mtxl->_32 * _mtxr->_24 + _mtxl->_33 * _mtxr->_34 + _mtxl->_34 * _mtxr->_44;
		_out->_41 = _mtxl->_41 * _mtxr->_11 + _mtxl->_42 * _mtxr->_21 + _mtxl->_43 * _mtxr->_31 + _mtxl->_44 * _mtxr->_41;
		_out->_42 = _mtxl->_41 * _mtxr->_12 + _mtxl->_42 * _mtxr->_22 + _mtxl->_43 * _mtxr->_32 + _mtxl->_44 * _mtxr->_42;
		_out->_43 = _mtxl->_41 * _mtxr->_13 + _mtxl->_42 * _mtxr->_23 + _mtxl->_43 * _mtxr->_33 + _mtxl->_44 * _mtxr->_43;
		_out->_44 = _mtxl->_41 * _mtxr->_14 + _mtxl->_42 * _mtxr->_24 + _mtxl->_43 * _mtxr->_34 + _mtxl->_44 * _mtxr->_44;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::matOrthoOffCenterLH_( D3DMATRIX* _out,  float l, float r, float b, float t, float zn, float zf )
	{
		_out->_12 = _out->_13 = _out->_14 = _out->_21 = _out->_23 = _out->_24 =
		_out->_31 = _out->_32 = _out->_34 = 0.0f;

		float inv_lr = 1.0f / ( l - r );
		float inv_bt = 1.0f / ( b - t );
		float inv_znzf = 1.0f / ( zn - zf );
		_out->_11 = -2.0f * inv_lr;
		_out->_22 = -2.0f * inv_bt;
		_out->_33 = -1.0f * inv_znzf;
		_out->_41 = ( l + r ) * inv_lr;
		_out->_42 = ( t + b ) * inv_bt;
		_out->_43 = zn * inv_znzf;
		_out->_44 = 1.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setProjectionMatrix_( std::size_t _width, std::size_t _height )
	{
		D3DMATRIX tmp;
		//D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
		matIdent_( &m_matProj );
		m_matProj._11 = m_matProj._33 = m_matProj._44 = 1.0f;
		m_matProj._22 = -1.0f;

		//D3DXMatrixTranslation(&tmp, -0.5f, height+0.5f, 0.0f);
		matIdent_( &tmp );
		tmp._41 = -0.5f; tmp._42 = _height + 0.5f; tmp._43 = 0.0f;

		matMul_(&m_matProj, &m_matProj, &tmp);
		matOrthoOffCenterLH_(&tmp, 0.0f, (float)_width, 0.0f, (float)_height, 0.0f, 1.0f);
		matMul_(&m_matProj, &m_matProj, &tmp);
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::init_lost_()
	{
		// Store render target
		pScreenSurf = NULL;
		pScreenDepth = NULL;

		m_pD3DDevice->GetRenderTarget(&pScreenSurf);
		m_pD3DDevice->GetDepthStencilSurface(&pScreenDepth);

		for( TRenderTextureMap::iterator it = m_renderTextureMap.begin(), it_end = m_renderTextureMap.end();
			it != it_end;
			it++ )
		{
			if( it->second == NULL ) continue;

			IDirect3DTexture8* d3dTexInterface = it->second->getInterface();
			IDirect3DSurface8* depthInterface = it->second->getDepthInterface();
			if( d3dTexInterface != NULL )
			{
				d3dCreateTexture_( it->second->getWidth(), it->second->getHeight(), 1, D3DUSAGE_RENDERTARGET,
					d3dpp->BackBufferFormat, D3DPOOL_DEFAULT, &d3dTexInterface );
				it->second->setTexInterface( d3dTexInterface );
			}
			if( depthInterface != NULL )
			{
				m_pD3DDevice->CreateDepthStencilSurface( it->second->getWidth(), it->second->getHeight(),
					D3DFMT_D16, D3DMULTISAMPLE_NONE, &depthInterface );
				it->second->setDepthInterface( depthInterface );
			}
		}

		// sync surfaces
		D3DFORMAT fmt = D3DFMT_X8R8G8B8;
		UINT w = 2;
		UINT d = 1;
		//D3DXCheckTextureRequirements( pD3DDevice, &w, &w, &d, D3DUSAGE_RENDERTARGET, &fmt, D3DPOOL_DEFAULT );
		m_pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, TRUE, &(m_syncTargets[0]) );
		m_pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, TRUE, &(m_syncTargets[1]) );
		d3dCreateTexture_( w, w, d, 0, fmt, D3DPOOL_SYSTEMMEM, &m_syncTempTex );
		m_syncTempTex->GetSurfaceLevel( 0, &m_syncTemp );

		// Create Vertex buffer

		if( FAILED (m_pD3DDevice->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(TVertex),
			D3DUSAGE_WRITEONLY,
			D3DFVF_MENGE_VERTEX,
			D3DPOOL_DEFAULT, &pVB )))
		{
			log_error( "Can't create D3D vertex buffer" );
			return false;
		}

		if( FAILED (m_pD3DDevice->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(TVertex),
			D3DUSAGE_WRITEONLY,
			D3DFVF_MENGE_VERTEX,
			D3DPOOL_DEFAULT, &pVB3D )))
		{
			log_error( "Can't create Menge D3D vertex buffer" );
			return false;
		}

		//pD3DDevice->SetVertexShader( D3DFVF_HGEVERTEX );
		//pD3DDevice->SetStreamSource( 0, pVB, sizeof(hgeVertex) );

		// Create and setup Index buffer

		if( FAILED( m_pD3DDevice->CreateIndexBuffer(VERTEX_BUFFER_SIZE*6/4*sizeof(WORD),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_DEFAULT, &pIB ) ) )
		{
			log_error( "Can't create D3D index buffer" );
			return false;
		}

		WORD *pIndices, n=0;
		if( FAILED( pIB->Lock( 0, 0, (BYTE**)&pIndices, 0 ) ) )
		{
			log_error( "Can't lock D3D index buffer" );
			return false;
		}

		for(int i=0; i<VERTEX_BUFFER_SIZE/4; i++) {
			*pIndices++=n;
			*pIndices++=n+1;
			*pIndices++=n+2;
			*pIndices++=n+2;
			*pIndices++=n+3;
			*pIndices++=n;
			n+=4;
		}

		pIB->Unlock();
		//pD3DDevice->SetIndices(pIB,0);

		if( FAILED( m_pD3DDevice->CreateIndexBuffer(VERTEX_BUFFER_SIZE*sizeof(WORD),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_DEFAULT, &pIB3D ) ) )
		{
			log_error( "Can't create Menge D3D index buffer" );
			return false;
		}

		// Set common render states
		if( m_layer3D )
		{
			prepare3D_();
		}
		else
		{
			prepare2D_();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::render_batch_( bool _endScene )
	{
		HRESULT hr;
		if( VertArray )
		{
			pVB->Unlock();

			if(nPrim)
			{
				switch(CurPrimType)
				{
				case MENGE_PRIM_QUADS:
					hr = m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, nPrim<<2, 0, nPrim<<1 );
					NumDips++;
					if( FAILED( hr ) )
					{
						log_error( "ERROR!!!" );
					}
					break;

				case MENGE_PRIM_TRIPLES:
					//добавил сет рендер стейт
					//DWORD state;
					//pD3DDevice->GetRenderState(D3DRS_WRAP0,&state);
					//pD3DDevice->SetRenderState(D3DRS_WRAP0,D3DWRAP_U | D3DWRAP_V);
					m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
					m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
					//
					m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nPrim );

					m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
					m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

					//
					//pD3DDevice->SetRenderState(D3DRS_WRAP0,state);
					break;

				case MENGE_PRIM_LINES:
					m_pD3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, nPrim );
					break;
				}

				nPrim = 0;
			}

			if( _endScene )
			{
				VertArray = NULL;
			}
			else 
			{
				pVB->Lock( 0, 0, (BYTE**)&VertArray, 0 );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::prepare2D_()
	{
		render_batch_( false );

		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

		m_pD3DDevice->SetVertexShader( D3DFVF_MENGE_VERTEX );
		m_pD3DDevice->SetStreamSource( 0, pVB, sizeof(TVertex) );
		m_pD3DDevice->SetIndices(pIB,0);

		///
		m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		//Gfx_SetBlendState( BLEND_SRCALPHA, BLEND_INVSRCALPHA );

		m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF,        0x01 );
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		m_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
		//pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER );
		//pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER );

		if( m_texFilter == true )
		{
			m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		}
		else
		{
			m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
			m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
		}

		nPrim = 0;
		CurPrimType = MENGE_PRIM_QUADS;
		//CurBlendMode = BLEND_DEFAULT;
		m_currSrcBlend = BF_SOURCE_ALPHA;
		m_currDstBlend = BF_ONE_MINUS_SOURCE_ALPHA;
		m_curTexture = NULL;

		//D3DXMatrixIdentity( &matView );
		matIdent_( &m_matWorld );
		//_SetProjectionMatrix( nScreenWidth, nScreenHeight );
		m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
		m_pD3DDevice->SetTransform( D3DTS_VIEW, &m_matView );
		m_pD3DDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

		m_layer3D = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::prepare3D_()
	{
		render_batch_( false );

		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

		m_pD3DDevice->SetStreamSource( 0, pVB3D, sizeof(TVertex) );
		m_pD3DDevice->SetVertexShader( D3DFVF_MENGE_VERTEX );
		m_pD3DDevice->SetIndices( pIB3D, 0 );
		m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

		m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

		m_layer3D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	HRESULT DX8RenderSystem::d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,  DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8 * ppTexture )
	{
		return m_pD3DDevice->CreateTexture( Width, Height, MipLevels, Usage, Format, Pool, ppTexture );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::clear( DWORD _color )
	{
		//pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		if( m_curRenderTexture )
		{
			if( m_curRenderTexture->getDepthInterface() )
			{
				m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
			}
			else
			{
				m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, _color, 1.0f, 0 );
			}
		}
		else
		{
			//if( bZBuffer )
			{
				m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
			}
			/*else
			{
				m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
			}*/
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureMatrix( const float* _texture )
	{
		std::copy( _texture, _texture + 16, &(m_matTexture._11) );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		m_pD3DDevice->SetTransform( D3DTS_TEXTURE0, &m_matTexture );
	}
	//////////////////////////////////////////////////////////////////////////
	HRESULT DX8RenderSystem::loadSurfaceFromSurface_( LPDIRECT3DSURFACE8 pDestSurface, CONST RECT * pDestRect,  LPDIRECT3DSURFACE8 pSrcSurface, CONST RECT * pSrcRect )
	{
		D3DLOCKED_RECT dstLockedRect;
		D3DLOCKED_RECT srcLockedRect;
		RECT dstRect;
		RECT srcRect;
		UINT srcWidth = 0, srcHeight = 0, dstWidth = 0, dstHeight = 0;

		if( pDestRect != NULL )
		{
			dstWidth = pDestRect->right - pDestRect->left;
			dstHeight = pDestRect->bottom - pDestRect->top;
			dstRect = *pDestRect;
		}
		else
		{
			D3DSURFACE_DESC desc;
			HRESULT hr = pDestSurface->GetDesc( &desc );
			if( FAILED( hr ) )
			{
				return hr;
			}
			dstWidth = desc.Width;
			dstHeight = desc.Height;
			dstRect.left = 0;
			dstRect.top = 0;
			dstRect.right = dstWidth;
			dstRect.bottom = dstHeight;
		}

		if( pSrcRect != NULL )
		{
			srcWidth = pSrcRect->right - pSrcRect->left;
			srcHeight = pSrcRect->bottom - pSrcRect->top;
			srcRect = *pSrcRect;
		}
		else
		{
			D3DSURFACE_DESC desc;
			HRESULT hr = pSrcSurface->GetDesc( &desc );
			if( FAILED( hr ) )
			{
				return hr;
			}
			srcWidth = desc.Width;
			srcHeight = desc.Height;
			srcRect.left = 0;
			srcRect.top = 0;
			srcRect.right = srcWidth;
			srcRect.bottom = srcHeight;
		}


		HRESULT hr = pDestSurface->LockRect( &dstLockedRect, pDestRect, 0 );
		if( FAILED( hr ) )
		{
			return hr;
		}
		hr = pSrcSurface->LockRect( &srcLockedRect, pSrcRect, D3DLOCK_READONLY );
		if( FAILED( hr ) )
		{
			pDestSurface->UnlockRect();
			return hr;
		}

		// resampler
		// only optimized for 2D
		// srcdata stays at beginning of slice, pdst is a moving pointer
		unsigned char* srcdata = (unsigned char*)srcLockedRect.pBits;
		unsigned char* pdst = (unsigned char*)dstLockedRect.pBits;
		int channels = 4;
		UINT srcRowPitch = srcLockedRect.Pitch / channels;
		UINT dstRowSkip = dstLockedRect.Pitch / channels - dstWidth;
		// sx_48,sy_48 represent current position in source
		// using 16/48-bit fixed precision, incremented by steps
		uint64 stepx = ((uint64)srcWidth << 48) / dstWidth;
		uint64 stepy = ((uint64)srcHeight << 48) / dstHeight;

		// bottom 28 bits of temp are 16/12 bit fixed precision, used to
		// adjust a source coordinate backwards by half a pixel so that the
		// integer bits represent the first sample (eg, sx1) and the
		// fractional bits are the blend weight of the second sample
		unsigned int temp;

		uint64 sy_48 = (stepy >> 1) - 1;
		for (size_t y = dstRect.top; y < dstRect.bottom; y++, sy_48+=stepy )
		{
			temp = sy_48 >> 36;
			temp = (temp > 0x800)? temp - 0x800: 0;
			unsigned int syf = temp & 0xFFF;
			size_t sy1 = temp >> 12;
			size_t sy2 = (std::min<size_t>)(sy1+1, srcRect.bottom-srcRect.top-1);
			size_t syoff1 = sy1 * srcRowPitch;
			size_t syoff2 = sy2 * srcRowPitch;

			uint64 sx_48 = (stepx >> 1) - 1;
			for (size_t x = dstRect.left; x < dstRect.right; x++, sx_48+=stepx)
			{
				temp = sx_48 >> 36;
				temp = (temp > 0x800)? temp - 0x800 : 0;
				unsigned int sxf = temp & 0xFFF;
				size_t sx1 = temp >> 12;
				size_t sx2 = (std::min<size_t>)(sx1+1, srcRect.right-srcRect.left-1);

				unsigned int sxfsyf = sxf*syf;
				for (unsigned int k = 0; k < channels; k++) 
				{
					unsigned int accum =
						srcdata[(sx1 + syoff1)*channels+k]*(0x1000000-(sxf<<12)-(syf<<12)+sxfsyf) +
						srcdata[(sx2 + syoff1)*channels+k]*((sxf<<12)-sxfsyf) +
						srcdata[(sx1 + syoff2)*channels+k]*((syf<<12)-sxfsyf) +
						srcdata[(sx2 + syoff2)*channels+k]*sxfsyf;
					// accum is computed using 8/24-bit fixed-point math
					// (maximum is 0xFF000000; rounding will not cause overflow)
					*pdst++ = (accum + 0x800000) >> 24;
				}
			}
			pdst += channels*dstRowSkip;
		}

		pSrcSurface->UnlockRect();
		pDestSurface->UnlockRect();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::render_quad_( TQuad* _quad )
	{
		if(VertArray)
		{
			if( CurPrimType!=MENGE_PRIM_QUADS 
				|| nPrim>=VERTEX_BUFFER_SIZE/MENGE_PRIM_QUADS 
				|| m_curTexture!=_quad->tex 
				|| m_currSrcBlend != _quad->srcBlend 
				|| m_currDstBlend != _quad->dstBlend )
			{
				render_batch_( false );

				CurPrimType = MENGE_PRIM_QUADS;
				//if(CurBlendMode != quad->blend) _SetBlendMode(quad->blend);
				if( m_currSrcBlend != _quad->srcBlend || m_currDstBlend != _quad->dstBlend )
				{
					setBlendState_( _quad->srcBlend, _quad->dstBlend );
				}
				if(_quad->tex != m_curTexture)
				{
					m_pD3DDevice->SetTexture( 0, _quad->tex );
					m_curTexture = _quad->tex;
				}
			}

			memcpy( &VertArray[nPrim*MENGE_PRIM_QUADS], _quad->v, sizeof(TVertex)*MENGE_PRIM_QUADS);
			nPrim++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::render_triple_( TTriple* _triple )
	{
		if(VertArray)
		{
			if( CurPrimType!=MENGE_PRIM_TRIPLES 
				|| nPrim>=VERTEX_BUFFER_SIZE/MENGE_PRIM_TRIPLES 
				|| m_curTexture != _triple->tex 
				|| m_currSrcBlend != _triple->srcBlend 
				|| m_currDstBlend != _triple->dstBlend )
			{
				render_batch_( false );

				CurPrimType = MENGE_PRIM_TRIPLES;
				if( m_currSrcBlend != _triple->srcBlend || m_currDstBlend != _triple->dstBlend )
				{
					setBlendState_( _triple->srcBlend, _triple->dstBlend );
				}
				if(_triple->tex != m_curTexture) 
				{
					m_pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)_triple->tex );
					m_curTexture = _triple->tex;
				}
			}

			memcpy( &VertArray[nPrim*MENGE_PRIM_TRIPLES], _triple->v, sizeof(TVertex)*MENGE_PRIM_TRIPLES );
			nPrim++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::begin_scene_( DX8RenderTexture* _target /*= NULL */ )
	{
		NumDips = 0;
		LPDIRECT3DSURFACE8 pSurf=0, pDepth=0;
		D3DDISPLAYMODE Mode;

		HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
		if (hr == D3DERR_DEVICELOST)
		{
			return false;
		}
		else if (hr == D3DERR_DEVICENOTRESET)
		{
			if( !m_fullscreen )
			{
				if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN) 
				{
					log_error( "Can't determine desktop video mode" );
					return false;
				}

				d3dppW.BackBufferFormat = Mode.Format;
				if(format_id_(Mode.Format) < 4) 
				{
					m_screenBits = 16;
				}
				else 
				{
					m_screenBits = 32;
				}
			}

			if( !gfx_restore_() ) return false; 
		}

		if(VertArray)
		{
			log_error( "Gfx_BeginScene: Scene is already being rendered" );
			return false;
		}

		if( _target != m_curRenderTexture )
		{
			if(_target)
			{
				_target->getInterface()->GetSurfaceLevel(0, &pSurf);
				pDepth=_target->getDepthInterface();
			}
			else
			{
				pSurf=pScreenSurf;
				pDepth=pScreenDepth;
			}
			if(FAILED(m_pD3DDevice->SetRenderTarget(pSurf, pDepth)))
			{
				if(_target) pSurf->Release();
				log_error( "Gfx_BeginScene: Can't set render target" );
				return false;
			}
			if(_target)
			{
				pSurf->Release();
				if(_target->getDepthInterface()) m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ); 
				else m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ); 
				setProjectionMatrix_( _target->getWidth(), _target->getHeight() );
			}
			else
			{
				//if(bZBuffer) pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ); 
				/*else*/ m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
				setProjectionMatrix_( m_screenResolution[0], m_screenResolution[1] );
			}

			m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
			matIdent_(&m_matView);
			m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_matView);

			m_curRenderTexture = _target;
		}

		m_pD3DDevice->BeginScene();
		pVB->Lock( 0, 0, (BYTE**)&VertArray, 0 );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::set_clipping_( int _x /*= 0*/, int _y /*= 0*/, int _w /*= 0*/, int _h /*= 0 */ )
	{
		D3DVIEWPORT8 vp;
		int scr_width, scr_height;

		if( !m_curRenderTexture ) 
		{
			scr_width = m_screenResolution[0];
			scr_height = m_screenResolution[1];
		}
		else 
		{
			scr_width = m_curRenderTexture->getWidth();
			scr_height = m_curRenderTexture->getHeight();
		}

		if(!_w) 
		{
			vp.X=0;
			vp.Y=0;
			vp.Width=scr_width;
			vp.Height=scr_height;
		}
		else
		{
			if(_x<0) { _w+=_x; _x=0; }
			if(_y<0) { _h+=_y; _y=0; }

			if(_x+_w > scr_width) _w=scr_width-_x;
			if(_y+_h > scr_height) _h=scr_height-_y;

			vp.X=_x;
			vp.Y=_y;
			vp.Width=_w;
			vp.Height=_h;
		}

		vp.MinZ=0.0f;
		vp.MaxZ=1.0f;

		render_batch_( false );
		HRESULT hr = m_pD3DDevice->SetViewport(&vp);
		if( FAILED( hr ) )
		{
			log_error( "Error: D3D8 failed to SetViewport" );
		}

		D3DMATRIX tmp;
		//D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
		matIdent_( &m_matProj );
		m_matProj._11 = m_matProj._33 = 1.0f; m_matProj._22 = -1.0f;

		//D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
		matIdent_( &tmp );
		tmp._41 = -0.5f; tmp._42 = 0.5f;

		matMul_( &m_matProj, &m_matProj, &tmp );
		matOrthoOffCenterLH_( &tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
		matMul_( &m_matProj, &m_matProj, &tmp );
		m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::gfx_restore_()
	{

		if( m_syncTargets[0] )
		{
			m_syncTargets[0]->Release();
			m_syncTargets[0] = 0;
		}
		if( m_syncTargets[1] )
		{
			m_syncTargets[1]->Release();
			m_syncTargets[1] = 0;
		}
		if( m_syncTemp )
		{
			m_syncTemp->Release();
			m_syncTemp = 0;
		}
		if( m_syncTempTex )
		{
			m_syncTempTex->Release();
			m_syncTempTex = 0;
		}
		if(pScreenSurf) pScreenSurf->Release();
		if(pScreenDepth) pScreenDepth->Release();

		for( TRenderTextureMap::iterator it = m_renderTextureMap.begin(), it_end = m_renderTextureMap.end();
			it != it_end;
			it++ )
		{
			if( it->second == NULL ) continue;

			it->second->getInterface()->Release();
			it->second->getDepthInterface()->Release();
		}

		if( pIB || pIB3D )
		{
			m_pD3DDevice->SetIndices(NULL,0);
		}

		if( pVB || pVB3D )
		{
			m_pD3DDevice->SetStreamSource( 0, NULL, sizeof(TVertex) );
		}

		if(pIB)
		{
			pIB->Release();
		}
		if(pVB)
		{
			pVB->Release();
		}
		if(pIB3D)
		{
			pIB3D->Release();
		}
		if(pVB3D)
		{
			pVB3D->Release();
		}

		HRESULT hr = m_pD3DDevice->Reset(d3dpp);
		if( FAILED( hr ) )
		{
			return false;
		}

		if(!init_lost_()) return false;

		//if(procGfxRestoreFunc) return procGfxRestoreFunc();
		onRestoreDevice();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::vector<int> & DX8RenderSystem::getResolutionList()
	{
		static std::vector<int> a;
		return a;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::gfx_done_()
	{
		if( m_syncTargets[0] )
		{
			m_syncTargets[0]->Release();
			m_syncTargets[0] = 0;
		}
		if( m_syncTargets[1] )
		{
			m_syncTargets[1]->Release();
			m_syncTargets[1] = 0;
		}
		if( m_syncTemp )
		{
			m_syncTemp->Release();
			m_syncTemp = 0;
		}
		if( m_syncTempTex )
		{
			m_syncTempTex->Release();
			m_syncTempTex = 0;
		}

		for( TTextureMap::iterator it = m_textureMap.begin(), it_end = m_textureMap.end();
			it != it_end;
			it++ )
		{
			it->second->getInterface()->Release();
			delete it->second;
		}
		m_textureMap.clear();

		if(pScreenSurf) 
		{ 
			pScreenSurf->Release();
			pScreenSurf=0; 
		}
		if(pScreenDepth) 
		{ 
			pScreenDepth->Release();
			pScreenDepth=0; 
		}

		for( TRenderTextureMap::iterator it = m_renderTextureMap.begin(), it_end = m_renderTextureMap.end();
			it != it_end;
			it++ )
		{
			if( it->second == NULL ) continue;

			IDirect3DTexture8* pTex = it->second->getInterface();
			IDirect3DSurface8* pDepth = it->second->getDepthInterface();
			if(pTex) pTex->Release();
			if(pDepth) pDepth->Release();
			delete it->second;
		}
		m_renderTextureMap.clear();

		if( m_pD3DDevice )
		{
			m_pD3DDevice->SetStreamSource( 0, NULL, sizeof(TVertex) );
			m_pD3DDevice->SetIndices(NULL,0);
		}

		if( pIB )
		{
			pIB->Release();
			pIB = 0;
		}
		if( pIB3D )
		{
			pIB3D->Release();
			pIB3D = 0;
		}
		if( pVB )
		{
			if( VertArray ) 
			{	
				pVB->Unlock(); 
				VertArray = 0;
			}
			pVB->Release();
			pVB = 0;
		}
		if( pVB3D )
		{
			pVB3D->Release();
			pVB3D = 0;
		}
		if(m_pD3DDevice) 
		{ 
			m_pD3DDevice->Release(); 
			m_pD3DDevice=0; 
		}
		if(m_pD3D) 
		{ 
			m_pD3D->Release();
			m_pD3D=0; 
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setBlendState_( EBlendFactor _srcBlend, EBlendFactor _dstBlend )
	{
		if( m_currSrcBlend != _srcBlend )
		{
			m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, s_toD3DBlend( _srcBlend ) );
			m_currSrcBlend = _srcBlend;
		}
		if( m_currDstBlend != _dstBlend )
		{
			m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, s_toD3DBlend( _dstBlend ) );
			m_currDstBlend = _dstBlend;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::onRestoreDevice()
	{
		// restoring render targets
		/*for( TTargetMap::iterator it = m_targetMap.begin(), it_end = m_targetMap.end();
			it != it_end;
			it++ )
		{
			if( it->second.texture != 0 )
			{
				HTEXTURE htex = m_hge->Target_GetTexture( it->second.handle );
				it->second.texture->restore( htex );
			}
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
