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

#define D3DFVF_MENGE_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1 )

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
	D3DPRIMITIVETYPE s_toD3DPrimitiveType( EPrimitiveType _type )
	{
		switch( _type )
		{
		case Menge::PT_POINTLIST:
			return D3DPT_POINTLIST;
		case Menge::PT_LINELIST:
			return D3DPT_LINELIST;
		case Menge::PT_LINESTRIP:
			return D3DPT_LINESTRIP;
		case Menge::PT_TRIANGLELIST:
			return D3DPT_TRIANGLELIST;
		case Menge::PT_TRIANGLESTRIP:
			return D3DPT_TRIANGLESTRIP;
		case Menge::PT_TRIANGLEFAN:
			return D3DPT_TRIANGLEFAN;
		}
		return D3DPT_POINTLIST;
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
	UINT s_getPrimitiveCount( EPrimitiveType _pType, std::size_t _indexCount )
	{
		switch( _pType )
		{
		case PT_POINTLIST:
			return _indexCount;
		case PT_LINELIST:
			return _indexCount / 2;
		case PT_LINESTRIP:
			return _indexCount - 1;
		case PT_TRIANGLELIST:
			return _indexCount / 3;
		case PT_TRIANGLESTRIP:
		case PT_TRIANGLEFAN:
			return _indexCount - 2;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	D3DTEXTUREADDRESS s_toD3DTextureAddress( ETextureAddressMode _mode )
	{
		switch( _mode )
		{
		case TAM_CLAMP:
			return D3DTADDRESS_CLAMP;
		case TAM_WRAP:
			return D3DTADDRESS_WRAP;
		case TAM_MIRROR:
			return D3DTADDRESS_MIRROR;
		}
		return D3DTADDRESS_CLAMP;
	}
	//////////////////////////////////////////////////////////////////////////
	D3DCULL s_toD3DCullMode( ECullMode _mode )
	{
		switch( _mode )
		{
		case CM_CULL_NONE:
			return D3DCULL_NONE;
		case CM_CULL_CW:
			return D3DCULL_CW;
		case CM_CULL_CCW:
			return D3DCULL_CCW;
		}
		return D3DCULL_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	D3DCMPFUNC s_toD3DCmpFunc( ECompareFunction _func )
	{
		switch( _func )
		{
		case CMPF_ALWAYS_FAIL:
			return D3DCMP_NEVER;
		case CMPF_LESS:
			return D3DCMP_LESS;
		case CMPF_EQUAL:
			return D3DCMP_EQUAL;
		case CMPF_LESS_EQUAL:
			return D3DCMP_LESSEQUAL;
		case CMPF_GREATER:
			return D3DCMP_GREATER;
		case CMPF_NOT_EQUAL:
			return D3DCMP_NOTEQUAL;
		case CMPF_GREATER_EQUAL:
			return D3DCMP_GREATEREQUAL;
		case CMPF_ALWAYS_PASS:
			return D3DCMP_ALWAYS;
		}
		return D3DCMP_NEVER;
	}
	//////////////////////////////////////////////////////////////////////////
	D3DFILLMODE s_toD3DFillMode( EFillMode _mode )
	{
		switch( _mode )
		{
		case FM_POINT:
			return D3DFILL_POINT;
		case FM_WIREFRAME:
			return D3DFILL_WIREFRAME;
		case FM_SOLID:
			return D3DFILL_SOLID;
		}
		return D3DFILL_POINT;
	}
	//////////////////////////////////////////////////////////////////////////
	D3DSHADEMODE s_toD3DShadeMode( EShadeType _type )
	{
		switch( _type )
		{
		case SHT_FLAT:
			return D3DSHADE_FLAT;
		case SHT_GOURAUD:
			return D3DSHADE_GOURAUD;
		case SHT_PHONG:
			return D3DSHADE_PHONG;
		}
		return D3DSHADE_FLAT;
	}
	//////////////////////////////////////////////////////////////////////////
	DX8RenderSystem::DX8RenderSystem()
		: m_logSystem( NULL )
		, m_pD3D( NULL )
		, m_pD3DDevice( NULL )
		, m_inRender( false )
		, m_texFilter( true )
		, m_curRenderTexture( NULL )
		, m_syncTemp( NULL )
		, m_syncTempTex( NULL )
		, pScreenSurf( NULL )
		, pScreenDepth( NULL )
		, m_vbHandleCounter( 0 )
		, m_ibHandleCounter( 0 )
		, m_currentIB( 0 )
		, m_frontBufferCopySurface( NULL )
		, m_listener( NULL )
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
	bool DX8RenderSystem::initialize( LogSystemInterface* _logSystem, RenderSystemListener* _listener )
	{
		m_logSystem = _logSystem;
		m_listener = _listener;

		D3DADAPTER_IDENTIFIER8 AdID;
		D3DDISPLAYMODE Mode;
		D3DFORMAT Format = D3DFMT_UNKNOWN;
		UINT nModes, i;
		m_frames = 0;

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

		m_currentRenderTarget = NULL;

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


		// Init all stuff that can be lost
		//setProjectionMatrix_( _width, _height );
		hr = m_pD3DDevice->CreateImageSurface( d3dpp->BackBufferWidth, d3dpp->BackBufferHeight, D3DFMT_A8R8G8B8, &m_frontBufferCopySurface );

		/*IDirect3DSwapChain8* m_pSwapChain = NULL;
		m_pD3DDevice->CreateAdditionalSwapChain(d3dpp, &m_pSwapChain);
		LPDIRECT3DSURFACE8 pBack=NULL,pStencil=NULL;
		m_pSwapChain->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBack);
		m_pD3DDevice->GetDepthStencilSurface(&pStencil);
		m_pD3DDevice->SetRenderTarget(pBack,pStencil);
		pBack->Release();
		pStencil->Release();*/

		if(!init_lost_()) 
		{
			return false;
		}

		clear( 0 );

		log( "DX8RenderSystem initalized successfully!" );

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
		//HRESULT hr = m_pD3DDevice->GetFrontBuffer( m_frontBufferCopySurface );
		if( FAILED( hr ) )
		{
			log_error( "D3D Error: failed to GetBackBuffer" );
			return;
		}

		IDirect3DTexture8* dtext = dxTexture->getInterface();
		LPDIRECT3DSURFACE8 dsurf;
		D3DSURFACE_DESC desc;
		dtext->GetLevelDesc(0, &desc);
		hr = dtext->GetSurfaceLevel(0, &dsurf );
		if( FAILED( hr ) )
		{
			log_error( "D3D Error: failed to GetSurfaceLevel" );
			return;
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
		//std::copy( _projection, _projection + 16, &(m_matProj._11) );
		m_pD3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)_projection );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setViewMatrix( const float * _view )
	{
		//std::copy( _view, _view + 16, &(m_matView._11) );
		m_pD3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)_view );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setWorldMatrix( const float * _world )
	{
		//std::copy( _world, _world + 16, &(m_matWorld._11) );
		m_pD3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)_world );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX8RenderSystem::createImage( std::size_t& _width, std::size_t& _height, PixelFormat& _format )
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
			log_error( "DX8RenderSystem: can't create texture %dx%d", _width, _height );
			return NULL;
		}

		DX8Texture* dxTexture = new DX8Texture( dxTextureInterface, _width, _height, _format );

		_width = tex_width;
		_height = tex_height;
		return dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX8RenderSystem::createRenderTargetImage( std::size_t& _width, std::size_t& _height )
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
			log_error( "DX8RenderSystem: can't create RenderTexture %dx%d", _width, _height );
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
			_width, _height, PF_X8R8G8B8 );

		_width = tex_width;
		_height = tex_height;
		m_renderTextureList.push_back( dxRenderTexture );
		return dxRenderTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::loadImage( RenderImageInterface* _image, ImageDecoderInterface* _imageDecoder )
	{
		DX8Texture* texture = static_cast<DX8Texture*>( _image );
		if( texture == NULL )
		{
			return;
		}
		
		texture->loadData( _imageDecoder );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::releaseImage( RenderImageInterface * _image )
	{
		DX8Texture* dxTexture = static_cast<DX8Texture*>( _image );
		if( dxTexture == NULL )
		{
			return;
		}

		TRenderTextureList::iterator it_find = std::find( m_renderTextureList.begin()
														  , m_renderTextureList.end()
														  , dxTexture );
		if( it_find != m_renderTextureList.end() )
		{
			DX8RenderTexture* rtextrue = (DX8RenderTexture*)dxTexture;
			rtextrue->getInterface()->Release();
			rtextrue->getDepthInterface()->Release();
			m_renderTextureList.erase( it_find );
		}
		else
		{
			IDirect3DTexture8* pTex = dxTexture->getInterface();
			pTex->Release();
		}
		delete dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::beginScene()
	{
		if( m_inRender )
		{
			m_pD3DDevice->EndScene();
		}
		if( begin_scene_() == false )
		{
			log_error( "Error: D3D8 Failed to BeginScene" );
		}

		m_inRender = true;
		m_currentRenderTarget = NULL;

		// set render targets dirty to clear one time before rendering into one
		for( TRenderTextureList::iterator it = m_renderTextureList.begin(), it_end = m_renderTextureList.end();
			it != it_end;
			it++ )
		{
			(*it)->setDirty( true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::endScene()
	{
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
	void DX8RenderSystem::clearFrameBuffer( uint32 _frameBufferTypes, uint32 _color, float _depth, uint16 _stencil )
	{
		DWORD frameBufferFlags = 0;
		if( ( _frameBufferTypes & FBT_COLOR ) != 0 )
		{
			frameBufferFlags |= D3DCLEAR_TARGET;
		}
		if( ( _frameBufferTypes & FBT_DEPTH ) != 0 )
		{
			frameBufferFlags |= D3DCLEAR_ZBUFFER;
		}
		if( ( _frameBufferTypes & FBT_STENCIL ) != 0 )
		{
			frameBufferFlags |= D3DCLEAR_STENCIL;
		}
		m_pD3DDevice->Clear( 0, NULL, frameBufferFlags, _color, _depth, _stencil );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::beginLayer2D()
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::endLayer2D()
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::beginLayer3D()
	{
		// empty
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
	void DX8RenderSystem::setRenderTarget( RenderImageInterface* _renderTarget, bool _clear )
	{
		if( m_inRender )
		{
			m_pD3DDevice->EndScene();
		}
		else
		{
			m_inRender = true;
		}

		m_currentRenderTarget = static_cast<DX8RenderTexture*>( _renderTarget );
		begin_scene_( m_currentRenderTarget );
		if( m_currentRenderTarget == NULL ) return;
		if( m_currentRenderTarget->isDirty() && _clear )
		{
			clear( 0 );
			m_currentRenderTarget->setDirty( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * DX8RenderSystem::createLight( const String & _name )
	{
		return NULL;
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
		//return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::syncCPU_()
	{
		//_render_batch( false );
		m_pD3DDevice->BeginScene();

		m_pD3DDevice->SetRenderTarget( m_syncTargets[m_frames % 2], 0 );
		//m_syncTargets[m_frames % 2]->Release();
		//setProjectionMatrix_( 2, 2 );
		//m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
		
		D3DMATRIX view;
		matIdent_( &view );
		m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

		m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 3, 0, 1 );

		if( m_curRenderTexture )
		{
			LPDIRECT3DSURFACE8 surf;
			m_curRenderTexture->getInterface()->GetSurfaceLevel( 0, &surf );
			m_pD3DDevice->SetRenderTarget( surf, m_curRenderTexture->getDepthInterface());
			surf->Release();
			//setProjectionMatrix_( m_curRenderTexture->getWidth(), m_curRenderTexture->getHeight() );
		}
		else
		{
			m_pD3DDevice->SetRenderTarget( pScreenSurf, pScreenDepth );
			//setProjectionMatrix_( m_screenResolution[0], m_screenResolution[1] );
		}

		//m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
		matIdent_(&view);
		m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

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
	bool DX8RenderSystem::init_lost_()
	{
		// Store render target
		pScreenSurf = NULL;
		pScreenDepth = NULL;

		m_pD3DDevice->GetRenderTarget(&pScreenSurf);
		m_pD3DDevice->GetDepthStencilSurface(&pScreenDepth);

		for( TRenderTextureList::iterator it = m_renderTextureList.begin(), it_end = m_renderTextureList.end();
			it != it_end;
			it++ )
		{
			IDirect3DTexture8* d3dTexInterface = (*it)->getInterface();
			IDirect3DSurface8* depthInterface = (*it)->getDepthInterface();
			if( d3dTexInterface != NULL )
			{
				d3dCreateTexture_( (*it)->getWidth(), (*it)->getHeight(), 1, D3DUSAGE_RENDERTARGET,
					d3dpp->BackBufferFormat, D3DPOOL_DEFAULT, &d3dTexInterface );
				(*it)->setTexInterface( d3dTexInterface );
			}
			if( depthInterface != NULL )
			{
				m_pD3DDevice->CreateDepthStencilSurface( (*it)->getWidth(), (*it)->getHeight(),
					D3DFMT_D16, D3DMULTISAMPLE_NONE, &depthInterface );
				(*it)->setDepthInterface( depthInterface );
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

		for( std::map<VBHandle, VBInfo>::iterator it = m_vertexBuffers.begin(), it_end = m_vertexBuffers.end();
			it != it_end;
			++it )
		{
			VBInfo& vbInfo = it->second;
			HRESULT hr = m_pD3DDevice->CreateVertexBuffer( vbInfo.length, vbInfo.usage, 
														vbInfo.fvf, vbInfo.pool, &vbInfo.pVB );
			if( FAILED( hr ) )
			{
				return false;
			}
		}

		for( std::map<IBHandle, IBInfo>::iterator it = m_indexBuffers.begin(), it_end = m_indexBuffers.end();
			it != it_end;
			++it )
		{
			IBInfo& ibInfo = it->second;
			HRESULT hr = m_pD3DDevice->CreateIndexBuffer( ibInfo.length, ibInfo.usage, ibInfo.format,
															ibInfo.pool, &ibInfo.pIB );
			if( FAILED( hr ) )
			{
				return false;
			}
		}

		// Set common render states
		m_pD3DDevice->SetVertexShader( D3DFVF_MENGE_VERTEX );

		m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF,        0x01 );
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

		m_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

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

		return true;
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
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		m_pD3DDevice->SetTransform( D3DTS_TEXTURE0, (const D3DMATRIX*)_texture );
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

		//POINT pt = { 0, 0 };
		//HRESULT hr = m_pD3DDevice->CopyRects( pSrcSurface, &srcRect, 1, pDestSurface, &pt );

		HRESULT hr = pDestSurface->LockRect( &dstLockedRect, pDestRect, D3DLOCK_DISCARD );
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

		/*if( srcWidth == dstWidth
			&& srcHeight == dstHeight )
		{
			unsigned char* srcdata = (unsigned char*)srcLockedRect.pBits;
			unsigned char* dstdata = (unsigned char*)dstLockedRect.pBits;
			for( std::size_t i = 0; i < srcHeight; ++i )
			{
				std::copy( srcdata, srcdata + srcWidth * 4, dstdata );
				srcdata += srcLockedRect.Pitch;
				dstdata += dstLockedRect.Pitch;
			}
		}
		else
		{*/
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
		//}

		pSrcSurface->UnlockRect();
		pDestSurface->UnlockRect();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::begin_scene_( DX8RenderTexture* _target /*= NULL */ )
	{
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
				//setProjectionMatrix_( _target->getWidth(), _target->getHeight() );
			}
			else
			{
				/*if(bZBuffer)*/ m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ); 
				//else m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
				//setProjectionMatrix_( m_screenResolution[0], m_screenResolution[1] );
			}

			//m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

			m_curRenderTexture = _target;
		}

		if( FAILED( m_pD3DDevice->BeginScene() ) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::set_clipping_( int _x, int _y, int _w, int _h )
	{
		D3DVIEWPORT8 vp;
			
		vp.X=_x;
		vp.Y=_y;
		vp.Width=_w;
		vp.Height=_h;

		vp.MinZ=0.0f;
		vp.MaxZ=1.0f;

		HRESULT hr = m_pD3DDevice->SetViewport(&vp);
		if( FAILED( hr ) )
		{
			log_error( "Error: D3D8 failed to SetViewport" );
		}

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

		for( TRenderTextureList::iterator it = m_renderTextureList.begin(), it_end = m_renderTextureList.end();
			it != it_end;
			it++ )
		{
			(*it)->getInterface()->Release();
			(*it)->getDepthInterface()->Release();
		}

		m_pD3DDevice->SetIndices(NULL,0);
		m_pD3DDevice->SetStreamSource( 0, NULL, sizeof(TVertex) );

		for( std::map<VBHandle, VBInfo>::iterator it = m_vertexBuffers.begin(), it_end = m_vertexBuffers.end();
			it != it_end;
			++it )
		{
			it->second.pVB->Release();
		}

		for( std::map<IBHandle, IBInfo>::iterator it = m_indexBuffers.begin(), it_end = m_indexBuffers.end();
			it != it_end;
			++it )
		{
			it->second.pIB->Release();
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

		if( m_pD3DDevice )
		{
			m_pD3DDevice->SetStreamSource( 0, NULL, sizeof(TVertex) );
			m_pD3DDevice->SetIndices(NULL,0);
		}

		for( std::map<VBHandle, VBInfo>::iterator it = m_vertexBuffers.begin(), it_end = m_vertexBuffers.end();
			it != it_end;
			++it )
		{
			it->second.pVB->Release();
		}

		for( std::map<IBHandle, IBInfo>::iterator it = m_indexBuffers.begin(), it_end = m_indexBuffers.end();
			it != it_end;
			++it )
		{
			it->second.pIB->Release();
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
		if( m_listener != NULL )
		{
			m_listener->onDeviceRestored();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	VBHandle DX8RenderSystem::createVertexBuffer( std::size_t _verticesNum )
	{
		IDirect3DVertexBuffer8* vb = NULL;
		HRESULT hr = m_pD3DDevice->CreateVertexBuffer( _verticesNum * sizeof(TVertex), D3DUSAGE_WRITEONLY,
			D3DFVF_MENGE_VERTEX, D3DPOOL_DEFAULT, &vb );

		if( FAILED(hr) )
		{
			return 0;
		}

		VBInfo vbInfo;
		vbInfo.length = _verticesNum * sizeof(TVertex);
		vbInfo.usage = D3DUSAGE_WRITEONLY;
		vbInfo.fvf = D3DFVF_MENGE_VERTEX;
		vbInfo.pool = D3DPOOL_DEFAULT;
		vbInfo.pVB = vb;
		// count from 1
		m_vertexBuffers.insert( std::make_pair( ++m_vbHandleCounter, vbInfo ) );

		return m_vbHandleCounter;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::releaseVertexBuffer( VBHandle _vbHandle )
	{
		std::map<VBHandle, VBInfo>::iterator it_find = m_vertexBuffers.find( _vbHandle );
		if( it_find != m_vertexBuffers.end() )
		{
			it_find->second.pVB->Release();
			m_vertexBuffers.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	IBHandle DX8RenderSystem::createIndexBuffer( std::size_t _indiciesNum )
	{
		IDirect3DIndexBuffer8* ib = NULL;
		HRESULT hr = m_pD3DDevice->CreateIndexBuffer( sizeof(uint16) * _indiciesNum, D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib );

		if( FAILED(hr) )
		{
			return 0;
		}

		IBInfo ibInfo;
		ibInfo.length = sizeof(uint16) * _indiciesNum;
		ibInfo.usage = D3DUSAGE_WRITEONLY;
		ibInfo.format = D3DFMT_INDEX16;
		ibInfo.pool = D3DPOOL_DEFAULT;
		ibInfo.pIB = ib;
		// count from 1
		m_indexBuffers.insert( std::make_pair( ++m_ibHandleCounter, ibInfo ) );

		return m_ibHandleCounter;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::releaseIndexBuffer( IBHandle _ibHandle )
	{
		std::map<IBHandle, IBInfo>::iterator it_find = m_indexBuffers.find( _ibHandle );
		if( it_find != m_indexBuffers.end() )
		{
			it_find->second.pIB->Release();
			m_indexBuffers.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TVertex* DX8RenderSystem::lockVertexBuffer( VBHandle _vbHandle )
	{
		IDirect3DVertexBuffer8* vb = m_vertexBuffers[_vbHandle].pVB;
		TVertex* lock = NULL;
		vb->Lock( 0, 0, (BYTE**)&lock, 0 );
		return lock;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::unlockVertexBuffer( VBHandle _vbHandle )
	{
		IDirect3DVertexBuffer8* vb = m_vertexBuffers[_vbHandle].pVB;
		vb->Unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	uint16* DX8RenderSystem::lockIndexBuffer( IBHandle _ibHandle )
	{
		IDirect3DIndexBuffer8* ib = m_indexBuffers[_ibHandle].pIB;
		uint16* lock = NULL;
		ib->Lock( 0, 0, (BYTE**)&lock, 0 );
		return lock;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::unlockIndexBuffer( IBHandle _ibHandle )
	{
		IDirect3DIndexBuffer8* ib = m_indexBuffers[_ibHandle].pIB;
		ib->Unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setVertexBuffer( VBHandle _vbHandle )
	{
		if( _vbHandle == 0 )
		{
			return;
		}
		VBInfo& vbInfo = m_vertexBuffers[_vbHandle];
		m_pD3DDevice->SetStreamSource( 0, vbInfo.pVB, sizeof(TVertex) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setIndexBuffer( IBHandle _ibHandle )
	{
		m_currentIB = _ibHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::drawIndexedPrimitive( EPrimitiveType _type, 
		std::size_t _baseVertexIndex,  std::size_t _startIndex, std::size_t _verticesNum, std::size_t _indiciesNum )
	{
		if( m_currentIB == 0 )
		{
			return;
		}
		IDirect3DIndexBuffer8* ib = m_indexBuffers[m_currentIB].pIB;
		HRESULT hr = m_pD3DDevice->SetIndices( ib, _baseVertexIndex );

		UINT pc = s_getPrimitiveCount( _type, _indiciesNum );
		hr = m_pD3DDevice->DrawIndexedPrimitive( s_toD3DPrimitiveType( _type ),
											0, _verticesNum, _startIndex, pc );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTexture( std::size_t _stage, RenderImageInterface* _texture )
	{
		IDirect3DTexture8* d3d8Texture = NULL;
		if( _texture != NULL )
		{
			DX8Texture* t = static_cast<DX8Texture*>( _texture );
			d3d8Texture = t->getInterface();
		}
		m_pD3DDevice->SetTexture( _stage, d3d8Texture );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst )
	{
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, s_toD3DBlend( _src ) );
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, s_toD3DBlend( _dst) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureAddressing( std::size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
		m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ADDRESSU, s_toD3DTextureAddress( _modeU ) );
		m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ADDRESSV, s_toD3DTextureAddress( _modeV ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureFactor( uint32 _color )
	{
		m_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, _color );
	}
	//////////////////////////////////////////////////////////////////////////
	float DX8RenderSystem::getTexelOffsetX() const
	{
		return 0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
	float DX8RenderSystem::getTexelOffsetY() const
	{
		return 0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setCullMode( ECullMode _mode )
	{
		m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, s_toD3DCullMode( _mode ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setDepthBufferTestEnable( bool _depthTest )
	{
		D3DZBUFFERTYPE test = D3DZB_TRUE;
		if( _depthTest == false )
		{
			test = D3DZB_FALSE;
		}
		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, test );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
	{
		DWORD dWrite = TRUE;
		if( _depthWrite == false )
		{
			dWrite = FALSE;
		}
		m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, dWrite );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		m_pD3DDevice->SetRenderState( D3DRS_ZFUNC, s_toD3DCmpFunc( _depthFunction ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setFillMode( EFillMode _mode )
	{
		m_pD3DDevice->SetRenderState( D3DRS_FILLMODE, s_toD3DFillMode( _mode ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
	{
		DWORD value = 0;
		if( _r == true )
		{
			value |= D3DCOLORWRITEENABLE_RED;
		}
		if( _g == true )
		{
			value |= D3DCOLORWRITEENABLE_GREEN;
		}
		if( _b == true )
		{
			value |= D3DCOLORWRITEENABLE_BLUE;
		}
		if( _a == true )
		{
			value |= D3DCOLORWRITEENABLE_ALPHA;
		}
		m_pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE, value );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setShadeType( EShadeType _sType )
	{
		m_pD3DDevice->SetRenderState( D3DRS_SHADEMODE, s_toD3DShadeMode( _sType ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setAlphaTestEnable( bool _alphaTest )
	{
		DWORD alphaTest = FALSE;
		if( _alphaTest == true )
		{
			alphaTest = TRUE;
		}
		m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, alphaTest );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
	{
		DWORD alphaBlend = FALSE;
		if( _alphaBlend == true )
		{
			alphaBlend = TRUE;
		}
		m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, alphaBlend );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha )
	{
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, s_toD3DCmpFunc( _alphaFunc ) );
		DWORD alpha = _alpha;
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF, alpha );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
