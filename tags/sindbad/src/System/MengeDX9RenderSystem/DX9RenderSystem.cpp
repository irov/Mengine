/*
 *	DX9RenderSystem.cpp
 *
 *	Created by _Berserk_ on 23.05.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DX9RenderSystem.h"
#	include "DX9Texture.h"
#	include "DX9RenderTexture.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#	include <cmath>
#	include <algorithm>

#if defined(_MSC_VER)
#define snprintf _snprintf
#endif

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::RenderSystemInterface ** _ptrInterface )
{
	*_ptrInterface = new Menge::DX9RenderSystem();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::RenderSystemInterface* _ptrInterface )
{
	delete static_cast<Menge::DX9RenderSystem*>(_ptrInterface);
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
	D3DTEXTUREOP s_toD3DTextureOp( ETextureOp _textureOp )
	{
		switch( _textureOp )
		{
		case TOP_DISABLE:
			return D3DTOP_DISABLE;
		case TOP_SELECTARG1:
			return D3DTOP_SELECTARG1;
		case TOP_SELECTARG2:
			return D3DTOP_SELECTARG2;
		case TOP_MODULATE:
			return D3DTOP_MODULATE;
		case TOP_ADD:
			return D3DTOP_ADD;
		case TOP_SUBSTRACT:
			return D3DTOP_SUBTRACT;
		}
		return D3DTOP_DISABLE;
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD s_toD3DTextureArg( ETextureArgument _texArg )
	{
		switch( _texArg )
		{
		case TARG_CURRENT:
			return D3DTA_CURRENT;
		case TARG_DIFFUSE:
			return D3DTA_DIFFUSE;
		case TARG_SPECULAR:
			return D3DTA_SPECULAR;
		case TARG_TEXTURE:
			return D3DTA_TEXTURE;
		case TARG_TFACTOR:
			return D3DTA_TFACTOR;
		}
		return D3DTA_DIFFUSE;
	}
	//////////////////////////////////////////////////////////////////////////
	D3DSAMPLERSTATETYPE s_toD3DTextureFilterType( ETextureFilterType _filterType )
	{
		switch( _filterType )
		{
		case TFT_MAGNIFICATION:
			return D3DSAMP_MAGFILTER;
		case TFT_MINIFICATION:
			return D3DSAMP_MINFILTER;
		case TFT_MIPMAP:
			return D3DSAMP_MIPFILTER;
		}
		return D3DSAMP_MAGFILTER;
	}
	//////////////////////////////////////////////////////////////////////////
	D3DTEXTUREFILTERTYPE s_toD3DTextureFilter( ETextureFilter _filter )
	{
		switch( _filter )
		{
		case TF_NONE:
			return D3DTEXF_NONE;
		case TF_POINT:
			return D3DTEXF_POINT;
		case TF_LINEAR:
			return D3DTEXF_LINEAR;
		case TF_ANISOTROPIC:
			return D3DTEXF_ANISOTROPIC;
		//case TF_FLATCUBIC:
		//	return D3DTEXF_FLATCUBIC;
		case TF_GAUSSIANCUBIC:
			return D3DTEXF_GAUSSIANQUAD;
		}
		return D3DTEXF_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderSystem::DX9RenderSystem()
		: m_logSystem( NULL )
		, m_pD3D( NULL )
		, m_pD3DDevice( NULL )
		, m_inRender( false )
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
		for( size_t i = 0; i < D3DDP_MAXTEXCOORD; ++i )
		{
			m_addressU[i] = D3DTADDRESS_WRAP;
			m_addressV[i] = D3DTADDRESS_WRAP;
			m_textureColorArg1[i] = D3DTA_TEXTURE;
			m_textureColorArg2[i] = D3DTA_CURRENT;
			m_textureAlphaArg1[i] = D3DTA_TEXTURE;
			m_textureAlphaArg2[i] = D3DTA_CURRENT;
		}
		m_textureColorOp[0] = D3DTOP_MODULATE;
		m_textureAlphaOp[0] = D3DTOP_SELECTARG1;
		for( size_t i = 1; i < D3DDP_MAXTEXCOORD; ++i )
		{
			m_textureColorOp[i] = D3DTOP_DISABLE;
			m_textureAlphaOp[i] = D3DTOP_DISABLE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderSystem::~DX9RenderSystem()
	{
		gfx_done_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::initialize( LogSystemInterface* _logSystem, RenderSystemListener* _listener )
	{
		m_logSystem = _logSystem;
		m_listener = _listener;

		D3DADAPTER_IDENTIFIER9 AdID;
		D3DDISPLAYMODE Mode;
		D3DFORMAT Format = D3DFMT_UNKNOWN;
		UINT nModes, i;
		m_frames = 0;

		// Init D3D
		log( "Initializing DX9RenderSystem..." );
		m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ); // D3D_SDK_VERSION
		if( m_pD3D == NULL )
		{
			log_error( "Error (DX9RenderSystem::initialize): Direct3DCreate9 failed" );
			return false;
		}

		m_adapterToUse = D3DADAPTER_DEFAULT;
		m_deviceType = D3DDEVTYPE_HAL;
		// Look for 'NVIDIA PerfHUD' adapter
		// If it is present, override default settings
		for ( UINT Adapter = 0; Adapter < m_pD3D->GetAdapterCount(); ++Adapter )
		{
			D3DADAPTER_IDENTIFIER9 Identifier;
			HRESULT Res = m_pD3D->GetAdapterIdentifier( Adapter, 0, &Identifier );
			if (strstr(Identifier.Description,"PerfHUD") != 0)
			{
				m_adapterToUse = Adapter;
				m_deviceType = D3DDEVTYPE_REF;
				break;
			}
		}

		// Get adapter info
		if(FAILED( m_pD3D->GetAdapterIdentifier( m_adapterToUse, 0, &AdID ) ) )
		{
			log_error( "Can't determine adapter identifier" );
			return false;
		}

		log( "D3D Driver: %s", AdID.Driver );
		log( "Description: %s", AdID.Description );
		log( "Version: %d.%d.%d.%d",
			HIWORD(AdID.DriverVersion.HighPart),
			LOWORD(AdID.DriverVersion.HighPart),
			HIWORD(AdID.DriverVersion.LowPart),
			LOWORD(AdID.DriverVersion.LowPart));

		// Set up Windowed presentation parameters
		if(FAILED( m_pD3D->GetAdapterDisplayMode(m_adapterToUse, &Mode)) || Mode.Format==D3DFMT_UNKNOWN ) 
		{
			log_error( "Error (DX9RenderSystem::initialize): Can't determine desktop video mode" );
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
		nModes = m_pD3D->GetAdapterModeCount( m_adapterToUse, D3DFMT_X8R8G8B8 );

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
			m_pD3D->EnumAdapterModes( m_adapterToUse, D3DFMT_X8R8G8B8, i, &Mode );
			if(Mode.Width != screenWidth || Mode.Height != screenHeight) continue;
			//if(nScreenBPP==16 && (_format_id(Mode.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
			if(format_id_(Mode.Format) > format_id_(Format)) Format=Mode.Format;
		}

		if(Format == D3DFMT_UNKNOWN)
		{
			log_error( "Error (DX9RenderSystem::initialize): Can't find appropriate full screen video mode" );
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

		D3DCAPS9 caps;
		m_pD3D->GetDeviceCaps( m_adapterToUse, m_deviceType, &caps );
		if( ( caps.TextureCaps & D3DPTEXTURECAPS_POW2 ) == 0 
			|| ( caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL ) != 0 )
		{
			m_supportNPOT = true;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits, 
		bool _fullscreen, WindowHandle _winHandle, bool _waitForVSync, int _FSAAType, int _FSAAQuality )
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
		d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;

		if( _waitForVSync == true )
		{
			d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		d3dppW.EnableAutoDepthStencil = TRUE;
		d3dppW.AutoDepthStencilFormat = D3DFMT_D16;

		d3dppFS.BackBufferWidth  = _width;
		d3dppFS.BackBufferHeight = _height;
		d3dppFS.hDeviceWindow    = (HWND)_winHandle;


		if( _waitForVSync == true )
		{
			d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		d3dppFS.EnableAutoDepthStencil = TRUE;
		d3dppFS.AutoDepthStencilFormat = D3DFMT_D16;

		d3dpp = _fullscreen ? &d3dppFS : &d3dppW;

		if(format_id_(d3dpp->BackBufferFormat) < 4) m_screenBits = 16;
		else m_screenBits = 32;

		// Create D3D Device
		HRESULT hr;

		hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)_winHandle,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
			d3dpp, &m_pD3DDevice );

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)_winHandle,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
				d3dpp, &m_pD3DDevice );
		}

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)_winHandle,
				D3DCREATE_MIXED_VERTEXPROCESSING |D3DCREATE_FPU_PRESERVE ,
				d3dpp, &m_pD3DDevice );

			if( FAILED( hr ) )
			{
				Sleep( 100 );
				hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)_winHandle,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
					d3dpp, &m_pD3DDevice );
			}
		}

		if( FAILED ( hr ) )
		{
			log_error( "Error (DX9RenderSystem::createRenderWindow): Can't create D3D device (hr:%d, hwnd:%d)",
				hr, (HWND)_winHandle );
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

		if(!init_lost_()) 
		{
			return false;
		}

		clear( 0 );

		log( "DX9RenderSystem initalized successfully!" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::screenshot( RenderImageInterface* _image, const float * _rect )
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
		DX9Texture* dxTexture = static_cast<DX9Texture*>( _image );

		LPDIRECT3DSURFACE9 surf;
		HRESULT hr = m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &surf );
		//HRESULT hr = m_pD3DDevice->GetFrontBuffer( m_frontBufferCopySurface );
		if( FAILED( hr ) )
		{
			log_error( "Error (DX9RenderSystem::screenshot): failed to GetBackBuffer (hr:%d)", hr );
			return;
		}

		IDirect3DTexture9* dtext = dxTexture->getInterface();
		LPDIRECT3DSURFACE9 dsurf;
		D3DSURFACE_DESC desc;
		dtext->GetLevelDesc(0, &desc);
		hr = dtext->GetSurfaceLevel(0, &dsurf );
		if( FAILED( hr ) )
		{
			log_error( "Error (DX9RenderSystem::screenshot): failed to GetSurfaceLevel (hr:%d)", hr );
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
			log_error( "Error (DX9RenderSystem::screenshot): failed to loadSurfaceFromSurface_ (hr:%d)", hr );
		}
		surf->Release();
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::render()
	{
		// deprecated
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setContentResolution( const std::size_t * _resolution )
	{
		m_contentResolution[0] = _resolution[0];
		m_contentResolution[1] = _resolution[1];
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setProjectionMatrix( const float * _projection )
	{
		//std::copy( _projection, _projection + 16, &(m_matProj._11) );
		m_pD3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)_projection );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setViewMatrix( const float * _view )
	{
		//std::copy( _view, _view + 16, &(m_matView._11) );
		m_pD3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)_view );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setWorldMatrix( const float * _world )
	{
		//std::copy( _world, _world + 16, &(m_matWorld._11) );
		m_pD3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)_world );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX9RenderSystem::createImage( std::size_t& _width, std::size_t& _height, PixelFormat& _format )
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
		
		IDirect3DTexture9* dxTextureInterface = NULL;
		HRESULT hr = d3dCreateTexture_( tex_width, tex_height, 1, 0, 
			s_toD3DFormat( _format ), D3DPOOL_MANAGED, &dxTextureInterface );

		if( FAILED( hr ) )
		{
			log_error( "Error (DX9RenderSystem::createImage): failed to d3dCreateTexture_ %dx%d", _width, _height );
			return NULL;
		}

		DX9Texture* dxTexture = new DX9Texture( dxTextureInterface );

		_width = tex_width;
		_height = tex_height;
		return dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX9RenderSystem::createRenderTargetImage( std::size_t& _width, std::size_t& _height )
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

		IDirect3DTexture9* dxTextureInterface = NULL;
		HRESULT hr = d3dCreateTexture_( tex_width, tex_height, 1, D3DUSAGE_RENDERTARGET,
			fmt, D3DPOOL_DEFAULT, &dxTextureInterface );

		if( FAILED( hr ) )
		{
			log_error( "DX8RenderSystem: can't create RenderTexture %dx%d", _width, _height );
			return NULL;
		}

		IDirect3DSurface9* depthSurface = NULL;

		hr = m_pD3DDevice->CreateDepthStencilSurface( tex_width, tex_height,
			D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &depthSurface, NULL );
		if( FAILED( hr ) )
		{   
			dxTextureInterface->Release();
			log_error( "Can't create render target depth buffer" );
			return NULL;
		}

		DX9RenderTexture* dxRenderTexture = new DX9RenderTexture( dxTextureInterface, depthSurface );

		_width = tex_width;
		_height = tex_height;
		m_renderTextureList.push_back( dxRenderTexture );
		return dxRenderTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::releaseImage( RenderImageInterface * _image )
	{
		DX9Texture* dxTexture = static_cast<DX9Texture*>( _image );
		if( dxTexture == NULL )
		{
			return;
		}

		TRenderTextureList::iterator it_find = std::find( m_renderTextureList.begin()
														  , m_renderTextureList.end()
														  , dxTexture );
		if( it_find != m_renderTextureList.end() )
		{
			DX9RenderTexture* rtextrue = (DX9RenderTexture*)dxTexture;
			rtextrue->getInterface()->Release();
			rtextrue->getDepthInterface()->Release();
			m_renderTextureList.erase( it_find );
		}
		else
		{
			IDirect3DTexture9* pTex = dxTexture->getInterface();
			pTex->Release();
		}
		delete dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::beginScene()
	{
		if( m_inRender )
		{
			m_pD3DDevice->EndScene();
		}
		if( begin_scene_() == false )
		{
			//log_error( "Error: D3D8 Failed to BeginScene" );
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
	void DX9RenderSystem::endScene()
	{
		m_pD3DDevice->EndScene();

		m_inRender = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::swapBuffers()
	{
		// sync GPU with CPU
		syncCPU_();

		HRESULT hr = m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
		if( FAILED( hr ) )
		{
			log_error( "Error: D3D9 failed to swap buffers" );
		}
		m_frames++;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clearFrameBuffer( uint32 _frameBufferTypes, uint32 _color, float _depth, uint16 _stencil )
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
	void DX9RenderSystem::beginLayer2D()
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::endLayer2D()
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::beginLayer3D()
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::endLayer3D()
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setRenderArea( const float* _renderArea )
	{
		float w = _renderArea[2] - _renderArea[0];
		float h = _renderArea[3] - _renderArea[1];

		set_clipping_( (int)_renderArea[0], (int)_renderArea[1], (int)w, (int)h );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
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
	void DX9RenderSystem::setRenderTarget( RenderImageInterface* _renderTarget, bool _clear )
	{
		if( m_inRender )
		{
			m_pD3DDevice->EndScene();
		}
		else
		{
			m_inRender = true;
		}

		m_currentRenderTarget = static_cast<DX9RenderTexture*>( _renderTarget );
		begin_scene_( m_currentRenderTarget );
		if( m_currentRenderTarget == NULL ) return;
		if( m_currentRenderTarget->isDirty() && _clear )
		{
			clear( 0 );
			m_currentRenderTarget->setDirty( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * DX9RenderSystem::createLight( const String & _name )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::releaseLight( LightInterface * _light )
	{
		// empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::onWindowMovedOrResized()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::onWindowClose()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::log( const char* _message, ... )
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
	void DX9RenderSystem::log_error( const char* _message, ... )
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
	bool DX9RenderSystem::supportNPOT() const
	{
		return m_supportNPOT;
		//return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::syncCPU_()
	{
		//_render_batch( false );
		m_pD3DDevice->BeginScene();

		m_pD3DDevice->SetRenderTarget( 0, m_syncTargets[m_frames % 2] );
		//m_syncTargets[m_frames % 2]->Release();
		//setProjectionMatrix_( 2, 2 );
		//m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
		
		D3DMATRIX view;
		matIdent_( &view );
		m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

		m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1 );

		if( m_curRenderTexture )
		{
			LPDIRECT3DSURFACE9 surf;
			m_curRenderTexture->getInterface()->GetSurfaceLevel( 0, &surf );
			m_pD3DDevice->SetRenderTarget( 0, surf );
			m_pD3DDevice->SetDepthStencilSurface( m_curRenderTexture->getDepthInterface() );
			surf->Release();
			//setProjectionMatrix_( m_curRenderTexture->getWidth(), m_curRenderTexture->getHeight() );
		}
		else
		{
			m_pD3DDevice->SetRenderTarget( 0, pScreenSurf );
			m_pD3DDevice->SetDepthStencilSurface( pScreenDepth );
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
	int DX9RenderSystem::format_id_( D3DFORMAT _format )
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
	void DX9RenderSystem::matIdent_( D3DMATRIX* _mtx )
	{
		_mtx->_12 = _mtx->_13 = _mtx->_14 = 
		_mtx->_21 = _mtx->_23 = _mtx->_24 = 
		_mtx->_31 = _mtx->_32 = _mtx->_34 = 
		_mtx->_41 = _mtx->_42 = _mtx->_43 = 0.0f;
		_mtx->_11 = _mtx->_22 = _mtx->_33 = _mtx->_44 = 1.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::matMul_( D3DMATRIX* _out, D3DMATRIX* _mtxl, D3DMATRIX* _mtxr )
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
	bool DX9RenderSystem::init_lost_()
	{
		// Store render target
		pScreenSurf = NULL;
		pScreenDepth = NULL;

		m_pD3DDevice->GetRenderTarget(0, &pScreenSurf);
		m_pD3DDevice->GetDepthStencilSurface(&pScreenDepth);

		for( TRenderTextureList::iterator it = m_renderTextureList.begin(), it_end = m_renderTextureList.end();
			it != it_end;
			it++ )
		{
			IDirect3DTexture9* d3dTexInterface = (*it)->getInterface();
			IDirect3DSurface9* depthInterface = (*it)->getDepthInterface();
			if( d3dTexInterface != NULL )
			{
				d3dCreateTexture_( (*it)->getWidth(), (*it)->getHeight(), 1, D3DUSAGE_RENDERTARGET,
					d3dpp->BackBufferFormat, D3DPOOL_DEFAULT, &d3dTexInterface );
				(*it)->setTexInterface( d3dTexInterface );
			}
			if( depthInterface != NULL )
			{
				m_pD3DDevice->CreateDepthStencilSurface( (*it)->getWidth(), (*it)->getHeight(),
					D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &depthInterface, NULL );
				(*it)->setDepthInterface( depthInterface );
			}
		}

		// sync surfaces
		D3DFORMAT fmt = D3DFMT_X8R8G8B8;
		UINT w = 2;
		UINT d = 1;
		//D3DXCheckTextureRequirements( pD3DDevice, &w, &w, &d, D3DUSAGE_RENDERTARGET, &fmt, D3DPOOL_DEFAULT );
		m_pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, 0, TRUE, &(m_syncTargets[0]), NULL );
		m_pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, 0, TRUE, &(m_syncTargets[1]), NULL );
		d3dCreateTexture_( w, w, d, 0, fmt, D3DPOOL_SYSTEMMEM, &m_syncTempTex );
		m_syncTempTex->GetSurfaceLevel( 0, &m_syncTemp );

		for( std::map<VBHandle, VBInfo>::iterator it = m_vertexBuffers.begin(), it_end = m_vertexBuffers.end();
			it != it_end;
			++it )
		{
			VBInfo& vbInfo = it->second;
			HRESULT hr = m_pD3DDevice->CreateVertexBuffer( vbInfo.length, vbInfo.usage, 
														vbInfo.fvf, vbInfo.pool, &vbInfo.pVB, NULL );
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
															ibInfo.pool, &ibInfo.pIB, NULL );
			if( FAILED( hr ) )
			{
				return false;
			}
		}

		// Set common render states
		//m_pD3DDevice->SetVertexShader( D3DFVF_MENGE_VERTEX );
		m_pD3DDevice->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		m_pD3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	HRESULT DX9RenderSystem::d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,  DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * ppTexture )
	{
		return m_pD3DDevice->CreateTexture( Width, Height, MipLevels, Usage, Format, Pool, ppTexture, NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clear( DWORD _color )
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
	void DX9RenderSystem::setTextureMatrix( size_t _stage, const float* _texture )
	{
		if( _texture != NULL )
		{
			m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			D3DTRANSFORMSTATETYPE level = static_cast<D3DTRANSFORMSTATETYPE>( static_cast<DWORD>( D3DTS_TEXTURE0 ) + _stage );
			m_pD3DDevice->SetTransform( level, (const D3DMATRIX*)_texture );
		}
		else
		{
			m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	HRESULT DX9RenderSystem::loadSurfaceFromSurface_( LPDIRECT3DSURFACE9 pDestSurface, CONST RECT * pDestRect,  LPDIRECT3DSURFACE9 pSrcSurface, CONST RECT * pSrcRect )
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

		if( srcWidth == dstWidth
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
		{
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
		}

		pSrcSurface->UnlockRect();
		pDestSurface->UnlockRect();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::begin_scene_( DX9RenderTexture* _target /*= NULL */ )
	{
		LPDIRECT3DSURFACE9 pSurf=0, pDepth=0;
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
				if(FAILED(m_pD3D->GetAdapterDisplayMode(m_adapterToUse, &Mode)) || Mode.Format==D3DFMT_UNKNOWN) 
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
			if(FAILED(m_pD3DDevice->SetRenderTarget( 0, pSurf ))
				|| FAILED(m_pD3DDevice->SetDepthStencilSurface( pDepth )))
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
	void DX9RenderSystem::set_clipping_( int _x, int _y, int _w, int _h )
	{
		D3DVIEWPORT9 vp;
			
		vp.X=_x;
		vp.Y=_y;
		vp.Width=_w;
		vp.Height=_h;

		vp.MinZ=0.0f;
		vp.MaxZ=1.0f;

		HRESULT hr = m_pD3DDevice->SetViewport(&vp);
		if( FAILED( hr ) )
		{
			log_error( "Error: D3D9 failed to SetViewport" );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::gfx_restore_()
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

		m_pD3DDevice->SetIndices(NULL);
		m_pD3DDevice->SetStreamSource( 0, NULL, 0, 0 );

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
	const std::vector<int> & DX9RenderSystem::getResolutionList()
	{
		static std::vector<int> a;
		return a;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::gfx_done_()
	{
		ULONG useCount = 0;
		if( m_syncTargets[0] )
		{
			useCount = m_syncTargets[0]->Release();
			m_syncTargets[0] = 0;
		}
		if( m_syncTargets[1] )
		{
			useCount = m_syncTargets[1]->Release();
			m_syncTargets[1] = 0;
		}
		if( m_syncTemp )
		{
			useCount = m_syncTemp->Release();
			m_syncTemp = 0;
		}
		if( m_syncTempTex )
		{
			useCount = m_syncTempTex->Release();
			m_syncTempTex = 0;
		}

		if(pScreenSurf) 
		{ 
			useCount = pScreenSurf->Release();
			pScreenSurf=0; 
		}
		if(pScreenDepth) 
		{ 
			useCount = pScreenDepth->Release();
			pScreenDepth=0; 
		}

		if( m_pD3DDevice )
		{
			m_pD3DDevice->SetStreamSource( 0, NULL, 0, 0 );
			m_pD3DDevice->SetIndices(NULL);
		}

		for( std::map<VBHandle, VBInfo>::iterator it = m_vertexBuffers.begin(), it_end = m_vertexBuffers.end();
			it != it_end;
			++it )
		{
			useCount = it->second.pVB->Release();
		}

		for( std::map<IBHandle, IBInfo>::iterator it = m_indexBuffers.begin(), it_end = m_indexBuffers.end();
			it != it_end;
			++it )
		{
			useCount = it->second.pIB->Release();
		}


		if(m_pD3DDevice) 
		{ 
			useCount = m_pD3DDevice->Release(); 
			m_pD3DDevice = 0; 
		}
		if(m_pD3D) 
		{ 
			useCount = m_pD3D->Release();
			m_pD3D = 0; 
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::onRestoreDevice()
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
		for( size_t i = 0; i < D3DDP_MAXTEXCOORD; ++i )
		{
			m_addressU[i] = D3DTADDRESS_WRAP;
			m_addressV[i] = D3DTADDRESS_WRAP;
			m_textureColorArg1[i] = D3DTA_TEXTURE;
			m_textureColorArg2[i] = D3DTA_CURRENT;
			m_textureAlphaArg1[i] = D3DTA_TEXTURE;
			m_textureAlphaArg2[i] = D3DTA_CURRENT;
		}
		m_textureColorOp[0] = D3DTOP_MODULATE;
		m_textureAlphaOp[0] = D3DTOP_SELECTARG1;
		for( size_t i = 1; i < D3DDP_MAXTEXCOORD; ++i )
		{
			m_textureColorOp[i] = D3DTOP_DISABLE;
			m_textureAlphaOp[i] = D3DTOP_DISABLE;
		}
		if( m_listener != NULL )
		{
			m_listener->onDeviceRestored();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	VBHandle DX9RenderSystem::createVertexBuffer( std::size_t _verticesNum, std::size_t _vertexSize )
	{
		IDirect3DVertexBuffer9* vb = NULL;
		HRESULT hr = m_pD3DDevice->CreateVertexBuffer( _verticesNum * _vertexSize, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			0, D3DPOOL_DEFAULT, &vb, NULL );

		if( FAILED(hr) )
		{
			return 0;
		}

		VBInfo vbInfo;
		vbInfo.length = _verticesNum * _vertexSize;
		vbInfo.vertexSize = _vertexSize;
		vbInfo.usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
		vbInfo.fvf = 0;
		vbInfo.pool = D3DPOOL_DEFAULT;
		vbInfo.pVB = vb;
		// count from 1
		m_vertexBuffers.insert( std::make_pair( ++m_vbHandleCounter, vbInfo ) );

		return m_vbHandleCounter;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::releaseVertexBuffer( VBHandle _vbHandle )
	{
		std::map<VBHandle, VBInfo>::iterator it_find = m_vertexBuffers.find( _vbHandle );
		if( it_find != m_vertexBuffers.end() )
		{
			it_find->second.pVB->Release();
			m_vertexBuffers.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	IBHandle DX9RenderSystem::createIndexBuffer( std::size_t _indiciesNum )
	{
		IDirect3DIndexBuffer9* ib = NULL;
		HRESULT hr = m_pD3DDevice->CreateIndexBuffer( sizeof(uint16) * _indiciesNum, D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, NULL );

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
	void DX9RenderSystem::releaseIndexBuffer( IBHandle _ibHandle )
	{
		std::map<IBHandle, IBInfo>::iterator it_find = m_indexBuffers.find( _ibHandle );
		if( it_find != m_indexBuffers.end() )
		{
			it_find->second.pIB->Release();
			m_indexBuffers.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void* DX9RenderSystem::lockVertexBuffer( VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags )
	{
		IDirect3DVertexBuffer9* vb = m_vertexBuffers[_vbHandle].pVB;
		void* lock = NULL;
		HRESULT hr = vb->Lock( _offset, _size, &lock, _flags );
		if( FAILED( hr ) )
		{
			lock = NULL;
		}
		return lock;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::unlockVertexBuffer( VBHandle _vbHandle )
	{
		IDirect3DVertexBuffer9* vb = m_vertexBuffers[_vbHandle].pVB;
		HRESULT hr = vb->Unlock();
		return SUCCEEDED( hr );
	}
	//////////////////////////////////////////////////////////////////////////
	uint16* DX9RenderSystem::lockIndexBuffer( IBHandle _ibHandle )
	{
		IDirect3DIndexBuffer9* ib = m_indexBuffers[_ibHandle].pIB;
		uint16* lock = NULL;
		HRESULT hr = ib->Lock( 0, 0, (void**)&lock, 0 );
		return lock;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::unlockIndexBuffer( IBHandle _ibHandle )
	{
		IDirect3DIndexBuffer9* ib = m_indexBuffers[_ibHandle].pIB;
		HRESULT hr = ib->Unlock();
		return SUCCEEDED( hr );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setVertexBuffer( VBHandle _vbHandle )
	{
		if( _vbHandle == 0 )
		{
			return;
		}
		VBInfo& vbInfo = m_vertexBuffers[_vbHandle];
		HRESULT hr = m_pD3DDevice->SetStreamSource( 0, vbInfo.pVB, 0, vbInfo.vertexSize );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setIndexBuffer( IBHandle _ibHandle )
	{
		if( _ibHandle == 0 )
		{
			return;
		}
		IDirect3DIndexBuffer9* ib = m_indexBuffers[_ibHandle].pIB;
		HRESULT hr = m_pD3DDevice->SetIndices( ib );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::drawIndexedPrimitive( EPrimitiveType _type, std::size_t _baseVertexIndex,
		std::size_t _minIndex, std::size_t _verticesNum, std::size_t _startIndex, std::size_t _primCount )
	{
		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive( s_toD3DPrimitiveType( _type ), 
											_baseVertexIndex, _minIndex, _verticesNum, _startIndex, _primCount );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTexture( std::size_t _stage, RenderImageInterface* _texture )
	{
		HRESULT hr;
		IDirect3DTexture9* d3d9Texture = NULL;
		if( _texture != NULL )
		{
			DX9Texture* t = static_cast<DX9Texture*>( _texture );
			d3d9Texture = t->getInterface();
		}
		hr = m_pD3DDevice->SetTexture( _stage, d3d9Texture );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setSrcBlendFactor( EBlendFactor _src )
	{
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, s_toD3DBlend( _src ) );
		if( FAILED( hr ) )
		{
			log_error( "Error: DX9RenderSystem failed to setBlendFactor (hr:%d)", hr );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDstBlendFactor( EBlendFactor _dst )
	{
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, s_toD3DBlend( _dst) );
		if( FAILED( hr ) )
		{
			log_error( "Error: DX9RenderSystem failed to setBlendFactor (hr:%d)", hr );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureAddressing( std::size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
		HRESULT hr;
		D3DTEXTUREADDRESS adrU = s_toD3DTextureAddress( _modeU );
		D3DTEXTUREADDRESS adrV = s_toD3DTextureAddress( _modeV );
		if( m_addressU[_stage] != adrU )
		{
			m_addressU[_stage] = adrU;
			hr = m_pD3DDevice->SetSamplerState( _stage, D3DSAMP_ADDRESSU, adrU );
			if( FAILED( hr ) )
			{
				log_error( "Error: DX9RenderSystem failed to setTextureAddressing (hr:%d)", hr );
			}
		}
		if( m_addressV[_stage] != adrV )
		{
			m_addressV[_stage] = adrV;
			hr = m_pD3DDevice->SetSamplerState( _stage, D3DSAMP_ADDRESSV, adrV );
			if( FAILED( hr ) )
			{
				log_error( "Error: DX9RenderSystem failed to setTextureAddressing (hr:%d)", hr );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureFactor( uint32 _color )
	{
		m_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, _color );
	}
	//////////////////////////////////////////////////////////////////////////
	float DX9RenderSystem::getTexelOffsetX() const
	{
		return 0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
	float DX9RenderSystem::getTexelOffsetY() const
	{
		return 0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setCullMode( ECullMode _mode )
	{
		m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, s_toD3DCullMode( _mode ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferTestEnable( bool _depthTest )
	{
		D3DZBUFFERTYPE test = _depthTest ? D3DZB_TRUE : D3DZB_FALSE;
		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, test );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
	{
		DWORD dWrite = _depthWrite ? TRUE : FALSE;
		m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, dWrite );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		m_pD3DDevice->SetRenderState( D3DRS_ZFUNC, s_toD3DCmpFunc( _depthFunction ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setFillMode( EFillMode _mode )
	{
		m_pD3DDevice->SetRenderState( D3DRS_FILLMODE, s_toD3DFillMode( _mode ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
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
	void DX9RenderSystem::setShadeType( EShadeType _sType )
	{
		m_pD3DDevice->SetRenderState( D3DRS_SHADEMODE, s_toD3DShadeMode( _sType ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaTestEnable( bool _alphaTest )
	{
		DWORD alphaTest = _alphaTest ? TRUE : FALSE;
		m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, alphaTest );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
	{
		DWORD alphaBlend = _alphaBlend ? TRUE : FALSE;
		m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, alphaBlend );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha )
	{
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, s_toD3DCmpFunc( _alphaFunc ) );
		DWORD alpha = _alpha;
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF, alpha );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setLightingEnable( bool _light )
	{
		DWORD value = _light ? TRUE : FALSE;
		m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, value );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp
													,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		HRESULT hr;
		D3DTEXTUREOP colorOp = s_toD3DTextureOp( _textrueOp );
		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		DWORD arg2 = s_toD3DTextureArg( _arg2 );
		if( m_textureColorOp[_stage] != colorOp )
		{
			m_textureColorOp[_stage] = colorOp;
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLOROP, colorOp );
			if( FAILED( hr ) )
			{
				log_error( "Error: DX9RenderSystem failed to setTextureStageColorOp (hr:%d)", hr );
			}
		}
		if( m_textureColorArg1[_stage] != arg1 )
		{
			m_textureColorArg1[_stage] = arg1;
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLORARG1, arg1 );
			if( FAILED( hr ) )
			{
				log_error( "Error: DX9RenderSystem failed to setTextureStageColorOp (hr:%d)", hr );
			}
		}
		if( m_textureColorArg2[_stage] != arg2 )
		{
			m_textureColorArg2[_stage] = arg2;
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLORARG2, arg2 );
			if( FAILED( hr ) )
			{
				log_error( "Error: DX9RenderSystem failed to setTextureStageColorOp (hr:%d)", hr );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp
													,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		HRESULT hr;
		D3DTEXTUREOP alphaOp = s_toD3DTextureOp( _textrueOp );
		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		DWORD arg2 = s_toD3DTextureArg( _arg2 );
		if( m_textureAlphaOp[_stage] != alphaOp )
		{
			m_textureAlphaOp[_stage] = alphaOp;
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAOP, alphaOp );
			if( FAILED( hr ) )
			{
				log_error( "Error: DX9RenderSystem failed to setTextureStageAlphaOp (hr:%d)", hr );
			}
		}
		if( m_textureAlphaArg1[_stage] != arg1 )
		{
			m_textureAlphaArg1[_stage] = arg1;
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAARG1, arg1 );
			if( FAILED( hr ) )
			{
				log_error( "Error: DX9RenderSystem failed to setTextureStageAlphaOp (hr:%d)", hr );
			}
		}
		if( m_textureAlphaArg2[_stage] != arg2 )
		{
			m_textureAlphaArg2[_stage] = arg2;
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAARG2, arg2 );
			if( FAILED( hr ) )
			{
				log_error( "Error: DX9RenderSystem failed to setTextureStageAlphaOp (hr:%d)", hr );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter )
	{
		m_pD3DDevice->SetSamplerState( _stage, s_toD3DTextureFilterType( _filterType )
											, s_toD3DTextureFilter( _filter ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setVertexDeclaration( uint32 _declaration )
	{
		m_pD3DDevice->SetFVF( _declaration );
		//HRESULT hr = m_pD3DDevice->SetVertexShader( _declaration );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
