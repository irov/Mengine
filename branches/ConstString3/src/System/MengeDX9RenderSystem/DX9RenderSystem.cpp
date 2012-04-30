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

#	include "Logger/Logger.h"

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
	static const D3DFORMAT D32SFormats[]	= { D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D15S1, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16, (D3DFORMAT) 0 };
	static const D3DFORMAT D32Formats[]		= { D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D16, D3DFMT_D15S1, (D3DFORMAT) 0 };
	static const D3DFORMAT D16SFormats[]	= { D3DFMT_D15S1, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D16, D3DFMT_D32, D3DFMT_D24X8, (D3DFORMAT) 0 };
	static const D3DFORMAT D16Formats[]		= { D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D24X4S4, (D3DFORMAT) 0 };
	//////////////////////////////////////////////////////////////////////////
	static size_t s_getPrimitiveCount( EPrimitiveType _pType, size_t _indexCount )
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
	static D3DFORMAT s_toD3DFormat( int _format )
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
	static DWORD s_toD3DBlend( EBlendFactor _blend )
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
	static D3DPRIMITIVETYPE s_toD3DPrimitiveType( EPrimitiveType _type )
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
	static Menge::uint32 s_firstPOW2From( Menge::uint32 _n )
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
	static D3DTEXTUREADDRESS s_toD3DTextureAddress( ETextureAddressMode _mode )
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
	static D3DCULL s_toD3DCullMode( ECullMode _mode )
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
	static D3DCMPFUNC s_toD3DCmpFunc( ECompareFunction _func )
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
	static D3DFILLMODE s_toD3DFillMode( EFillMode _mode )
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
	static D3DSHADEMODE s_toD3DShadeMode( EShadeType _type )
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
	static D3DTEXTUREOP s_toD3DTextureOp( ETextureOp _textureOp )
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
	static DWORD s_toD3DTextureArg( ETextureArgument _texArg )
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
	static D3DSAMPLERSTATETYPE s_toD3DTextureFilterType( ETextureFilterType _filterType )
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
	static D3DTEXTUREFILTERTYPE s_toD3DTextureFilter( ETextureFilter _filter )
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
	static int s_format_id_( D3DFORMAT _format )
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
	static void s_matIdent_( D3DMATRIX* _mtx )
	{
			_mtx->_12 = _mtx->_13 = _mtx->_14 = 
			_mtx->_21 = _mtx->_23 = _mtx->_24 = 
			_mtx->_31 = _mtx->_32 = _mtx->_34 = 
			_mtx->_41 = _mtx->_42 = _mtx->_43 = 0.0f;
			_mtx->_11 = _mtx->_22 = _mtx->_33 = _mtx->_44 = 1.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_matMul_( D3DMATRIX* _out, D3DMATRIX* _mtxl, D3DMATRIX* _mtxr )
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
	DX9RenderSystem::DX9RenderSystem()
		: m_logService( NULL )
		, m_serviceProvider( NULL )
		, m_pD3D( NULL )
		, m_pD3DDevice( NULL )
		, m_inRender( false )
		, m_curRenderTexture( NULL )
		, m_syncTemp( NULL )
		, m_syncTempTex( NULL )
		, m_screenSurf( NULL )
		, m_screenDepth( NULL )
		, m_vbHandleCounter( 0 )
		, m_ibHandleCounter( 0 )
		, m_currentIB( 0 )
		, m_listener( NULL )
	{
		m_syncTargets[0] = NULL;
		m_syncTargets[1] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderSystem::~DX9RenderSystem()
	{
		release_();
		if( m_hd3d9 != NULL )
		{
			FreeLibrary( m_hd3d9 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::initialize( ServiceProviderInterface * _serviceProvide, RenderSystemListener * _listener )
	{
		m_listener = _listener;

		m_serviceProvider = _serviceProvide;
		m_logService = m_serviceProvider->getServiceT<LogServiceInterface>("LogService");
		
		D3DADAPTER_IDENTIFIER9 AdID;
		D3DDISPLAYMODE Mode;
		D3DFORMAT Format = D3DFMT_UNKNOWN;
		UINT nModes, i;
		m_frames = 0;

		// Init D3D
		LOGGER_INFO(m_logService)( "Initializing DX9RenderSystem..." );
		m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ); // D3D_SDK_VERSION
		if( m_pD3D == NULL )
		{
			LOGGER_ERROR(m_logService)( "Error (DX9RenderSystem::initialize): Direct3DCreate9 failed" );
			return false;
		}

		m_hd3d9 = ::LoadLibraryA( "d3d9.dll" );
		if( m_hd3d9 == NULL )
		{
			LOGGER_ERROR(m_logService)( "Failed to load d3d9.dll" );
			return false;
		}

		typedef IDirect3D9* (WINAPI *PDIRECT3DCREATE9)(UINT);
		PDIRECT3DCREATE9 pDirect3DCreate9 = (PDIRECT3DCREATE9)::GetProcAddress( m_hd3d9, "Direct3DCreate9" );
		if( pDirect3DCreate9 == NULL )
		{
			LOGGER_ERROR(m_logService)( "Failed to get Direct3DCreate9 proc address" );
			return false;
		}

		// Init D3D
		LOGGER_INFO(m_logService)( "Initializing DX9RenderSystem..." );
		m_pD3D = pDirect3DCreate9( D3D_SDK_VERSION ); // D3D_SDK_VERSION
		LOGGER_INFO(m_logService)( "called pDirect3DCreate9" );
		if( m_pD3D == NULL )
		{
			LOGGER_ERROR(m_logService)( "Can't create D3D interface" );
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
			LOGGER_ERROR(m_logService)( "Can't determine adapter identifier" );
			return false;
		}

		LOGGER_ERROR(m_logService)( "D3D Driver: %s", AdID.Driver );
		LOGGER_ERROR(m_logService)( "Description: %s", AdID.Description );
		LOGGER_ERROR(m_logService)( "Version: %d.%d.%d.%d",
			HIWORD(AdID.DriverVersion.HighPart),
			LOWORD(AdID.DriverVersion.HighPart),
			HIWORD(AdID.DriverVersion.LowPart),
			LOWORD(AdID.DriverVersion.LowPart));

		// Set up Windowed presentation parameters
		if(FAILED( m_pD3D->GetAdapterDisplayMode(m_adapterToUse, &Mode)) || Mode.Format == D3DFMT_UNKNOWN ) 
		{
			LOGGER_ERROR(m_logService)( "Error (DX9RenderSystem::initialize): Can't determine desktop video mode" );
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
			if(s_format_id_(Mode.Format) > s_format_id_(Format)) Format=Mode.Format;
		}

		if(Format == D3DFMT_UNKNOWN)
		{
			LOGGER_ERROR(m_logService)( "Error (DX9RenderSystem::initialize): Can't find appropriate full screen video mode" );
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

		s_matIdent_( &m_matTexture );

		D3DCAPS9 caps;
		m_pD3D->GetDeviceCaps( m_adapterToUse, m_deviceType, &caps );
		if( ( caps.TextureCaps & D3DPTEXTURECAPS_POW2 ) == 0 
			|| ( caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL ) != 0 )
		{
			m_supportNPOT = true;
		}

		return true;
	}
	bool DX9RenderSystem::createRenderWindow( size_t _width, size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle, bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
		static const char *szFormats[]={ 
			"UNKNOWN"
			,"R5G6B5"
			,"X1R5G5B5"
			,"A1R5G5B5"
			,"X8R8G8B8"
			,"A8R8G8B8"
		};

		m_screenResolution.setWidth(_width);
		m_screenResolution.setHeight(_height);

		m_fullscreen = _fullscreen;

		d3dppW.BackBufferWidth  = _width;
		d3dppW.BackBufferHeight = _height;
		d3dppW.hDeviceWindow    = (HWND)_winHandle;
		d3dppW.BackBufferCount  = 1;
		d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;

		if( _waitForVSync == true )
		{
			d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		d3dppW.EnableAutoDepthStencil = FALSE;
		d3dppW.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
		
		d3dppFS.BackBufferWidth  = _width;
		d3dppFS.BackBufferHeight = _height;
		d3dppFS.hDeviceWindow    = (HWND)_winHandle;
		d3dppFS.BackBufferCount  = 1;

		if( _waitForVSync == true )
		{
			d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		d3dppFS.EnableAutoDepthStencil = FALSE;
		d3dppFS.AutoDepthStencilFormat = D3DFMT_UNKNOWN;

		if( _fullscreen == true )
		{
			d3dpp = &d3dppFS;
		}
		else
		{
			d3dpp = &d3dppW;
		}
		
		if( s_format_id_(d3dpp->BackBufferFormat) < 4 ) 
		{
			m_screenBits = 16;
		}
		else 
		{
			m_screenBits = 32;
		}
		
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
			LOGGER_ERROR(m_logService)( "Error (DX9RenderSystem::createRenderWindow): Can't create D3D device (hr:%d, hwnd:%d)",
				hr, (HWND)_winHandle );
			return false;
		}

		LOGGER_INFO(m_logService)( "Mode: %d x %d x %s\n"
			, _width
			, _height
			, szFormats[s_format_id_(d3dpp->BackBufferFormat)]
		);

		if(!initLost_()) 
		{
			return false;
		}

		clear( 0 );

		LOGGER_INFO(m_logService)( "DX9RenderSystem initalized successfully!" );

		return true;
	}
	//FIXME
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::screenshot( RenderImageInterface* _image, const float * _rect )
	{
		//RECT rect;
		//if( _rect )
		//{
		//	rect.left = (LONG)_rect[0];
		//	rect.top = (LONG)_rect[1];
		//	rect.right = (LONG)_rect[2];
		//	rect.bottom = (LONG)_rect[3];
		//}
		//else
		//{
		//	rect.left = 0;
		//	rect.top = 0;
		//	rect.right = m_screenResolution.getWidth();
		//	rect.bottom = m_screenResolution.getHeight();
		//}
		//DX9Texture* dxTexture = static_cast<DX9Texture*>( _image );

		//LPDIRECT3DSURFACE9 surf;
		//HRESULT hr = m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &surf );
		////HRESULT hr = m_pD3DDevice->GetFrontBuffer( m_frontBufferCopySurface );
		//if( FAILED( hr ) )
		//{
		//	LOGGER_ERROR(m_logService)( "Error (DX9RenderSystem::screenshot): failed to GetBackBuffer (hr:%d)", hr );
		//	return;
		//}

		//IDirect3DTexture9* dtext = dxTexture->getInterface();
		//LPDIRECT3DSURFACE9 dsurf;
		//D3DSURFACE_DESC desc;
		//dtext->GetLevelDesc(0, &desc);
		//hr = dtext->GetSurfaceLevel(0, &dsurf );
		//if( FAILED( hr ) )
		//{
		//	LOGGER_ERROR(m_logService)( "Error (DX9RenderSystem::screenshot): failed to GetSurfaceLevel (hr:%d)", hr );
		//	return;
		//}

		//RECT dest_rect;
		//dest_rect.top = 0;
		//dest_rect.left = 0;
		//dest_rect.right = rect.right - rect.left;
		//dest_rect.bottom = rect.bottom - rect.top;
		//if( dest_rect.right > desc.Width )
		//{
		//	dest_rect.right = desc.Width;
		//}
		//if( dest_rect.bottom > desc.Height )
		//{
		//	dest_rect.bottom = desc.Height;
		//}

		//hr = loadSurfaceFromSurface_( dsurf, &dest_rect, surf, &rect );
		//if( FAILED( hr ) )
		//{
		//	LOGGER_ERROR(m_logService)( "Error (DX9RenderSystem::screenshot): failed to loadSurfaceFromSurface_ (hr:%d)", hr );
		//}
		//surf->Release();
		RECT rect;
		if( _rect )
		{
			rect.left = (std::max)( 0L, (LONG)_rect[0] );
			rect.top = (std::max)( 0L, (LONG)_rect[1] );
			rect.right = std::min<LONG>( m_screenResolution.getWidth(), (LONG)_rect[2] );
			rect.bottom = std::min<LONG>( m_screenResolution.getHeight(), (LONG)_rect[3] );
		}
		else
		{
			rect.left = 0;
			rect.top = 0;
			rect.right = m_screenResolution.getWidth();
			rect.bottom = m_screenResolution.getHeight();
		}

		DX9Texture* dxTexture = static_cast<DX9Texture*>( _image );

		LPDIRECT3DSURFACE9 renderTarget;
		HRESULT hr;
		D3DSURFACE_DESC rtDesc;
		
		hr = m_pD3DDevice->GetRenderTarget( 0, &renderTarget );

		if( !renderTarget || FAILED(hr) )
		{
			LOGGER_ERROR(m_logService)( "D3D Error: failed to GetRenderTarget %i",hr );
			return;
		}

		renderTarget->GetDesc( &rtDesc );
		
		LPDIRECT3DSURFACE9 offscreenSurface;
		hr = m_pD3DDevice->CreateOffscreenPlainSurface( rtDesc.Width, rtDesc.Height, rtDesc.Format, D3DPOOL_SYSTEMMEM, &offscreenSurface, NULL );

		if( FAILED(hr) )
		{
			LOGGER_ERROR(m_logService)( "D3D Error: failed to CreateOffscreenPlainSurface %i",hr );
			renderTarget->Release();
			//offscreenSurface->Release();
			return;
		}
		
		hr = m_pD3DDevice->GetRenderTargetData( renderTarget, offscreenSurface );

		if(! SUCCEEDED(hr))
		{
			LOGGER_ERROR(m_logService)( "D3D Error: failed to GetRenderTargetData %i",hr );
			renderTarget->Release();
			offscreenSurface->Release();
			return;
		}
		
		IDirect3DTexture9* dtext = dxTexture->getInterface();
		LPDIRECT3DSURFACE9 dsurf;
		D3DSURFACE_DESC desc;

		dtext->GetLevelDesc(0, &desc);
		hr = dtext->GetSurfaceLevel(0, &dsurf );

		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "D3D Error: failed to GetSurfaceLevel %i",hr );
			renderTarget->Release();
			offscreenSurface->Release();
			return;
		}

		RECT dest_rect;
		dest_rect.top = 0;
		dest_rect.left = 0;
		dest_rect.right = rect.right - rect.left;
		dest_rect.bottom = rect.bottom - rect.top;

		if( (UINT)dest_rect.right > desc.Width )
		{
			dest_rect.right = desc.Width;
		}
		if( (UINT)dest_rect.bottom > desc.Height )
		{
			dest_rect.bottom = desc.Height;
		}

		hr = loadSurfaceFromSurface_( dsurf, &dest_rect, offscreenSurface, &rect );

		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "D3D Error: failed to loadSurfaceFromSurface_ %i",hr );
		}
				
		renderTarget->Release();
		offscreenSurface->Release();
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
	{
		HRESULT	hr = m_pD3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)_projection.buff() );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setProjectionMatrix (hr:%p)"
				, hr 
				);
		}
	}
	/////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX9RenderSystem::createImage( size_t _width, size_t _height, size_t & _realWidth, size_t & _realHeight, PixelFormat& _format )
	{
		if( _format == Menge::PF_R8G8B8 )
		{
			_format = Menge::PF_X8R8G8B8;
		}
		
		size_t tex_width = _width;
		size_t tex_height = _height;

		bool npot = supportNPOT_();
		if( npot == false )	// we're all gonna die
		{
			if( ( _width & ( _width - 1 ) ) != 0 || ( _height & ( _height - 1 ) ) != 0 )
			{
				tex_width = s_firstPOW2From( _width );
				tex_height = s_firstPOW2From( _height );
			}
		}

		IDirect3DTexture9* dxTextureInterface = NULL;
		
		HRESULT hr = d3dCreateTexture_( tex_width, tex_height, 1, 0, 
			_format, D3DPOOL_MANAGED, &dxTextureInterface );

		if( hr == D3DERR_INVALIDCALL )
		{
			if( _format == Menge::PF_A8 )	// try to fallback
			{
				_format = Menge::PF_A8R8G8B8;

				//D3DFORMAT dx_new_format = s_toD3DFormat( _format );

				hr = d3dCreateTexture_( tex_width, tex_height, 1, 0, 
					_format, D3DPOOL_MANAGED, &dxTextureInterface );
			}
		}

		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "DX9RenderSystem: can't create texture %dx%d %d (hr:%p)"
				, _width
				, _height
				, _format
				, hr 
				);

			return NULL;
		}

		DX9Texture* dxTexture = new DX9Texture( dxTextureInterface );

		D3DSURFACE_DESC texDesc;
		dxTextureInterface->GetLevelDesc( 0, &texDesc );

		_realWidth = texDesc.Width;
		_realHeight = texDesc.Height;

		LOGGER_INFO(m_logService)( "Texture created %dx%d %d"
			, texDesc.Width
			, texDesc.Height
			, texDesc.Format 
			);

		return dxTexture;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX9RenderSystem::createRenderTargetImage( size_t& _width, size_t& _height, size_t & _realWidth, size_t & _realHeight, PixelFormat& _format )
	{
		//if( m_supportR8G8B8 == false )
		//{
		if( _format == Menge::PF_R8G8B8 )
		{
			_format = Menge::PF_X8R8G8B8;
		}
		//}

		size_t tex_width = _width;
		size_t tex_height = _height;

		bool npot = supportNPOT_();
		if( npot == false )	// we're all gonna die
		{
			if( ( _width & ( _width - 1 ) ) != 0 || ( _height & ( _height - 1 ) ) != 0 )
			{
				tex_width = s_firstPOW2From( _width );
				tex_height = s_firstPOW2From( _height );
			}
		}

		IDirect3DTexture9 * dxTextureInterface = NULL;
		IDirect3DSurface9 * dxSurfaceInterface = NULL;

		HRESULT hr = d3dCreateTexture_( tex_width, tex_height, 1, D3DUSAGE_RENDERTARGET, 
			_format, D3DPOOL_DEFAULT, &dxTextureInterface );

		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "DX9RenderSystem: can't create RenderTexture %dx%d", _width, _height );
			return NULL;
		}

		IDirect3DSurface9* depthSurface = NULL;

		hr = m_pD3DDevice->CreateDepthStencilSurface( tex_width, tex_height,
			D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &depthSurface, NULL );

		if( FAILED( hr ) )
		{   
			dxTextureInterface->Release();
			LOGGER_ERROR(m_logService)( "Can't create render target depth buffer" );
			return NULL;
		}

		DX9RenderTexture* dxRenderTexture = new DX9RenderTexture( dxTextureInterface, depthSurface );
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
		
		dxTexture->release();
		delete dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::beginScene()
	{
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
				hr = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode);
				if( FAILED( hr ) || Mode.Format==D3DFMT_UNKNOWN) 
				{
					LOGGER_ERROR(m_logService)( "Can't determine desktop video mode" );
					return false;
				}

				d3dppW.BackBufferFormat = Mode.Format;
				if(s_format_id_(Mode.Format) < 4) 
				{
					m_screenBits = 16;
				}
				else 
				{
					m_screenBits = 32;
				}
			}

			if( this->restore_() == false )
			{
				return false;
			}
		}

		//m_pD3DDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		//m_pD3DDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
		//m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		//m_pD3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		//m_pD3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//m_pD3DDevice->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_MODULATE);

		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		//m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		
		////m_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		//m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		//m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

