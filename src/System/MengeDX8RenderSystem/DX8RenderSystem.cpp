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

#	include <algorithm>
#	include <cassert>
#	include <cmath>

//#	include <d3dx8.h>

#	ifndef MENGE_MASTER_RELEASE
#		define MENGE_LOG log
#	else
#		define MENGE_LOG
#	endif

#	define MENGE_LOG_ERROR log_error

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
	typedef IDirect3D8* (WINAPI *PDIRECT3DCREATE8)(UINT);

	static const D3DFORMAT D32SFormats[]	= { D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D15S1, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16, (D3DFORMAT) 0 };
	static const D3DFORMAT D32Formats[]		= { D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D16, D3DFMT_D15S1, (D3DFORMAT) 0 };
	static const D3DFORMAT D16SFormats[]	= { D3DFMT_D15S1, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D16, D3DFMT_D32, D3DFMT_D24X8, (D3DFORMAT) 0 };
	static const D3DFORMAT D16Formats[]		= { D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D24X4S4, (D3DFORMAT) 0 };
	//////////////////////////////////////////////////////////////////////////
	static std::size_t s_getPrimitiveCount( EPrimitiveType _pType, std::size_t _indexCount )
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
	static Menge::uint32 s_firstPO2From( Menge::uint32 _n )
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
	static D3DTEXTURESTAGESTATETYPE s_toD3DTextureFilterType( ETextureFilterType _filterType )
	{
		switch( _filterType )
		{
		case TFT_MAGNIFICATION:
			return D3DTSS_MAGFILTER;
		case TFT_MINIFICATION:
			return D3DTSS_MINFILTER;
		case TFT_MIPMAP:
			return D3DTSS_MIPFILTER;
		}
		return D3DTSS_MAGFILTER;
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
		case TF_FLATCUBIC:
			return D3DTEXF_FLATCUBIC;
		case TF_GAUSSIANCUBIC:
			return D3DTEXF_GAUSSIANCUBIC;
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
	static D3DFORMAT s_findMatchingZFormat( IDirect3D8* _pD3D8, UINT _adapter, D3DDEVTYPE _devtype, D3DFORMAT _bbufferfmt, bool _stencil )
	{
		assert( _pD3D8 );
		int fmtID = s_format_id_( _bbufferfmt );
		if( fmtID == 0 )
		{
			return static_cast<D3DFORMAT>( 0 );
		}
		const D3DFORMAT *pFormatList;
		if( fmtID > 3 )	// 32bit
		{
			pFormatList = _stencil ? D32SFormats : D32Formats;
		}
		else
		{
			pFormatList = _stencil ? D16SFormats : D16Formats;
		}

		while( *pFormatList != 0 )
		{
			// Does this depth format exist on this card, and can it be used in conjunction with the specified rendertarget format?
			if( SUCCEEDED( _pD3D8->CheckDeviceFormat( _adapter, _devtype, _bbufferfmt, 
				D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, *pFormatList ) ) )
			{
				if ( SUCCEEDED( _pD3D8->CheckDepthStencilMatch( 
					_adapter, _devtype,	_bbufferfmt, _bbufferfmt, *pFormatList ) ) )
				{
					break;
				}
			}
			pFormatList++;
		}

		return *pFormatList;

	}
	//////////////////////////////////////////////////////////////////////////
	DX8RenderSystem::DX8RenderSystem()
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
		, m_listener( NULL )
		, m_supportNPOT( false )
		, m_syncReady( false )
	{
		m_syncTargets[0] = NULL;
		m_syncTargets[1] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	DX8RenderSystem::~DX8RenderSystem()
	{
		release_();
		if( m_hd3d8 != NULL )
		{
			FreeLibrary( m_hd3d8 );
		}
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

		m_hd3d8 = ::LoadLibraryW( L"d3d8.dll" );
		if( m_hd3d8 == NULL )
		{
			MENGE_LOG_ERROR( "Failed to load d3d8.dll" );
			return false;
		}

		PDIRECT3DCREATE8 pDirect3DCreate8 = (PDIRECT3DCREATE8)::GetProcAddress( m_hd3d8, "Direct3DCreate8" );
		if( pDirect3DCreate8 == NULL )
		{
			MENGE_LOG_ERROR( "Failed to get Direct3DCreate8 proc address" );
			return false;
		}

		// Init D3D
		MENGE_LOG( "Initializing DX8RenderSystem..." );
		m_pD3D = pDirect3DCreate8( D3D_SDK_VERSION ); // D3D_SDK_VERSION
		if( m_pD3D == NULL )
		{
			MENGE_LOG_ERROR( "Can't create D3D interface" );
			return false;
		}

		// Get adapter info
		m_pD3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, D3DENUM_NO_WHQL_LEVEL, &AdID );

		MENGE_LOG( "VendorId: %d", AdID.VendorId  );
		MENGE_LOG( "DeviceId: %d", AdID.DeviceId );
		MENGE_LOG( "D3D Driver: %s", AdID.Driver );
		MENGE_LOG( "Description: %s", AdID.Description );

		MENGE_LOG( "Version: %d.%d.%d.%d"
			, HIWORD(AdID.DriverVersion.HighPart)
			, LOWORD(AdID.DriverVersion.HighPart)
			, HIWORD(AdID.DriverVersion.LowPart)
			, LOWORD(AdID.DriverVersion.LowPart)
			);

		// Set up Windowed presentation parameters
		if(FAILED( m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN ) 
		{
			MENGE_LOG_ERROR( "Can't determine desktop video mode" );
			return false;
		}

		UINT screenWidth = Mode.Width;
		UINT screenHeight = Mode.Height;

		ZeroMemory( &d3dppW, sizeof(d3dppW) );
		d3dppW.BackBufferFormat = Mode.Format;

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

			if(Mode.Width != screenWidth || Mode.Height != screenHeight) 
			{
				continue;
			}

			//if(nScreenBPP==16 && (_format_id(Mode.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
			if( s_format_id_(Mode.Format) > s_format_id_(Format) )
			{
				Format=Mode.Format;
			}
		}

		if( Format == D3DFMT_UNKNOWN )
		{
			MENGE_LOG_ERROR( "Can't find appropriate full screen video mode" );
			return false;
		}

		ZeroMemory( &d3dppFS, sizeof(d3dppFS) );
		d3dppFS.BackBufferFormat = Format;

		s_matIdent_( &m_matTexture );

		D3DCAPS8 caps;
		m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );

		if( ( ( caps.TextureCaps & D3DPTEXTURECAPS_POW2 ) == 0 )
			/*|| ( ( caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL ) != 0 )*/ )
		{
			m_supportNPOT = true;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits, 
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

		m_screenResolution[0] = _width;
		m_screenResolution[1] = _height;
		m_fullscreen = _fullscreen;

		d3dppW.MultiSampleType  = D3DMULTISAMPLE_NONE;
		d3dppW.Windowed         = TRUE;
		d3dppW.Flags			= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		d3dppW.BackBufferWidth  = _width;
		d3dppW.BackBufferHeight = _height;
		d3dppW.BackBufferCount  = 1;

		d3dppW.hDeviceWindow    = (HWND)_winHandle;

		d3dppW.SwapEffect = _waitForVSync ? D3DSWAPEFFECT_COPY_VSYNC : D3DSWAPEFFECT_COPY;
		d3dppW.FullScreen_PresentationInterval = 0;

		d3dppW.EnableAutoDepthStencil = TRUE;
		d3dppW.AutoDepthStencilFormat = 
			s_findMatchingZFormat( m_pD3D, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dppW.BackBufferFormat, false );
		d3dppW.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		d3dppFS.MultiSampleType  = D3DMULTISAMPLE_NONE;
		d3dppFS.Windowed         = FALSE;
		d3dppFS.Flags			 = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		d3dppFS.BackBufferWidth  = _width;
		d3dppFS.BackBufferHeight = _height;
		d3dppFS.BackBufferCount  = 1;

		d3dppFS.hDeviceWindow    = (HWND)_winHandle;

		d3dppFS.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dppFS.FullScreen_PresentationInterval = _waitForVSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

		d3dppFS.EnableAutoDepthStencil = TRUE;
		d3dppFS.AutoDepthStencilFormat = 
			s_findMatchingZFormat( m_pD3D, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dppFS.BackBufferFormat, false );

		d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		d3dpp = _fullscreen ? &d3dppFS : &d3dppW;

		if( s_format_id_(d3dpp->BackBufferFormat) < 4 ) 
		{
			m_screenBits = 16;
		}
		else 
		{
			m_screenBits = 32;
		}

		//_fullscreen ? MENGE_LOG( "fullscreen mode" ) : MENGE_LOG( "windowed mode" );

		//MENGE_LOG( "attempting to create d3ddevice: %dx%dx%d\nBackbuffer format %d\nDepthstencil format %d",
		//	d3dpp->BackBufferWidth, d3dpp->BackBufferHeight, m_screenBits, d3dpp->BackBufferFormat, d3dpp->AutoDepthStencilFormat );

		// Create D3D Device
		HRESULT hr;

		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)_winHandle,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
			d3dpp, &m_pD3DDevice );

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)_winHandle,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
				d3dpp, &m_pD3DDevice );
		}

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)_winHandle,
				D3DCREATE_MIXED_VERTEXPROCESSING |D3DCREATE_FPU_PRESERVE ,
				d3dpp, &m_pD3DDevice );

			if( FAILED( hr ) )
			{
				Sleep( 100 );
				hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)_winHandle,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
					d3dpp, &m_pD3DDevice );
			}
		}

		if( FAILED ( hr ) )
		{
			MENGE_LOG_ERROR( "Can't create D3D device (hr:%d, hwnd:%d)"
				, hr
				, (HWND)_winHandle 
				);

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

		MENGE_LOG( "Mode: %d x %d x %s\n"
			, _width
			, _height
			, szFormats[s_format_id_(d3dpp->BackBufferFormat)]
		);


		// Init all stuff that can be lost

		if(!init_lost_()) 
		{
			return false;
		}

		clear( 0 );

		MENGE_LOG( "DX8RenderSystem initalized successfully!" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::screenshot( RenderImageInterface* _image, const float * _rect )
	{
		RECT rect;
		if( _rect )
		{
			rect.left = (std::max)( 0L, (LONG)_rect[0] );
			rect.top = (std::max)( 0L, (LONG)_rect[1] );
			rect.right = std::min<LONG>( m_screenResolution[0], (LONG)_rect[2] );
			rect.bottom = std::min<LONG>( m_screenResolution[1], (LONG)_rect[3] );
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
			MENGE_LOG_ERROR( "D3D Error: failed to GetBackBuffer" );
			return;
		}

		IDirect3DTexture8* dtext = dxTexture->getInterface();
		LPDIRECT3DSURFACE8 dsurf;
		D3DSURFACE_DESC desc;
		dtext->GetLevelDesc(0, &desc);
		hr = dtext->GetSurfaceLevel(0, &dsurf );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "D3D Error: failed to GetSurfaceLevel" );
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

		hr = loadSurfaceFromSurface_( dsurf, &dest_rect, surf, &rect );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "D3D Error: failed to loadSurfaceFromSurface_" );
		}
		surf->Release();
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setProjectionMatrix( const float * _projection )
	{
		//std::copy( _projection, _projection + 16, &(m_matProj._11) );
		HRESULT	hr = m_pD3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)_projection );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setProjectionMatrix (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setModelViewMatrix( const float * _modelview )
	{
		//std::copy( _view, _view + 16, &(m_matView._11) );
		HRESULT	hr = m_pD3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)_modelview );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setViewMatrix (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * DX8RenderSystem::createImage( std::size_t& _width, std::size_t& _height, PixelFormat& _format )
	{
		MENGE_LOG( "Creating texture %dx%d %d"
			, _width
			, _height
			, _format 
			);

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

		D3DFORMAT dx_format = s_toD3DFormat( _format );

		HRESULT hr = d3dCreateTexture_( tex_width, tex_height, 1, 0, 
			dx_format, D3DPOOL_MANAGED, &dxTextureInterface );

		if( hr == D3DERR_INVALIDCALL )
		{
			if( _format == Menge::PF_A8 )	// try to fallback
			{
				_format = Menge::PF_A8R8G8B8;

				D3DFORMAT dx_format = s_toD3DFormat( _format );

				hr = d3dCreateTexture_( tex_width, tex_height, 1, 0, 
					dx_format, D3DPOOL_MANAGED, &dxTextureInterface );
			}
		}


		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "DX8RenderSystem: can't create texture %dx%d %d (hr:%d)"
				, _width
				, _height
				, _format
				, hr 
				);

			return NULL;
		}

		DX8Texture* dxTexture = new DX8Texture( dxTextureInterface );

		_width = tex_width;
		_height = tex_height;

		D3DSURFACE_DESC texDesc;
		dxTextureInterface->GetLevelDesc( 0, &texDesc );

		MENGE_LOG( "Texture created %dx%d %d"
			, texDesc.Width
			, texDesc.Height
			, texDesc.Format 
			);

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
			MENGE_LOG_ERROR( "DX8RenderSystem: can't create RenderTexture %dx%d"
				, _width
				, _height 
				);

			return NULL;
		}

		IDirect3DSurface8* depthSurface = NULL;
		hr = m_pD3DDevice->GetDepthStencilSurface( &depthSurface );

		D3DSURFACE_DESC depthDesc;
		depthSurface->GetDesc( &depthDesc );

		if( tex_width != depthDesc.Width
			|| tex_height != depthDesc.Height )
		{
			depthSurface->Release();
			depthSurface = NULL;

			hr = m_pD3DDevice->CreateDepthStencilSurface( tex_width, tex_height,
				D3DFMT_D16, D3DMULTISAMPLE_NONE, &depthSurface );

			if( FAILED( hr ) )
			{   
				dxTextureInterface->Release();
				MENGE_LOG_ERROR( "Can't create render target depth buffer" );
				return NULL;
			}
		}

		DX8RenderTexture* dxRenderTexture = new DX8RenderTexture( dxTextureInterface, depthSurface );

		_width = tex_width;
		_height = tex_height;

		m_renderTextureList.push_back( dxRenderTexture );

		return dxRenderTexture;
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
	bool DX8RenderSystem::beginScene()
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
					MENGE_LOG_ERROR( "Can't determine desktop video mode" );
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

			if( restore_() == false )
			{
				return false;
			}
		}

		hr = m_pD3DDevice->BeginScene();
		if( FAILED( hr ) )
		{
			return false;
		}

		m_inRender = true;
		// set render targets dirty to clear one time before rendering into one
		for( TRenderTextureList::iterator it = m_renderTextureList.begin(), it_end = m_renderTextureList.end();
			it != it_end;
			it++ )
		{
			(*it)->setDirty( true );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::endScene()
	{
		// sync GPU with CPU
		syncCPU_();

		HRESULT hr = m_pD3DDevice->EndScene();
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to EndScene (hr:%d)"
				, hr
				);
		}

		m_inRender = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::swapBuffers()
	{
		HRESULT hr = m_pD3DDevice->Present( NULL, NULL, NULL, NULL );

		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: D3D8 failed to swap buffers" );
		}

		++m_frames;
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
		HRESULT hr = m_pD3DDevice->Clear( 0, NULL, frameBufferFlags, _color, _depth, _stencil );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to clearFrameBuffer (hr:%d)"
				, hr 
				);
		}
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
	void DX8RenderSystem::setRenderViewport( const Viewport & _renderViewport )
	{
		float w = _renderViewport.getWidth();
		float h = _renderViewport.getHeight();

		set_clipping_( (int)_renderViewport.begin.x, (int)_renderViewport.begin.y, (int)w, (int)h );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
	{
		m_fullscreen = _fullscreen;

		d3dpp = _fullscreen ? &d3dppFS : &d3dppW;
		d3dpp->BackBufferWidth = _resolution.getWidth();
		d3dpp->BackBufferHeight = _resolution.getHeight();

		m_screenResolution = _resolution;
		//nScreenBPP = _bpp;

		if( restore_() == false )
		{
			MENGE_LOG_ERROR( "Error: Graphics change mode failed\n" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setRenderTarget( RenderImageInterface* _renderTarget, bool _clear )
	{
		/*if( m_inRender )
		{
		HRESULT hr = m_pD3DDevice->EndScene();
		if( FAILED( hr ) )
		{
		log_error( "Error: DX8RenderSystem failed to EndScene (hr:%d)", hr );
		}
		}
		else
		{
		m_inRender = true;
		}*/
		HRESULT hr;
		LPDIRECT3DSURFACE8 pSurf=0, pDepth=0;
		DX8RenderTexture* _target = static_cast<DX8RenderTexture*>( _renderTarget );
		//begin_scene_( m_currentRenderTarget );

		if( _target != m_curRenderTexture )
		{
			if(_target)
			{
				_target->getInterface()->GetSurfaceLevel(0, &pSurf);
				pDepth = _target->getDepthInterface();
			}
			else
			{
				pSurf = pScreenSurf;
				pDepth = pScreenDepth;
			}
			hr = m_pD3DDevice->SetRenderTarget( pSurf, pDepth );
			if( FAILED( hr ) )
			{
				if( _target )
				{
					pSurf->Release();
				}
				MENGE_LOG_ERROR( "Gfx_BeginScene: Can't set render target" );
				//return false;
			}
			if( _target )
			{
				pSurf->Release();
			}

			m_curRenderTexture = _target;
		}


		if( _target == NULL ) return;
		if( _target->isDirty() && _clear )
		{
			clear( 0 );
			_target->setDirty( false );
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
		if( m_syncReady == true )
		{
			HRESULT hr = m_pD3DDevice->SetRenderTarget( m_syncTargets[m_frames % 2], 0 );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to SetRenderTarget (hr:%d)"
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
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to SetTransform (hr:%d)"
					, hr 
					);
			}

			hr = m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 3, 0, 1 );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to DrawIndexedPrimitive (hr:%d)"
					, hr 
					);
			}

			if( m_curRenderTexture )
			{
				LPDIRECT3DSURFACE8 surf;
				m_curRenderTexture->getInterface()->GetSurfaceLevel( 0, &surf );
				hr = m_pD3DDevice->SetRenderTarget( surf, m_curRenderTexture->getDepthInterface());
				if( FAILED( hr ) )
				{
					MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to SetRenderTarget (hr:%d)"
						, hr 
						);
				}
				surf->Release();
				//setProjectionMatrix_( m_curRenderTexture->getWidth(), m_curRenderTexture->getHeight() );
			}
			else
			{
				hr = m_pD3DDevice->SetRenderTarget( pScreenSurf, pScreenDepth );
				if( FAILED( hr ) )
				{
					MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to SetRenderTarget (hr:%d)"
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
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to SetTransform (hr:%d)"
					, hr 
					);
			}

			hr = loadSurfaceFromSurface_( m_syncTemp, NULL, m_syncTargets[(m_frames + 1) % 2], NULL );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to loadSurfaceFromSurface_ (hr:%d)"
					, hr 
					);
			}
			D3DLOCKED_RECT rect;
			hr = m_syncTemp->LockRect( &rect, NULL, D3DLOCK_READONLY );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to LockRect (hr:%d)"
					, hr 
					);
			}
			hr = m_syncTemp->UnlockRect();
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::syncCPU_ failed to UnlockRect (hr:%d)"
					, hr 
					);
			}
		}
		else	// just lock backbuffer
		{
			LPDIRECT3DSURFACE8 surf;
			HRESULT hr = m_pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &surf );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "D3D Error: failed to GetBackBuffer" );
				return;
			}
			D3DLOCKED_RECT lockRect;
			surf->LockRect( &lockRect, NULL, D3DLOCK_READONLY );
			surf->UnlockRect();
			surf->Release();
		}

	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::init_lost_()
	{
		// Store render target
		pScreenSurf = NULL;
		pScreenDepth = NULL;

		HRESULT hr = m_pD3DDevice->GetRenderTarget(&pScreenSurf);
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to GetRenderTarget (hr:%d)"
				, hr 
				);
		}
		hr = m_pD3DDevice->GetDepthStencilSurface(&pScreenDepth);
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to GetDepthStencilSurface (hr:%d)"
				, hr 
				);
		}

		for( TRenderTextureList::iterator it = m_renderTextureList.begin(), it_end = m_renderTextureList.end();
			it != it_end;
			it++ )
		{
			IDirect3DTexture8* d3dTexInterface = (*it)->getInterface();
			IDirect3DSurface8* depthInterface = (*it)->getDepthInterface();
			if( d3dTexInterface != NULL )
			{
				hr = d3dCreateTexture_( (*it)->getWidth(), (*it)->getHeight(), 1, D3DUSAGE_RENDERTARGET,
					d3dpp->BackBufferFormat, D3DPOOL_DEFAULT, &d3dTexInterface );
				(*it)->setTexInterface( d3dTexInterface );
				if( FAILED( hr ) )
				{
					MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to d3dCreateTexture_ (hr:%d)"
						, hr 
						);
				}
			}
			if( depthInterface != NULL )
			{
				hr = m_pD3DDevice->CreateDepthStencilSurface( (*it)->getWidth(), (*it)->getHeight(),
					D3DFMT_D16, D3DMULTISAMPLE_NONE, &depthInterface );
				(*it)->setDepthInterface( depthInterface );
				if( FAILED( hr ) )
				{
					MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to CreateDepthStencilSurface (hr:%d)"
						, hr 
						);
				}
			}
		}


		createSyncTargets_();

		for( TMapVBInfo::iterator it = m_vertexBuffers.begin(), it_end = m_vertexBuffers.end();
			it != it_end;
			++it )
		{
			VBInfo& vbInfo = it->second;
			hr = m_pD3DDevice->CreateVertexBuffer( vbInfo.length, vbInfo.usage, 
				vbInfo.fvf, vbInfo.pool, &vbInfo.pVB );
			if( FAILED( hr ) )
			{
				return false;
			}
		}

		for( TMapIBInfo::iterator it = m_indexBuffers.begin(), it_end = m_indexBuffers.end();
			it != it_end;
			++it )
		{
			IBInfo& ibInfo = it->second;
			hr = m_pD3DDevice->CreateIndexBuffer( ibInfo.length, ibInfo.usage, ibInfo.format,
				ibInfo.pool, &ibInfo.pIB );
			if( FAILED( hr ) )
			{
				return false;
			}
		}

		// Set common render states
		//m_pD3DDevice->SetVertexShader( D3DFVF_MENGE_VERTEX );

		hr = m_pD3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to SetTextureStageState (hr:%d)"
				, hr 
				);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	HRESULT DX8RenderSystem::d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,  DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8 * ppTexture )
	{
		//HRESULT hr = D3DXCreateTexture( m_pD3DDevice, Width, Height, MipLevels, Usage, Format, Pool, ppTexture );
		HRESULT hr = m_pD3DDevice->CreateTexture( Width, Height, MipLevels, Usage, Format, Pool, ppTexture );
		return hr;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::clear( DWORD _color )
	{
		//pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		HRESULT	 hr;
		if( m_curRenderTexture )
		{
			if( m_curRenderTexture->getDepthInterface() )
			{
				hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
				if( FAILED( hr ) )
				{
					MENGE_LOG_ERROR( "Error: DX8RenderSystem::clear failed to Clear (hr:%d)"
						, hr 
						);
				}
			}
			else
			{
				hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, _color, 1.0f, 0 );
				if( FAILED( hr ) )
				{
					MENGE_LOG_ERROR( "Error: DX8RenderSystem::clear failed to Clear (hr:%d)"
						, hr 
						);
				}
			}
		}
		else
		{
			//if( bZBuffer )
			{
				hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
				if( FAILED( hr ) )
				{
					MENGE_LOG_ERROR( "Error: DX8RenderSystem::clear failed to Clear (hr:%d)"
						, hr 
						);
				}
			}
			/*else
			{
			m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
			}*/
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureMatrix( size_t _stage, const float* _texture )
	{
		HRESULT hr;
		if( _texture != NULL )
		{
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::setTextureMatrix failed to SetTextureStageState (hr:%d)"
					, hr 
					);
			}
			D3DTRANSFORMSTATETYPE level = static_cast<D3DTRANSFORMSTATETYPE>( static_cast<DWORD>( D3DTS_TEXTURE0 ) + _stage );
			hr = m_pD3DDevice->SetTransform( level, (const D3DMATRIX*)_texture );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::setTextureMatrix failed to SetTransform (hr:%d)"
					, hr 
					);
			}
		}
		else
		{
			hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::setTextureMatrix failed to SetTextureStageState (hr:%d)"
					, hr 
					);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	HRESULT DX8RenderSystem::loadSurfaceFromSurface_( LPDIRECT3DSURFACE8 pDestSurface, CONST RECT * pDestRect,  LPDIRECT3DSURFACE8 pSrcSurface, CONST RECT * pSrcRect )
	{
		D3DLOCKED_RECT dstLockedRect;
		D3DLOCKED_RECT srcLockedRect;
		RECT dstRect;
		RECT srcRect;
		UINT srcWidth = 0, srcHeight = 0, dstWidth = 0, dstHeight = 0;

		D3DSURFACE_DESC dest_desc;
		HRESULT hr = pDestSurface->GetDesc( &dest_desc );
		if( FAILED( hr ) )
		{
			return hr;
		}

		D3DSURFACE_DESC src_desc;
		hr = pSrcSurface->GetDesc( &src_desc );
		if( FAILED( hr ) )
		{
			return hr;
		}

		if( pDestRect != NULL )
		{
			dstWidth = pDestRect->right - pDestRect->left;
			dstHeight = pDestRect->bottom - pDestRect->top;
			dstRect = *pDestRect;
		}
		else
		{
			dstWidth = dest_desc.Width;
			dstHeight = dest_desc.Height;
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
			srcWidth = src_desc.Width;
			srcHeight = src_desc.Height;
			srcRect.left = 0;
			srcRect.top = 0;
			srcRect.right = srcWidth;
			srcRect.bottom = srcHeight;
		}

		//POINT pt = { 0, 0 };
		//HRESULT hr = m_pD3DDevice->CopyRects( pSrcSurface, &srcRect, 1, pDestSurface, &pt );

		hr = pDestSurface->LockRect( &dstLockedRect, pDestRect, D3DLOCK_DISCARD );
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

			if( src_desc.Format == D3DFMT_R5G6B5 )
			{
				for( std::size_t i = 0; i < srcHeight; ++i )
				{
					for( std::size_t j = 0; j < srcWidth; ++j )
					{
						uint16 color = *reinterpret_cast<uint16*>( srcdata + j*2 );
						uint32* dstColor = reinterpret_cast<uint32*>( dstdata + j*4 );
						*dstColor = 0xFF000000 | ((color & 0xF800) << 8) |
							((color & 0x07E0) << 5) | ((color & 0x001F) << 3);
					}
					srcdata += srcLockedRect.Pitch;
					dstdata += dstLockedRect.Pitch;
				}
			}
			else
			{
				for( std::size_t i = 0; i < srcHeight; ++i )
				{
					std::copy( srcdata, srcdata + srcWidth * 4, dstdata );
					srcdata += srcLockedRect.Pitch;
					dstdata += dstLockedRect.Pitch;
				}
			}
		}
		else
		{
			// resampler
			// only optimized for 2D
			// srcdata stays at beginning of slice, pdst is a moving pointer
			unsigned char* srcdata = (unsigned char*)srcLockedRect.pBits;
			unsigned char* pdst = (unsigned char*)dstLockedRect.pBits;
			unsigned int channels = 4;
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
			for (size_t y = (size_t)dstRect.top; y < (size_t)dstRect.bottom; y++, sy_48+=stepy )
			{
				temp = sy_48 >> 36;
				temp = (temp > 0x800)? temp - 0x800: 0;
				unsigned int syf = temp & 0xFFF;
				size_t sy1 = temp >> 12;
				size_t sy2 = (std::min<size_t>)(sy1+1, srcRect.bottom-srcRect.top-1);
				size_t syoff1 = sy1 * srcRowPitch;
				size_t syoff2 = sy2 * srcRowPitch;

				uint64 sx_48 = (stepx >> 1) - 1;
				for (size_t x = (size_t)dstRect.left; x < (size_t)dstRect.right; x++, sx_48+=stepx)
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
			MENGE_LOG_ERROR( "Error: D3D8 failed to SetViewport" );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::restore_()
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
		if(pScreenSurf) pScreenSurf->Release();
		if(pScreenDepth) pScreenDepth->Release();

		for( TRenderTextureList::iterator it = m_renderTextureList.begin(), it_end = m_renderTextureList.end();
			it != it_end;
			it++ )
		{
			(*it)->getInterface()->Release();
			(*it)->getDepthInterface()->Release();
		}

		hr = m_pD3DDevice->SetIndices(NULL,0);
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::restore failed to SetIndices (hr:%d)"
				, hr 
				);
		}
		hr = m_pD3DDevice->SetStreamSource( 0, NULL, 0 );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::restore failed to SetStreamSource (hr:%d)"
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

		if( init_lost_() == false )
		{
			return false;
		}

		//if(procGfxRestoreFunc) return procGfxRestoreFunc();
		onRestoreDevice();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::createSyncTargets_()
	{
		// sync surfaces
		D3DFORMAT fmt = D3DFMT_X8R8G8B8;
		UINT w = 2;
		UINT d = 1;
		m_syncReady = false;
		//D3DXCheckTextureRequirements( pD3DDevice, &w, &w, &d, D3DUSAGE_RENDERTARGET, &fmt, D3DPOOL_DEFAULT );
		HRESULT hr = m_pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, TRUE, &(m_syncTargets[0]) );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to CreateRenderTarget (hr:%d)"
				, hr 
				);
			return;
		}
		hr = m_pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, TRUE, &(m_syncTargets[1]) );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to CreateRenderTarget (hr:%d)"
				, hr 
				);
			return;
		}
		hr = d3dCreateTexture_( w, w, d, 0, fmt, D3DPOOL_SYSTEMMEM, &m_syncTempTex );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to d3dCreateTexture_ (hr:%d)"
				, hr 
				);
			return;
		}
		hr = m_syncTempTex->GetSurfaceLevel( 0, &m_syncTemp );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::init_lost_ failed to GetSurfaceLevel (hr:%d)"
				, hr 
				);
			return;
		}
		m_syncReady = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::getResolutions( TVectorResolutions & _resolutions )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::release_()
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
			hr = m_pD3DDevice->SetStreamSource( 0, NULL, 0 );
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::restore failed to SetStreamSource (hr:%d)"
					, hr 
					);
			}
			hr = m_pD3DDevice->SetIndices(NULL,0);
			if( FAILED( hr ) )
			{
				MENGE_LOG_ERROR( "Error: DX8RenderSystem::restore failed to SetIndices (hr:%d)"
					, hr 
					);
			}
		}

		for( TMapVBInfo::iterator 
			it = m_vertexBuffers.begin()
			, it_end = m_vertexBuffers.end();
		it != it_end;
		++it )
		{
			it->second.pVB->Release();
		}

		for( TMapIBInfo::iterator 
			it = m_indexBuffers.begin()
			, it_end = m_indexBuffers.end();
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

		//for( size_t i = 0; i < D3DDP_MAXTEXCOORD; ++i )
		//{
		//	m_addressU[i] = D3DTADDRESS_WRAP;
		//	m_addressV[i] = D3DTADDRESS_WRAP;
		//	m_textureColorArg1[i] = D3DTA_TEXTURE;
		//	m_textureColorArg2[i] = D3DTA_CURRENT;
		//	m_textureAlphaArg1[i] = D3DTA_TEXTURE;
		//	m_textureAlphaArg2[i] = D3DTA_CURRENT;
		//}
		//m_textureColorOp[0] = D3DTOP_MODULATE;
		//m_textureAlphaOp[0] = D3DTOP_SELECTARG1;
		//for( size_t i = 1; i < D3DDP_MAXTEXCOORD; ++i )
		//{
		//	m_textureColorOp[i] = D3DTOP_DISABLE;
		//	m_textureAlphaOp[i] = D3DTOP_DISABLE;
		//}

		if( m_listener != NULL )
		{
			m_listener->onDeviceRestored();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	VBHandle DX8RenderSystem::createVertexBuffer( std::size_t _verticesNum, std::size_t _vertexSize )
	{
		IDirect3DVertexBuffer8* vb = NULL;
		HRESULT hr = m_pD3DDevice->CreateVertexBuffer( _verticesNum * _vertexSize, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			0, D3DPOOL_DEFAULT, &vb );

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
	void DX8RenderSystem::releaseVertexBuffer( VBHandle _vbHandle )
	{
		TMapVBInfo::iterator it_find = m_vertexBuffers.find( _vbHandle );
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
		TMapIBInfo::iterator it_find = m_indexBuffers.find( _ibHandle );
		if( it_find != m_indexBuffers.end() )
		{
			it_find->second.pIB->Release();
			m_indexBuffers.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void* DX8RenderSystem::lockVertexBuffer( VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags )
	{
		IDirect3DVertexBuffer8* vb = m_vertexBuffers[_vbHandle].pVB;
		void* lock = NULL;
		HRESULT hr = vb->Lock( _offset, _size, (BYTE**)&lock, _flags );
		if( FAILED( hr ) )
		{
			lock = NULL;
		}
		return lock;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::unlockVertexBuffer( VBHandle _vbHandle )
	{
		IDirect3DVertexBuffer8* vb = m_vertexBuffers[_vbHandle].pVB;
		HRESULT hr = vb->Unlock();
		return SUCCEEDED( hr );
	}
	//////////////////////////////////////////////////////////////////////////
	uint16* DX8RenderSystem::lockIndexBuffer( IBHandle _ibHandle )
	{
		IDirect3DIndexBuffer8* ib = m_indexBuffers[_ibHandle].pIB;
		uint16* lock = NULL;
		HRESULT hr = ib->Lock( 0, 0, (BYTE**)&lock, 0 );
		return lock;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderSystem::unlockIndexBuffer( IBHandle _ibHandle )
	{
		IDirect3DIndexBuffer8* ib = m_indexBuffers[_ibHandle].pIB;
		HRESULT hr = ib->Unlock();
		return SUCCEEDED( hr );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setVertexBuffer( VBHandle _vbHandle )
	{
		if( _vbHandle == 0 )
		{
			return;
		}
		VBInfo& vbInfo = m_vertexBuffers[_vbHandle];
		HRESULT hr = m_pD3DDevice->SetStreamSource( 0, vbInfo.pVB, vbInfo.vertexSize );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem::setVertexBuffer failed to SetStreamSource (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setIndexBuffer( IBHandle _ibHandle )
	{
		if( _ibHandle == 0 )
		{
			return;
		}
		IDirect3DIndexBuffer8* ib = m_indexBuffers[_ibHandle].pIB;
		HRESULT hr = m_pD3DDevice->SetIndices( ib, 0 );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setIndexBuffer (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::drawIndexedPrimitive( EPrimitiveType _type, std::size_t _baseVertexIndex,
		std::size_t _minIndex, std::size_t _verticesNum, std::size_t _startIndex, std::size_t _indexCount )
	{
		D3DPRIMITIVETYPE primitiveType = s_toD3DPrimitiveType( _type );

		UINT primCount = s_getPrimitiveCount( _type, _indexCount );

		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive( primitiveType,
			_minIndex, _verticesNum, _startIndex, primCount );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to DrawIndexedPrimitive (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTexture( std::size_t _stage, RenderImageInterface* _texture )
	{
		HRESULT hr;
		IDirect3DTexture8* d3d8Texture = NULL;
		if( _texture != NULL )
		{
			DX8Texture* t = static_cast<DX8Texture*>( _texture );
			d3d8Texture = t->getInterface();
		}
		hr = m_pD3DDevice->SetTexture( _stage, d3d8Texture );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to SetTexture (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setSrcBlendFactor( EBlendFactor _src )
	{
		DWORD factor = s_toD3DBlend( _src );

		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, factor );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setBlendFactor (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setDstBlendFactor( EBlendFactor _dst )
	{
		DWORD factor = s_toD3DBlend( _dst );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, factor );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setBlendFactor (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureAddressing( std::size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
		HRESULT hr;
		D3DTEXTUREADDRESS adrU = s_toD3DTextureAddress( _modeU );
		D3DTEXTUREADDRESS adrV = s_toD3DTextureAddress( _modeV );

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ADDRESSU, adrU );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureAddressing (hr:%d)"
				, hr 
				);
		}

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ADDRESSV, adrV );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureAddressing (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureFactor( uint32 _color )
	{
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, _color );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureFactor (hr:%d)"
				, hr 
				);
		}
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
		D3DCULL mode = s_toD3DCullMode( _mode );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, mode );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setCullMode (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setDepthBufferTestEnable( bool _depthTest )
	{
		D3DZBUFFERTYPE test = _depthTest ? D3DZB_TRUE : D3DZB_FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, test );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setDepthBufferTestEnable (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
	{
		DWORD dWrite = _depthWrite ? TRUE : FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, dWrite );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setDepthBufferWriteEnable (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		D3DCMPFUNC func = s_toD3DCmpFunc( _depthFunction );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ZFUNC, func );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setDepthBufferCmpFunc (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setFillMode( EFillMode _mode )
	{
		D3DFILLMODE mode = s_toD3DFillMode( _mode );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_FILLMODE, mode );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setFillMode (hr:%d)"
				, hr 
				);
		}
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
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE, value );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setColorBufferWriteEnable (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setShadeType( EShadeType _sType )
	{
		D3DSHADEMODE mode = s_toD3DShadeMode( _sType );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_SHADEMODE, mode );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setShadeType (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setAlphaTestEnable( bool _alphaTest )
	{
		DWORD alphaTest = _alphaTest ? TRUE : FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, alphaTest );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setAlphaTestEnable (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
	{
		DWORD alphaBlend = _alphaBlend ? TRUE : FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, alphaBlend );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setAlphaBlendEnable (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha )
	{
		D3DCMPFUNC func = s_toD3DCmpFunc( _alphaFunc );
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, func );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setAlphaCmpFunc (hr:%d)"
				, hr 
				);
		}

		DWORD alpha = _alpha;
		hr = m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF, alpha );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setAlphaCmpFunc (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setLightingEnable( bool _light )
	{
		DWORD value = _light ? TRUE : FALSE;
		HRESULT hr = m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, value );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setLightingEnable (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp
		,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		HRESULT hr;
		D3DTEXTUREOP colorOp = s_toD3DTextureOp( _textrueOp );
		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		DWORD arg2 = s_toD3DTextureArg( _arg2 );

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLOROP, colorOp );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureStageColorOp (hr:%d)"
				, hr 
				);
		}

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLORARG1, arg1 );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureStageColorOp (hr:%d)"
				, hr 
				);
		}

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_COLORARG2, arg2 );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureStageColorOp (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp
		,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		HRESULT hr;
		D3DTEXTUREOP alphaOp = s_toD3DTextureOp( _textrueOp );
		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		DWORD arg2 = s_toD3DTextureArg( _arg2 );

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAOP, alphaOp );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureStageAlphaOp (hr:%d)"
				, hr
				);
		}

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAARG1, arg1 );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureStageAlphaOp (hr:%d)"
				, hr 
				);
		}

		hr = m_pD3DDevice->SetTextureStageState( _stage, D3DTSS_ALPHAARG2, arg2 );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureStageAlphaOp (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter )
	{
		D3DTEXTURESTAGESTATETYPE textureFilterType = s_toD3DTextureFilterType( _filterType );
		D3DTEXTUREFILTERTYPE textureFilter = s_toD3DTextureFilter( _filter );

		HRESULT hr = m_pD3DDevice->SetTextureStageState( _stage, textureFilterType, textureFilter );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setTextureStageFilter (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setVertexDeclaration( uint32 _declaration )
	{
		HRESULT hr = m_pD3DDevice->SetVertexShader( _declaration );
		if( FAILED( hr ) )
		{
			MENGE_LOG_ERROR( "Error: DX8RenderSystem failed to setVertexDeclaration (hr:%d)"
				, hr 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::setVSync( bool _vSync )
	{
		d3dppW.SwapEffect = _vSync ? D3DSWAPEFFECT_COPY_VSYNC : D3DSWAPEFFECT_COPY;
		d3dppFS.FullScreen_PresentationInterval = _vSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

		restore_();
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderSystem::makeProjection2D( float _left, float _right,
		float _top, float _bottom, 
		float _near, float _far,  
		float* _outMatrix )
	{
		float inv_lr = 1.0f / ( _left - _right );
		float inv_bt = 1.0f / ( _top - _bottom );
		float inv_znzf = 1.0f / ( _near - _far );
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
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