//		m_pD3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
//		m_pD3DDevice->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
//		m_pD3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
//		m_pD3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);

		hr = m_pD3DDevice->BeginScene();
		
		if( FAILED( hr ) )
		{
			return false;
		}

		m_inRender = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::endScene()
	{
		// sync GPU with CPU
		HRESULT hr;
		syncCPU_();
		
		hr = m_pD3DDevice->EndScene();
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to EndScene (hr:%p)"
				, hr
				);
		}

		m_inRender = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::swapBuffers()
	{
		HRESULT hr = m_pD3DDevice->Present( NULL, NULL, NULL, NULL );

		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: D3D9 failed to swap buffers" );
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
		
		HRESULT hr = m_pD3DDevice->Clear( 0, NULL, frameBufferFlags, _color, _depth, _stencil );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to clearFrameBuffer (hr:%p)"
				, hr 
				);
		}
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
	void DX9RenderSystem::setViewport( const Viewport & _viewport )
	{
		float width = _viewport.getWidth();
		float height = _viewport.getHeight();

		int x = (int)::floorf( _viewport.begin.x + 0.5f );
		int y = (int)::floorf( _viewport.begin.y + 0.5f );

		int w = (int)::floorf( width + 0.5f );
		int h = (int)::floorf( height + 0.5f );

		//int x = (int)_renderViewport.begin.x;
		//int y = (int)_renderViewport.begin.y;

		//int w = (int)width;
		//int h = (int)height;

		setViewport_( x, y, w, h );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
	{
		m_fullscreen = _fullscreen;

		d3dpp = _fullscreen ? &d3dppFS : &d3dppW;
		d3dpp->BackBufferWidth = _resolution.getWidth();
		d3dpp->BackBufferHeight = _resolution.getHeight();

		m_screenResolution = _resolution;
		//nScreenBPP = _bpp;

		if( restore_() == false )
		{
			LOGGER_ERROR(m_logService)( "Error: Graphics change mode failed\n" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setRenderTarget( RenderImageInterface* _renderTarget, bool _clear )
	{
		HRESULT hr;
		LPDIRECT3DSURFACE9 pSurf=0, pDepth=0;
		DX9RenderTexture* _target = static_cast<DX9RenderTexture*>( _renderTarget );
					
		if( _target != m_curRenderTexture )
		{
			if( _target )
			{
				_target->getInterface()->GetSurfaceLevel(0, &pSurf);
				pDepth=_target->getDepthInterface();
			}
			else
			{
				pSurf=m_screenSurf;
				pDepth=m_screenDepth;
			}
			hr = m_pD3DDevice->SetRenderTarget( 0, pSurf );

			//|| FAILED(m_pD3DDevice->SetDepthStencilSurface( pDepth ))
			if( FAILED( hr)	)
			{
				if(_target)
				{
					pSurf->Release();
				}
				LOGGER_ERROR(m_logService)( "Gfx_BeginScene: Can't set render target %l", hr );
				return;
			}
			if(_target)
			{
				pSurf->Release();
				if( _target->getDepthInterface() != NULL )
				{
					
					m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ); 
				}
				else
				{
					m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ); 
				}
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

		if( _target == NULL )
		{
			return;
		}
		if( _target->isDirty() && _clear )
		{
			clear_( 0 );
			_target->setDirty( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::supportTextureFormat( PixelFormat _format )
	{
		D3DFORMAT dxformat = s_toD3DFormat( _format ); 

		if( FAILED( m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dppW.BackBufferFormat, 0, D3DRTYPE_TEXTURE, dxformat ) ) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	/*LightInterface * DX9RenderSystem::createLight( const String & _name )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::releaseLight( LightInterface * _light )
	{
		// empty
	}*/
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::onWindowMovedOrResized()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::onWindowClose()
	{

	}
	/////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::supportNPOT_() const
	{
		return m_supportNPOT;
		//return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::syncCPU_()
	{
		////_render_batch( false );
		//m_pD3DDevice->BeginScene();

		//m_pD3DDevice->SetRenderTarget( 0, m_syncTargets[m_frames % 2] );
		////m_syncTargets[m_frames % 2]->Release();
		////setProjectionMatrix_( 2, 2 );
		////m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
		//
		//D3DMATRIX view;
		//s_matIdent_( &view );
		//m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

		//m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1 );

		//if( m_curRenderTexture )
		//{
		//	LPDIRECT3DSURFACE9 surf;
		//	m_curRenderTexture->getInterface()->GetSurfaceLevel( 0, &surf );
		//	m_pD3DDevice->SetRenderTarget( 0, surf );
		//	m_pD3DDevice->SetDepthStencilSurface( m_curRenderTexture->getDepthInterface() );
		//	surf->Release();
		//	//setProjectionMatrix_( m_curRenderTexture->getWidth(), m_curRenderTexture->getHeight() );
		//}
		//else
		//{
		//	m_pD3DDevice->SetRenderTarget( 0, m_screenSurf );
		//	m_pD3DDevice->SetDepthStencilSurface( m_sc );
		//	//setProjectionMatrix_( m_screenResolution[0], m_screenResolution[1] );
		//}

		////m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
		//s_matIdent_(&view);
		//m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);

		//HRESULT hr = loadSurfaceFromSurface_( m_syncTemp, NULL, m_syncTargets[(m_frames + 1) % 2], NULL );
		//D3DLOCKED_RECT rect;
		//hr = m_syncTemp->LockRect( &rect, NULL, D3DLOCK_READONLY );
		//m_syncTemp->UnlockRect();

		//m_pD3DDevice->EndScene();
		
		if( m_syncReady == true )
		{
			HRESULT hr = m_pD3DDevice->SetRenderTarget( 0, m_syncTargets[m_frames % 2] );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to SetRenderTarget (hr:%p)"
					, hr 
					);
			}
			//m_syncTargets[m_frames % 2]->Release();
			//setProjectionMatrix_( 2, 2 );
			//m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

			D3DMATRIX view;
			s_matIdent_( &view );
			hr = m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to SetTransform (hr:%p)"
					, hr 
					);
			}
			
			/*
			hr = m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1 );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to DrawIndexedPrimitive (hr:%p)"
					, hr 
					);
			}*/
			if( m_curRenderTexture )
			{
				LPDIRECT3DSURFACE9 surf;
				m_curRenderTexture->getInterface()->GetSurfaceLevel( 0, &surf );
				hr = m_pD3DDevice->SetRenderTarget( 0, surf );
				if( FAILED( hr ) )
				{
					LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to SetRenderTarget (hr:%p)"
						, hr 
						);
				}
				
				hr = m_pD3DDevice->SetDepthStencilSurface( m_curRenderTexture->getDepthInterface() );
				if( FAILED( hr ) )
				{
					LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to SetDepthStencilSurface (hr:%p)"
						, hr 
						);
				}
				surf->Release();
				//setProjectionMatrix_( m_curRenderTexture->getWidth(), m_curRenderTexture->getHeight() );
			}
			else
			{
				hr = m_pD3DDevice->SetRenderTarget( 0, m_screenSurf );
				if( FAILED( hr ) )
				{
					LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to SetRenderTarget (hr:%p)"
						, hr 
						);
				}
				hr = m_pD3DDevice->SetDepthStencilSurface( m_screenDepth );
				if( FAILED( hr ) )
				{
					LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to SetDepthStencilSurface (hr:%p)"
						, hr 
						);
				}
				//setProjectionMatrix_( m_screenResolution[0], m_screenResolution[1] );
			}
			
			//m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
			s_matIdent_(&view);
			hr = m_pD3DDevice->SetTransform(D3DTS_VIEW, &view);
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to SetTransform (hr:%p)"
					, hr 
					);
			}

			hr = loadSurfaceFromSurface_( m_syncTemp, NULL, m_syncTargets[(m_frames + 1) % 2], NULL );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to loadSurfaceFromSurface_ (hr:%p)"
					, hr 
					);
			}

			D3DLOCKED_RECT rect;
			hr = m_syncTemp->LockRect( &rect, NULL, D3DLOCK_READONLY );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to LockRect (hr:%p)"
					, hr 
					);
			}

			hr = m_syncTemp->UnlockRect();
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::syncCPU_ failed to UnlockRect (hr:%p)"
					, hr 
					);
			}
		}
		else	// just lock backbuffer
		{
			LPDIRECT3DSURFACE9 surf;
			HRESULT hr = m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &surf );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "D3D Error: failed to GetBackBuffer" );
				return;
			}
			D3DLOCKED_RECT lockRect;
			surf->LockRect( &lockRect, NULL, D3DLOCK_READONLY );
			surf->UnlockRect();
			surf->Release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::initLost_()
	{
		HRESULT hr;
		// Store render target
		m_screenSurf = NULL;
		m_screenDepth = NULL;

		m_pD3DDevice->GetRenderTarget(0, &m_screenSurf);
		m_pD3DDevice->GetDepthStencilSurface(&m_screenDepth);

		createSyncTargets_();

		for( TMapVBInfo::iterator 
			it = m_vertexBuffers.begin(), 
			it_end = m_vertexBuffers.end();
		it != it_end;
		++it )
		{
			VBInfo& vbInfo = it->second;
			hr = m_pD3DDevice->CreateVertexBuffer( vbInfo.length, vbInfo.usage, 
				vbInfo.fvf, vbInfo.pool, &vbInfo.pVB, NULL );

			if( FAILED( hr ) )
			{
				return false;
			}
		}

		for( TMapIBInfo::iterator 
			it = m_indexBuffers.begin(), 
			it_end = m_indexBuffers.end();
		it != it_end;
		++it )
		{
			IBInfo& ibInfo = it->second;
			hr = m_pD3DDevice->CreateIndexBuffer( ibInfo.length, ibInfo.usage, ibInfo.format,
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
	void DX9RenderSystem::createSyncTargets_()
	{
		// sync surfaces
		D3DFORMAT fmt = D3DFMT_X8R8G8B8;
		UINT w = 2;
		UINT d = 1;
		m_syncReady = false;
				
		HRESULT hr = m_pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, 0, TRUE, &(m_syncTargets[0]), NULL );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::init_lost_ failed to CreateRenderTarget (hr:%p)"
				, hr 
				);
			return;
		}

		hr = m_pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, 0, TRUE, &(m_syncTargets[1]), NULL );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::init_lost_ failed to CreateRenderTarget (hr:%p)"
				, hr 
				);
			return;
		}
		
		m_pD3DDevice->CreateTexture( w, w, d, 0, fmt, D3DPOOL_SYSTEMMEM, &m_syncTempTex, NULL );
		
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::init_lost_ failed to d3dCreateTexture_ (hr:%p)"
				, hr 
				);
			return;
		}
		
		hr = m_syncTempTex->GetSurfaceLevel( 0, &m_syncTemp );
		
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::init_lost_ failed to GetSurfaceLevel (hr:%p)"
				, hr 
				);
			return;
		}
		m_syncReady = true;
	}
	//////////////////////////////////////////////////////////////////////////
	HRESULT DX9RenderSystem::d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,  DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * ppTexture )
	{
		D3DFORMAT dx_format = s_toD3DFormat( Format );
		HRESULT hr = m_pD3DDevice->CreateTexture( Width, Height, MipLevels, Usage, dx_format, Pool, ppTexture, NULL );
		return hr;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clear( uint32 _color )
	{
		clear_(_color);
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureMatrix( size_t _stage, const float* _texture )
	{
		HRESULT hr;
		if( _texture != NULL )
		{
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::setTextureMatrix failed to SetTextureStageState (hr:%p)"
					, hr 
					);
			}

			D3DTRANSFORMSTATETYPE level = static_cast<D3DTRANSFORMSTATETYPE>( static_cast<DWORD>( D3DTS_TEXTURE0 ) + _stage );
			hr = m_pD3DDevice->SetTransform( level, (const D3DMATRIX*)_texture );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::setTextureMatrix failed to SetTransform (hr:%p)"
					, hr 
					);
			}
		}
		else
		{
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::setTextureMatrix failed to SetTextureStageState (hr:%p)"
					, hr 
					);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setModelViewMatrix( const mt::mat4f & _view )
	{
		HRESULT	hr = m_pD3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)_view.buff() );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setModelViewMatrix (hr:%p)"
				, hr 
				);
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
	void DX9RenderSystem::release_()
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

		if(m_screenSurf) 
		{ 
			useCount = m_screenSurf->Release();
			m_screenSurf=0; 
		}
		if(m_screenDepth) 
		{ 
			useCount = m_screenDepth->Release();
			m_screenDepth=0; 
		}

		if( m_pD3DDevice )
		{
			m_pD3DDevice->SetStreamSource( 0, NULL, 0, 0 );
			m_pD3DDevice->SetIndices(NULL);
		}

		for( TMapVBInfo::iterator 
			it = m_vertexBuffers.begin(),
			it_end = m_vertexBuffers.end();
		it != it_end;
		++it )
		{
			useCount = it->second.pVB->Release();
			/*if(useCount != 0)
			{
				int debug = 1;
			}*/
		}

		for( TMapIBInfo::iterator 
			it = m_indexBuffers.begin(),
			it_end = m_indexBuffers.end();
		it != it_end;
		++it )
		{
			useCount = it->second.pIB->Release();
			/*if(useCount != 0)
			{
				int debug = 1;
			}*/
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
	void DX9RenderSystem::onRestoreDevice_()
	{
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
		TMapVBInfo::iterator it_find = m_vertexBuffers.find( _vbHandle );
		if( it_find != m_vertexBuffers.end() )
		{
			it_find->second.pVB->Release();
			m_vertexBuffers.erase( it_find );
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
	void DX9RenderSystem::setVertexBuffer( VBHandle _vbHandle )
	{
		if( _vbHandle == 0 )
		{
			return;
		}

		TMapVBInfo::const_iterator it_found = m_vertexBuffers.find(_vbHandle);

		if( it_found == m_vertexBuffers.end() )
		{
			return;
		}

		const VBInfo & vbInfo = it_found->second;

		HRESULT hr = m_pD3DDevice->SetStreamSource( 0, vbInfo.pVB, 0, vbInfo.vertexSize );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::setVertexBuffer failed to SetStreamSource (hr:%p)"
				, hr 
				);
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
		IBHandle newIBHandleCounter = ++m_ibHandleCounter;
		m_indexBuffers.insert( std::make_pair( newIBHandleCounter, ibInfo ) );

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
	uint16* DX9RenderSystem::lockIndexBuffer( IBHandle _ibHandle )
	{
		IDirect3DIndexBuffer9* ib = m_indexBuffers[_ibHandle].pIB;
		//void* lock = NULL;
		uint16* lock = NULL;
		HRESULT hr = ib->Lock( 0, 0, (void**)&lock, 0 );
		
		if( FAILED( hr ) )
		{
			lock = NULL;
		}
		
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
	void DX9RenderSystem::setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex )
	{
		if( _ibHandle == 0 )
		{
			return;
		}

		TMapIBInfo::const_iterator it_found = m_indexBuffers.find(_ibHandle);

		if( it_found == m_indexBuffers.end() )
		{
			return;
		}

		IDirect3DIndexBuffer9* ib = it_found->second.pIB;
				
		HRESULT hr = m_pD3DDevice->SetIndices( ib );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setIndexBuffer (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::drawIndexedPrimitive( EPrimitiveType _type, std::size_t _baseVertexIndex,
		std::size_t _minIndex, std::size_t _verticesNum, std::size_t _startIndex, std::size_t _indexCount )
	{
		
		UINT primCount = s_getPrimitiveCount( _type, _indexCount );
		D3DPRIMITIVETYPE primitiveType = s_toD3DPrimitiveType( _type );

		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive(
			primitiveType, 
			_baseVertexIndex,
			_minIndex,
			_verticesNum,
			_startIndex,
			primCount );
		
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to DrawIndexedPrimitive (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTexture( size_t _stage, RenderImageInterface* _texture )
	{
		HRESULT hr;
		IDirect3DTexture9* d3d9Texture = NULL;
		if( _texture != NULL )
		{
			DX9Texture* t = static_cast<DX9Texture*>( _texture );
			d3d9Texture = t->getInterface();
		}
		hr = m_pD3DDevice->SetTexture( _stage, d3d9Texture );
		
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to SetTexture (hr:%p)"
				, hr 
				);
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setSrcBlendFactor( EBlendFactor _src )
	{
		//!COMMENT return;
		DWORD factor = s_toD3DBlend( _src );

		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, factor );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setBlendFactor (hr:%d)", hr );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDstBlendFactor( EBlendFactor _dst )
	{
		//!COMMENT return;
		DWORD factor = s_toD3DBlend( _dst );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, factor );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setBlendFactor (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
		HRESULT hr;
		D3DTEXTUREADDRESS adrU = s_toD3DTextureAddress( _modeU );
		D3DTEXTUREADDRESS adrV = s_toD3DTextureAddress( _modeV );
		//hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ADDRESSU, adrU );
		hr = m_pD3DDevice->SetSamplerState( _stage, D3DSAMP_ADDRESSU, adrU );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureAddressing (hr:%p)"
				, hr 
				);
		}

		//hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ADDRESSV, adrV );
		hr = m_pD3DDevice->SetSamplerState( _stage, D3DSAMP_ADDRESSV, adrV );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureAddressing (hr:%p)"
				, hr 
				);
		}

		/*
		if( m_addressU[_stage] != adrU )
		{
			m_addressU[_stage] = adrU;
			hr = m_pD3DDevice->SetSamplerState( _stage, D3DSAMP_ADDRESSU, adrU );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureAddressing (hr:%d)", hr );
			}
		}
		if( m_addressV[_stage] != adrV )
		{
			m_addressV[_stage] = adrV;
			hr = m_pD3DDevice->SetSamplerState( _stage, D3DSAMP_ADDRESSV, adrV );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureAddressing (hr:%d)", hr );
			}
		}
		*/
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureFactor( uint32 _color )
	{
		//!COMMENT return;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, _color );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureFactor (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float DX9RenderSystem::getTexelOffsetX() const
	{
		//return 0.5f;
		return -0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
	float DX9RenderSystem::getTexelOffsetY() const
	{
		//return 0.5f;
		return -0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setCullMode( ECullMode _mode )
	{
		D3DCULL mode = s_toD3DCullMode( _mode );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, mode );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setCullMode (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferTestEnable( bool _depthTest )
	{
		D3DZBUFFERTYPE test = _depthTest ? D3DZB_TRUE : D3DZB_FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, test );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setDepthBufferTestEnable (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
	{
		DWORD dWrite = _depthWrite ? TRUE : FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, dWrite );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setDepthBufferWriteEnable (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		D3DCMPFUNC func = s_toD3DCmpFunc( _depthFunction );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ZFUNC, func );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setDepthBufferCmpFunc (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setFillMode( EFillMode _mode )
	{
		D3DFILLMODE mode = s_toD3DFillMode( _mode );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_FILLMODE, mode );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setFillMode (hr:%p)"
				, hr 
				);
		}
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

		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE, value );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setColorBufferWriteEnable (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setShadeType( EShadeType _sType )
	{
		D3DSHADEMODE mode = s_toD3DShadeMode( _sType );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_SHADEMODE, mode );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setShadeType (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaTestEnable( bool _alphaTest )
	{
		//!COMMENT  return;
		DWORD alphaTest = _alphaTest ? TRUE : FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, alphaTest );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setAlphaTestEnable (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
	{
		//!COMMENT return;
		DWORD alphaBlend = _alphaBlend ? TRUE : FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, alphaBlend );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setAlphaBlendEnable (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha )
	{
		//!COMMENT return;
		D3DCMPFUNC func = s_toD3DCmpFunc( _alphaFunc );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, func );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setAlphaCmpFunc (hr:%p)"
				, hr 
				);
		}

		DWORD alpha = _alpha;
		hr = m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF, alpha );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setAlphaCmpFunc (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setLightingEnable( bool _light )
	{
		DWORD value = _light ? TRUE : FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, value );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setLightingEnable (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		//return;
		HRESULT hr;
		D3DTEXTUREOP colorOp = s_toD3DTextureOp( _textrueOp );
		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		DWORD arg2 = s_toD3DTextureArg( _arg2 );

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLOROP, colorOp );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureStageColorOp (hr:%p)"
				, hr 
				);
		}

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLORARG1, arg1 );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureStageColorOp (hr:%p)"
				, hr 
				);
		}

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLORARG2, arg2 );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureStageColorOp (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp
													,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		//return;
		HRESULT hr;
		D3DTEXTUREOP alphaOp = s_toD3DTextureOp( _textrueOp );
		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		DWORD arg2 = s_toD3DTextureArg( _arg2 );
		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAOP, alphaOp );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureStageAlphaOp (hr:%d)", hr );
		}

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAARG1, arg1 );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureStageAlphaOp (hr:%d)", hr );
		}
		
		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAARG2, arg2 );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureStageAlphaOp (hr:%d)", hr );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter )
	{
		//return;
		/*D3DTEXTURESTAGESTATETYPE textureFilterType = s_toD3DTextureFilterType( _filterType );
		D3DTEXTUREFILTERTYPE textureFilter = s_toD3DTextureFilter( _filter );

		HRESULT hr = m_pD3DDevice->SetTextureStageState( _stage, textureFilterType, textureFilter );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setTextureStageFilter (hr:%p)"
				, hr 
				);
		}*/
		D3DSAMPLERSTATETYPE textureFilterType = s_toD3DTextureFilterType( _filterType );
		D3DTEXTUREFILTERTYPE textureFilter = s_toD3DTextureFilter( _filter );
		m_pD3DDevice->SetSamplerState( _stage, textureFilterType, textureFilter );							
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setVertexDeclaration( size_t _vertexSize, uint32 _declaration )
	{
		HRESULT hr = m_pD3DDevice->SetFVF( _declaration );
		//HRESULT hr = m_pD3DDevice->SetVertexShader( _declaration );
		if( FAILED( hr ) )
		{	
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem failed to setVertexDeclaration (hr:%p)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::getResolutions( TVectorResolutions & _resolutions )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::restore_()
	{
		HRESULT hr;
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
		if(m_screenSurf) m_screenSurf->Release();
		if(m_screenDepth) m_screenDepth->Release();


		hr = m_pD3DDevice->SetIndices(NULL);
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::restore failed to SetIndices (hr:%p)"
				, hr 
				);
		}
		hr = m_pD3DDevice->SetStreamSource( 0, NULL, 0, 0 );
		if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::restore failed to SetStreamSource (hr:%p)"
				, hr 
				);
		}
		for( TMapVBInfo::iterator it = 
			m_vertexBuffers.begin(), 
			it_end = m_vertexBuffers.end();
		it != it_end;
		++it )
		{
			it->second.pVB->Release();
		}

		for( TMapIBInfo::iterator it = 
			m_indexBuffers.begin(), 
			it_end = m_indexBuffers.end();
		it != it_end;
		++it )
		{
			it->second.pIB->Release();
		}

		hr = m_pD3DDevice->Reset(d3dpp);
		if( FAILED( hr ) )
		{
			return false;
		}

		if( initLost_() == false )
		{
			return false;
		}

		onRestoreDevice_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setVSync( bool _vSync )
	{
		//d3dppW.SwapEffect = _vSync ? D3DSWAPEFFECT_COPY_VSYNC : D3DSWAPEFFECT_COPY;
		d3dppFS.PresentationInterval = _vSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		restore_();
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clear_( DWORD _color )
	{
		HRESULT	 hr;
		
		////pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		//if( m_curRenderTexture )
		//{
		//	if( m_curRenderTexture->getDepthInterface() )
		//	{
		//		m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
		//	}
		//	else
		//	{
		//		m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, _color, 1.0f, 0 );
		//	}
		//}
		//else
		//{
		//	//if( bZBuffer )
		//	{
		//		m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
		//	}
		//	/*else
		//	{
		//		m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
		//	}*/
		//}
		
		if( m_curRenderTexture )
		{
			//if( m_curRenderTexture->getDepthInterface() )
			//{
			//	hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
			//	if( FAILED( hr ) )
			//	{
			//		LOGGER_ERROR(m_logSystem)( "Error: DX8RenderSystem::clear failed to Clear (hr:%p)"
			//			, hr 
			//			);
			//	}
			//}
			//else
			{
				//hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, _color, 0.0f, 0 );
				//DWORD color = D3DCOLOR_ARGB(0, 0, 0, 0);
				hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 0.0f, 0 );
				
				if( FAILED( hr ) )
				{
					LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::clear failed to Clear (hr:%p)"
						, hr 
						);
				}
			}
		}
		else
		{
			//if( bZBuffer )
			{
				//hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
				//if( FAILED( hr ) )
				//{
				//	LOGGER_ERROR(m_logSystem)( "Error: DX8RenderSystem::clear failed to Clear (hr:%p)"
				//		, hr 
				//		);
				//}
			}
			/*else
			{
			m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
			}*/
			
			//hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 255, 255, 255), 0.0f, 0 );
			hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, _color, 0.0f, 0 );
			if( FAILED( hr ) )
			{
				LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::clear failed to Clear (hr:%p)"
					, hr 
					);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setViewport_( int _x, int _y, int _w, int _h )
	{
		if( _x == _y == _w == _h )
		{
			return;
		}

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
			LOGGER_ERROR(m_logService)( "Error: D3D9 failed to SetViewport [%d %d %d %d]"
				, _x
				, _y
				, _w
				, _h
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setSeparateAlphaBlendMode()
	{		
		//m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		//m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		//m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		//m_pD3DDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		//m_pD3DDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
		//m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		//m_pD3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		//m_pD3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//m_pD3DDevice->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_MODULATE);

		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		//m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		

		//m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);

		//m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		//m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		
		m_pD3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE ); 
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA );
	}

	void DX9RenderSystem::makeProjectionOrthogonal( mt::mat4f & _projectionMatrix , float _left, float _right , float _top, float _bottom , float _near, float _far )
	{
		float inv_lr = 1.0f / ( _left - _right );
		float inv_bt = 1.0f / ( _top - _bottom );
		float inv_znzf = 1.0f / ( _near - _far );

		float * _outMatrix = _projectionMatrix.buff();

		_outMatrix[0] = -2.0f * inv_lr;
		_outMatrix[1] = 0.0f;
		_outMatrix[2] = 0.0f;
		_outMatrix[3] = 0.0f;
		_outMatrix[4] = 0.0f;
		_outMatrix[5] = -2.0f * inv_bt;
		_outMatrix[6] = 0.0f;
		_outMatrix[7] = 0.0f;
		_outMatrix[8] = 0.0f;
		_outMatrix[9] = 0.0f;
		_outMatrix[10] = -1.0f * inv_znzf;
		_outMatrix[11] = 0.0f;
		_outMatrix[12] = ( _left + _right ) * inv_lr - 0.5f * _outMatrix[0];
		_outMatrix[13] = ( _top + _bottom ) * inv_bt - 0.5f * _outMatrix[5];
		_outMatrix[14] = _near * inv_znzf;
		_outMatrix[15] = 1.0f;
	}

	void DX9RenderSystem::setWorldMatrix( const mt::mat4f & _view )
	{

	}

	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
