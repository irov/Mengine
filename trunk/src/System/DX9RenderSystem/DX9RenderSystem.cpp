#	include "DX9RenderSystem.h"
#	include "DX9Texture.h"
#	include "DX9RenderTexture.h"
#   include "DX9RenderShader.h"

#	include "DX9ErrorHelper.h"

#	include <algorithm>
#	include <cmath>
#	include <stdio.h>

#	include "Logger/Logger.h"


SERVICE_FACTORY( RenderSystem, Menge::RenderSystemInterface, Menge::DX9RenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{    
	typedef IDirect3D9* (WINAPI *PDIRECT3DCREATE9)(UINT);

	static const D3DFORMAT D32SFormats[] = { D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D15S1, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16, (D3DFORMAT) 0 };
	static const D3DFORMAT D32Formats[]	= { D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D16, D3DFMT_D15S1, (D3DFORMAT) 0 };
	static const D3DFORMAT D16SFormats[] = { D3DFMT_D15S1, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D16, D3DFMT_D32, D3DFMT_D24X8, (D3DFORMAT) 0 };
	static const D3DFORMAT D16Formats[]	= { D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D24X4S4, (D3DFORMAT) 0 };
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
		case TF_FLATCUBIC:
			return D3DTEXF_PYRAMIDALQUAD;
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
	DX9RenderSystem::DX9RenderSystem()
		: m_serviceProvider(nullptr)
		, m_pD3D(nullptr)
		, m_pD3DDevice(nullptr)
		, m_curRenderTexture(nullptr)
		//, m_syncTemp(nullptr)
		//, m_syncTempTex(nullptr)
		, m_screenSurf(nullptr)
		, m_screenDepth(nullptr)
		, m_vbHandleCounter(0)
		, m_ibHandleCounter(0)
		, m_currentIB(0)
        , m_currentVB(0)
		, m_listener(nullptr)
		, m_supportNPOT(false)
        , m_supportL8(false)
        , m_supportA8(false)
		, m_supportR8G8B8(false)
		, m_syncReady(false)
        , m_hd3d9(NULL)
        , m_adapterToUse(D3DADAPTER_DEFAULT)
        , m_deviceType(D3DDEVTYPE_HAL)
        , m_waitForVSync(false)
        //, m_currentTexture(nullptr)
	{
		//m_syncTargets[0] = NULL;
		//m_syncTargets[1] = NULL;
        for( size_t i = 0; i != 8; ++i )
        {
            m_currentTexture[i] = nullptr;
        }
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderSystem::~DX9RenderSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * DX9RenderSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setRenderListener( RenderSystemListener * _listener )
    {
        m_listener = _listener;
    }
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::initialize()
	{
		m_frames = 0;

		m_hd3d9 = ::LoadLibrary( L"d3d9.dll" );

		if( m_hd3d9 == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "Failed to load d3d9.dll"
                );

			return false;
		}

		PDIRECT3DCREATE9 pDirect3DCreate9 = (PDIRECT3DCREATE9)::GetProcAddress( m_hd3d9, "Direct3DCreate9" );
		if( pDirect3DCreate9 == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)( "Failed to get Direct3DCreate9 proc address"
                );

			return false;
		}

		// Init D3D
		LOGGER_INFO(m_serviceProvider)( "Initializing DX9RenderSystem..." 
            );

		m_pD3D = pDirect3DCreate9( D3D_SDK_VERSION ); // D3D_SDK_VERSION

		if( m_pD3D == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "Can't create D3D interface" 
                );

			return false;
		}

        m_adapterToUse = D3DADAPTER_DEFAULT;
        m_deviceType = D3DDEVTYPE_HAL;

        UINT AdapterCount = m_pD3D->GetAdapterCount();
        for( UINT Adapter = 0; Adapter != AdapterCount; ++Adapter )
        {
            D3DADAPTER_IDENTIFIER9 Identifier;
            if( m_pD3D->GetAdapterIdentifier( Adapter, 0, &Identifier ) != D3D_OK )
            {
                continue;
            }

            if( strstr( Identifier.Description, "PerfHUD" ) != 0 )
            {
                m_adapterToUse = Adapter;
                m_deviceType = D3DDEVTYPE_REF;
                break;
            }
        }

		// Get adapter info
        D3DADAPTER_IDENTIFIER9 AdID;

		IF_DXCALL(m_serviceProvider, m_pD3D, GetAdapterIdentifier, (m_adapterToUse, 0, &AdID ) )
		{
			LOGGER_ERROR(m_serviceProvider)( "Can't determine adapter identifier" 
				);
			
			return false;
		}

        IF_DXCALL(m_serviceProvider, m_pD3D, GetAdapterIdentifier, ( m_adapterToUse, 0, &AdID ) )
        {
            LOGGER_ERROR(m_serviceProvider)( "Can't determine adapter identifier" 
                );

            return false;
        }

		LOGGER_INFO(m_serviceProvider)( "VendorId: %d", AdID.VendorId  );
		LOGGER_INFO(m_serviceProvider)( "DeviceId: %d", AdID.DeviceId );
		LOGGER_INFO(m_serviceProvider)( "D3D Driver: %s", AdID.Driver );
		LOGGER_INFO(m_serviceProvider)( "Description: %s", AdID.Description );

		LOGGER_INFO(m_serviceProvider)( "Version: %d.%d.%d.%d"
			, HIWORD(AdID.DriverVersion.HighPart)
			, LOWORD(AdID.DriverVersion.HighPart)
			, HIWORD(AdID.DriverVersion.LowPart)
			, LOWORD(AdID.DriverVersion.LowPart)
			);

		// Set up Windowed presentation parameters
        D3DDISPLAYMODE Mode;
        IF_DXCALL( m_serviceProvider, m_pD3D, GetAdapterDisplayMode, ( m_adapterToUse, &Mode ) )
        {
			LOGGER_ERROR(m_serviceProvider)( "Can't determine desktop video mode"
                );

			return false;
		}

		if( Mode.Format == D3DFMT_UNKNOWN )
		{
			LOGGER_ERROR(m_serviceProvider)( "Can't determine desktop video mode D3DFMT_UNKNOWN"
				);

			return false;
		}

		UINT screenWidth = Mode.Width;
		UINT screenHeight = Mode.Height;

		ZeroMemory( &m_d3dppW, sizeof(m_d3dppW) );
		m_d3dppW.BackBufferFormat = Mode.Format;

		// Set up Full Screen presentation parameters
		UINT nModes = m_pD3D->GetAdapterModeCount( m_adapterToUse, D3DFMT_X8R8G8B8 );

		//for(i=0; i<nModes; i++)
		//{
		//	m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &Mode);
		//	m_displayModes.push_back( Mode );
		//	m_resList.push_back( Mode.Width );
		//	m_resList.push_back( Mode.Height );
		//}

        D3DFORMAT Format = D3DFMT_UNKNOWN;

		for( UINT i = 0; i < nModes; ++i )
		{
			D3DDISPLAYMODE Mode;
			IF_DXCALL( m_serviceProvider, m_pD3D, EnumAdapterModes, ( m_adapterToUse, D3DFMT_X8R8G8B8, i, &Mode ) )
			{
				continue;
			}

			if(Mode.Width != screenWidth || Mode.Height != screenHeight) 
			{
				continue;
			}

			//if(nScreenBPP==16 && (_format_id(Mode.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
			if( s_format_id_(Mode.Format) > s_format_id_(Format) )
			{
				Format = Mode.Format;
			}
		}
		
		if( Format == D3DFMT_UNKNOWN )
		{
			LOGGER_ERROR(m_serviceProvider)( "Can't find appropriate full screen video mode" 
				);

			return false;
		}

		ZeroMemory( &m_d3dppFS, sizeof(m_d3dppFS) );
		m_d3dppFS.BackBufferFormat = Format;

		s_matIdent_( &m_matTexture );

		IF_DXCALL(m_serviceProvider, m_pD3D, GetDeviceCaps, ( m_adapterToUse, m_deviceType, &m_caps ) )
		{
			return false;
		}

		if( (m_caps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0 )			
		{
			m_supportNPOT = true;
		}
		else
		{
			m_supportNPOT = false;			
		}

        if( m_supportNPOT == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Render don't support D3DPTEXTURECAPS_POW2" );
        }
		
        m_supportL8 = this->supportTextureFormat( PF_L8 );

        if( m_supportL8 == false )
        {
            LOGGER_WARNING(m_serviceProvider)( "Render don't support PF_L8" 
                );
        }

        m_supportA8 = this->supportTextureFormat( PF_A8 );

        if( m_supportA8 == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Render don't support PF_A8" 
                );

            return false;
        }

		m_supportR8G8B8 = this->supportTextureFormat( PF_R8G8B8 );

        if( m_supportR8G8B8 == false )
        {
            LOGGER_WARNING(m_serviceProvider)( "Render don't support PF_R8G8B8" 
                );
        }

        bool supportDXT1 = this->supportTextureFormat( PF_DXT1 );

        if( supportDXT1 == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Render don't support PF_DXT1" 
                );

            return false;
        }
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::finalize()
    {
        this->release_();

        if( m_hd3d9 != NULL )
        {
            FreeLibrary( m_hd3d9 );
            m_hd3d9 = NULL;
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::refreshRenderStates_()
	{
#	define REFRESH_RENDER_STATE(STATE)\
	DXCALL( m_serviceProvider, m_pD3DDevice, GetRenderState, (STATE, &m_renderStates[STATE]) );

		REFRESH_RENDER_STATE(D3DRS_SRCBLEND);
		REFRESH_RENDER_STATE(D3DRS_DESTBLEND);
		REFRESH_RENDER_STATE(D3DRS_CULLMODE);
		REFRESH_RENDER_STATE(D3DRS_ZENABLE);
		REFRESH_RENDER_STATE(D3DRS_ZWRITEENABLE);
		REFRESH_RENDER_STATE(D3DRS_ZFUNC);
		REFRESH_RENDER_STATE(D3DRS_FILLMODE);
		REFRESH_RENDER_STATE(D3DRS_COLORWRITEENABLE);
		REFRESH_RENDER_STATE(D3DRS_SHADEMODE);
		REFRESH_RENDER_STATE(D3DRS_ALPHATESTENABLE);
		REFRESH_RENDER_STATE(D3DRS_ALPHABLENDENABLE);
		REFRESH_RENDER_STATE(D3DRS_ALPHAFUNC);
		REFRESH_RENDER_STATE(D3DRS_ALPHAREF);
		REFRESH_RENDER_STATE(D3DRS_LIGHTING);
		REFRESH_RENDER_STATE(D3DRS_ZENABLE);

#	undef REFRESH_RENDER_STATE

#	define REFRESH_TEXTURE_STAGE_STATE(STATE)\
	DXCALL( m_serviceProvider, m_pD3DDevice, GetTextureStageState, (stage, STATE, &m_textureStageStates[stage][STATE]) );

		for( DWORD stage = 0; stage != MENGE_MAX_TEXTURE_STAGES; ++stage )
		{
			REFRESH_TEXTURE_STAGE_STATE(D3DTSS_COLOROP);
			REFRESH_TEXTURE_STAGE_STATE(D3DTSS_COLORARG1);
			REFRESH_TEXTURE_STAGE_STATE(D3DTSS_COLORARG2);
			REFRESH_TEXTURE_STAGE_STATE(D3DTSS_ALPHAOP);
			REFRESH_TEXTURE_STAGE_STATE(D3DTSS_ALPHAARG1);
			REFRESH_TEXTURE_STAGE_STATE(D3DTSS_ALPHAARG2);
			REFRESH_TEXTURE_STAGE_STATE(D3DTSS_TEXCOORDINDEX);	
			REFRESH_TEXTURE_STAGE_STATE(D3DTSS_TEXTURETRANSFORMFLAGS);
		}

#	undef REFRESH_TEXTURE_STAGE_STATE


#	define REFRESH_SAMPLER_STATE(STATE)\
	DXCALL( m_serviceProvider, m_pD3DDevice, GetSamplerState, (stage, STATE, &m_samplerStates[stage][STATE]) );
		
		for( DWORD stage = 0; stage != MENGE_MAX_TEXTURE_STAGES; ++stage )
		{
			REFRESH_SAMPLER_STATE(D3DSAMP_ADDRESSU);
			REFRESH_SAMPLER_STATE(D3DSAMP_ADDRESSV);
			REFRESH_SAMPLER_STATE(D3DSAMP_MAGFILTER);
			REFRESH_SAMPLER_STATE(D3DSAMP_MINFILTER);
			REFRESH_SAMPLER_STATE(D3DSAMP_MIPFILTER);
		}

#	undef REFRESH_SAMPLER_STATE			
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::createRenderWindow( const Resolution & _resolution, int _bits, 
		bool _fullscreen, WindowHandle _winHandle, bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
        (void)_bits;
        (void)_FSAAType;
        (void)_FSAAQuality;

		static const char *szFormats[]={ 
			"UNKNOWN"
			,"R5G6B5"
			,"X1R5G5B5"
			,"A1R5G5B5"
			,"X8R8G8B8"
			,"A8R8G8B8"
		};
		
		m_windowResolution = _resolution;		

		m_fullscreen = _fullscreen;
        m_waitForVSync = _waitForVSync;

		m_d3dppW.MultiSampleType = D3DMULTISAMPLE_NONE;
		m_d3dppW.Windowed = TRUE;
		//m_d3dppW.Flags			= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		m_d3dppW.BackBufferWidth = m_windowResolution.getWidth();
		m_d3dppW.BackBufferHeight = m_windowResolution.getHeight();
		m_d3dppW.BackBufferCount = 1;

		m_d3dppW.hDeviceWindow = (HWND)_winHandle;

		m_d3dppW.SwapEffect = D3DSWAPEFFECT_DISCARD;
		
		// Set up Windowed presentation parameters
        D3DDISPLAYMODE Mode;
		IF_DXCALL( m_serviceProvider, m_pD3D, GetAdapterDisplayMode, (D3DADAPTER_DEFAULT, &Mode) ) 
		{
			LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::createRenderWindow Can't determine desktop video mode" 
                );

			return false;
		}

		m_d3dppW.BackBufferFormat = Mode.Format;

		m_d3dppW.EnableAutoDepthStencil = FALSE;
		m_d3dppW.AutoDepthStencilFormat = D3DFMT_UNKNOWN;

		m_d3dppW.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		
		m_d3dppFS.MultiSampleType = D3DMULTISAMPLE_NONE;
        m_d3dppFS.Windowed = FALSE;

		m_d3dppFS.BackBufferWidth = m_windowResolution.getWidth();
		m_d3dppFS.BackBufferHeight = m_windowResolution.getHeight();
		m_d3dppFS.BackBufferCount = 1;

		m_d3dppFS.hDeviceWindow = (HWND)_winHandle;

		m_d3dppFS.SwapEffect = D3DSWAPEFFECT_DISCARD;
               
        m_d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        		
		m_d3dppFS.EnableAutoDepthStencil = FALSE;
		m_d3dppFS.AutoDepthStencilFormat = D3DFMT_UNKNOWN;

        this->updateVSyncDPP_();
		
		if( _fullscreen == true )
		{
			m_d3dpp = &m_d3dppFS;
		}
		else
		{
			m_d3dpp = &m_d3dppW;
		}

		if( s_format_id_(m_d3dpp->BackBufferFormat) < 4 ) 
		{
			m_screenBits = 16;
		}
		else 
		{
			m_screenBits = 32;
		}

		//_fullscreen ? MENGE_LOG_INFO( "fullscreen mode" ) : MENGE_LOG_INFO( "windowed mode" );

		//MENGE_LOG_INFO( "attempting to create d3ddevice: %dx%dx%d\nBackbuffer format %d\nDepthstencil format %d",
		//	d3dpp->BackBufferWidth, d3dpp->BackBufferHeight, m_screenBits, d3dpp->BackBufferFormat, d3dpp->AutoDepthStencilFormat );

		// Create D3D Device
		HRESULT hr;

		hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)_winHandle,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
			m_d3dpp, &m_pD3DDevice );

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)_winHandle,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
				m_d3dpp, &m_pD3DDevice );
		}

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)_winHandle,
				D3DCREATE_MIXED_VERTEXPROCESSING |D3DCREATE_FPU_PRESERVE ,
				m_d3dpp, &m_pD3DDevice );

			if( FAILED( hr ) )
			{
				Sleep( 100 );
				hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)_winHandle,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
					m_d3dpp, &m_pD3DDevice );
			}
		}

		if( FAILED ( hr ) )
		{
			LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::createRenderWindow Can't create D3D device (hr:%u, hwnd:%u)"
				, hr
				, (HWND)_winHandle 
				);

			return false;
		}

		LOGGER_INFO(m_serviceProvider)( "Mode: %d x %d x %s\n"
			, m_windowResolution.getWidth()
			, m_windowResolution.getHeight()
			, szFormats[s_format_id_(m_d3dpp->BackBufferFormat)]
			);

		// Init all stuff that can be lost

		if( this->initLost_() == false ) 
		{
            LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::createRenderWindow invalid init lost"
                );

			return false;
		}

		LOGGER_WARNING(m_serviceProvider)( "DX9RenderSystem initalized successfully!" );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::screenshot( const RenderImageInterfacePtr & _image, const float * _rect )
	{	
		RECT rect;

		if( _rect )
		{
			rect.left = (std::max)( 0L, (LONG)_rect[0] );
			rect.top = (std::max)( 0L, (LONG)_rect[1] );
			rect.right = std::min<LONG>( m_windowResolution.getWidth(), (LONG)_rect[2] );
			rect.bottom = std::min<LONG>( m_windowResolution.getHeight(), (LONG)_rect[3] );
		}
		else
		{
			rect.left = 0;
			rect.top = 0;
			rect.right = m_windowResolution.getWidth();
			rect.bottom = m_windowResolution.getHeight();
		}
		
        DX9TexturePtr dxTexture = stdex::intrusive_static_cast<DX9TexturePtr>( _image );
		
		LPDIRECT3DSURFACE9 renderTarget;
		IF_DXCALL( m_serviceProvider, m_pD3DDevice, GetRenderTarget, ( 0, &renderTarget ) )
		{			
			return false;
		}

		if( renderTarget == nullptr )
		{
			return false;
		}

		D3DSURFACE_DESC rtDesc;
		renderTarget->GetDesc( &rtDesc );

		LPDIRECT3DSURFACE9 offscreenSurface;
		IF_DXCALL( m_serviceProvider, m_pD3DDevice, CreateOffscreenPlainSurface, (rtDesc.Width, rtDesc.Height, rtDesc.Format,  D3DPOOL_SYSTEMMEM, &offscreenSurface, NULL ) )
		{
			renderTarget->Release();
			offscreenSurface->Release();

			return false;
		}

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, GetRenderTargetData, ( renderTarget, offscreenSurface ) )
		{
			renderTarget->Release();
			offscreenSurface->Release();

			return false;
		}
				
		IDirect3DTexture9 * dtext = dxTexture->getDXTextureInterface();
				
		D3DSURFACE_DESC desc;
		IF_DXCALL( m_serviceProvider, dtext, GetLevelDesc, (0, &desc) )
		{
			renderTarget->Release();
			offscreenSurface->Release();

			return false;
		}

        LPDIRECT3DSURFACE9 dsurf;
		IF_DXCALL( m_serviceProvider, dtext, GetSurfaceLevel, (0, &dsurf ) )
		{
			renderTarget->Release();
			offscreenSurface->Release();

			return false;
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

		if( loadSurfaceFromSurface_( dsurf, &dest_rect, offscreenSurface, &rect ) == false )
		{
			renderTarget->Release();
			offscreenSurface->Release();

			return false;
		}

		renderTarget->Release();
		offscreenSurface->Release();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setProjectionMatrix device not created"
                );

            return;
        }

        m_projection = _projection;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetTransform, ( D3DTS_PROJECTION, (D3DMATRIX*)_projection.buff() ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setModelViewMatrix( const mt::mat4f & _modelview )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setModelViewMatrix device not created"
                );

            return;
        }

        m_modelview = _modelview;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetTransform, ( D3DTS_VIEW, (D3DMATRIX*)_modelview.buff() ) );
	}
    //////////////////////////////////////////////////////////////////////////
    PixelFormat DX9RenderSystem::findFormatFromChannels_( size_t & _channels, PixelFormat _format ) const
    {
        PixelFormat hwFormat = PF_UNKNOWN;

        switch( _format )
        {
        case PF_UNKNOWN:
            {
                if( _channels == 1 )
                {
                    if( m_supportA8 == true )
                    {
                        hwFormat = PF_A8;
                    }
                    else
                    {
                        hwFormat = PF_X8R8G8B8;
                        _channels = 4;
                    }
                }
                else if( _channels == 3 )
                {
                    if( m_supportR8G8B8 == true )
                    {
                        hwFormat = PF_R8G8B8;
                    }
                    else
                    {
                        hwFormat = PF_X8R8G8B8;
                        _channels = 4;
                    }
                }
                else if( _channels == 4 )
                {
                    hwFormat = PF_A8R8G8B8;
                }
            }break;
        default:
            {
                hwFormat = _format;
            }break;
        }

        return hwFormat;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr DX9RenderSystem::createImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
	{
        size_t hwChannels = _channels;

        PixelFormat hwFormat = this->findFormatFromChannels_( hwChannels, _format );

		IDirect3DTexture9 * dxTextureInterface = nullptr;
        
		if( this->d3dCreateTexture_( _width, _height, 1, 0,	hwFormat, D3DPOOL_MANAGED, &dxTextureInterface ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem.createImage: can't create texture %dx%d %d"
				, _width
				, _height
				, hwFormat
                );

			return nullptr;
		}

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( m_serviceProvider, dxTextureInterface, GetLevelDesc, ( 0, &texDesc ) )
		{
			return nullptr;
		}

		DX9Texture * dxTexture = m_factoryDX9Texture.createObjectT();
        dxTexture->initialize( m_serviceProvider, dxTextureInterface, texDesc.Width, texDesc.Height, hwChannels, hwFormat );

		LOGGER_INFO(m_serviceProvider)( "DX9RenderSystem.createImage: texture created %dx%d %d:%d"
			, texDesc.Width
			, texDesc.Height
			, texDesc.Format
            , hwChannels
			);

		return dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr DX9RenderSystem::createDynamicImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
	{
        size_t hwChannels = _channels;

        PixelFormat hwFormat = this->findFormatFromChannels_( hwChannels, _format );

		IDirect3DTexture9* dxTextureInterface = nullptr;

		if( d3dCreateTexture_( _width, _height, 1, 0, hwFormat, D3DPOOL_MANAGED, &dxTextureInterface ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem.createDynamicImage: can't create texture %dx%d %d"
				, _width
				, _height
				, hwFormat
				);

			return nullptr;
		}

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( m_serviceProvider, dxTextureInterface, GetLevelDesc, ( 0, &texDesc ) )
		{
			return nullptr;
		}

        DX9Texture * dxTexture = m_factoryDX9Texture.createObjectT();
        dxTexture->initialize( m_serviceProvider, dxTextureInterface, texDesc.Width, texDesc.Height, hwChannels, hwFormat );		
        
		LOGGER_INFO(m_serviceProvider)( "DX9RenderSystem.createDynamicImage: texture created %dx%d %d:%d"
			, texDesc.Width
			, texDesc.Height
			, texDesc.Format
            , hwChannels
			);

		return dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr DX9RenderSystem::createRenderTargetImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
	{
        size_t hwChannels = _channels;

        PixelFormat hwFormat = this->findFormatFromChannels_( hwChannels, _format );

		IDirect3DTexture9 * dxTextureInterface = nullptr;		
		if( this->d3dCreateTexture_( _width, _height, 1, D3DUSAGE_RENDERTARGET, hwFormat, D3DPOOL_DEFAULT, &dxTextureInterface ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem.createRenderTargetImage: can't  create render texture %dx%d %d"
				, _width
				, _height
				, hwFormat
				);

			return nullptr;
		}
		
        IDirect3DSurface9 * dxSurfaceInterface = nullptr;
		IF_DXCALL( m_serviceProvider, dxTextureInterface, GetSurfaceLevel, (0, &dxSurfaceInterface) )
		{
            LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem.createRenderTargetImage: can't get surface level %dx%d %d"
                , _width
                , _height
                , hwFormat
                );

			return nullptr;
		}

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( m_serviceProvider, dxTextureInterface, GetLevelDesc, ( 0, &texDesc ) )
		{
			return nullptr;
		}

		DX9RenderTexture* dxTexture = m_factoryDX9RenderTexture.createObjectT();

        dxTexture->initialize( m_serviceProvider, dxTextureInterface, texDesc.Width, texDesc.Height, hwChannels, hwFormat );
        dxTexture->setSurface( dxSurfaceInterface );

        LOGGER_INFO(m_serviceProvider)( "DX9RenderSystem.createRenderTargetImage: texture created %dx%d %d"
            , texDesc.Width
            , texDesc.Height
            , texDesc.Format
            );

		return dxTexture;
	}
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::resetDevice_()
    {
        if( m_fullscreen == false )
        {
			D3DDISPLAYMODE Mode;
            IF_DXCALL( m_serviceProvider, m_pD3D, GetAdapterDisplayMode, (D3DADAPTER_DEFAULT, &Mode) )
			{
				return false;
			}

            if( Mode.Format == D3DFMT_UNKNOWN ) 
            {
                LOGGER_ERROR(m_serviceProvider)( "Can't determine desktop video mode D3DFMT_UNKNOWN"
                    );

                return false;
            }

            m_d3dppW.BackBufferFormat = Mode.Format;

            if( s_format_id_(Mode.Format) < 4 ) 
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
            LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::resetDevice_: restore failed"
                );

            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::beginScene()
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::beginScene device not created"
                );

            return false;
        }

		HRESULT hr = m_pD3DDevice->TestCooperativeLevel();

        if( hr == D3DERR_DEVICENOTRESET )
        {
            LOGGER_WARNING(m_serviceProvider)( "DX9RenderSystem::beginScene: D3DERR_DEVICENOTRESET"
                );

            if( this->resetDevice_() == false )
            {
                ::Sleep(50);

                return false;
            }
        }
        else if( FAILED(hr) )
        {
            LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::beginScene: invalid TestCooperativeLevel %d"
                , hr
                );

            if( this->releaseResources_() == false )
            {
                LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::restore_ release resources"
                    );

                return false;
            }
  
            return false;
		}
		
        Viewport clear_viewport;

        clear_viewport.begin.x = 0.f;
        clear_viewport.begin.y = 0.f;
        clear_viewport.end.x = (float)m_windowResolution.getWidth();
        clear_viewport.end.y = (float)m_windowResolution.getHeight();
        
        if( m_viewport.equalViewport( clear_viewport ) == false )
        {
            this->setViewport( clear_viewport );

            this->clear_( 0 );

            this->setViewport( m_viewport );
        }
        
        IF_DXCALL( m_serviceProvider, m_pD3DDevice, BeginScene, () )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::endScene()
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::endScene device not created"
                );

            return;
        }

		// sync GPU with CPU
		//syncCPU_();
		
		DXCALL( m_serviceProvider, m_pD3DDevice, EndScene, () );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::swapBuffers()
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::swapBuffers device not created"
                );

            return;
        }

        HRESULT cooperativeLevel = m_pD3DDevice->TestCooperativeLevel();

        if( cooperativeLevel == D3DERR_DEVICELOST || 
            cooperativeLevel == D3DERR_DEVICENOTRESET )
        {
            return;
        }

		HRESULT hr = m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
        
        if( hr == D3DERR_DEVICELOST )
        {
            this->releaseResources_();
        }
        else if( FAILED( hr ) )
		{
			LOGGER_ERROR(m_serviceProvider)( "Error: D3D8 failed to swap buffers" );
		}

		++m_frames;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clearFrameBuffer( uint32 _frameBufferTypes, uint32 _color, float _depth, uint16 _stencil )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::clearFrameBuffer device not created"
                );

            return;
        }

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

		DXCALL( m_serviceProvider, m_pD3DDevice, Clear, ( 0, NULL, frameBufferFlags, _color, _depth, _stencil ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setViewport( const Viewport & _viewport )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setViewport device not created"
                );

            return;
        }
                
        m_viewport = _viewport;

		D3DVIEWPORT9 VP;

		VP.X = (int)::floorf( _viewport.begin.x + 0.5f );
		VP.Y = (int)::floorf( _viewport.begin.y + 0.5f );

		float width = _viewport.getWidth();
		float height = _viewport.getHeight();

		VP.Width = (int)::floorf( width + 0.5f );
		VP.Height = (int)::floorf( height + 0.5f );

		VP.MinZ = 0.f;
		VP.MaxZ = 1.f;

        size_t resolutionWidth = m_windowResolution.getWidth();
        size_t resolutionHeight = m_windowResolution.getHeight();

        if( VP.X >= resolutionWidth || 
            VP.Y >= resolutionHeight ||
            VP.X + VP.Width <= 0 || 
            VP.Y + VP.Height <= 0 )
        {
            LOGGER_INFO(m_serviceProvider)("DX9RenderSystem::setViewport set zero Viewport (%d, %d, %d, %d)"
                , VP.X
                , VP.Y
                , VP.X + VP.Width
                , VP.Y + VP.Height
                );

            D3DVIEWPORT9 VP_Zero;
            VP_Zero.X = 0;
            VP_Zero.Y = 0;
            VP_Zero.Width = 0;
            VP_Zero.Height = 0;
            VP_Zero.MinZ = 0.f;
            VP_Zero.MaxZ = 1.f;

            DXCALL( m_serviceProvider, m_pD3DDevice, SetViewport, (&VP_Zero) );

            return;
        }

        if( VP.X + VP.Width > resolutionWidth )
        {
            VP.Width = resolutionWidth - VP.X;
        }

        if( VP.Y + VP.Height > resolutionHeight )
        {
            VP.Height = resolutionHeight - VP.Y;
        }

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetViewport, (&VP) )
		{
			LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::setRenderViewport: failed viewport (%d, %d, %d, %d)"
				, VP.X
				, VP.Y
				, VP.X + VP.Width
				, VP.Y + VP.Height
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
	{
		m_fullscreen = _fullscreen;

        m_windowResolution = _resolution;

		m_d3dpp = _fullscreen ? &m_d3dppFS : &m_d3dppW;
		m_d3dpp->BackBufferWidth = m_windowResolution.getWidth();
		m_d3dpp->BackBufferHeight = m_windowResolution.getHeight();
		
		//nScreenBPP = _bpp;

		if( this->restore_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::changeWindowMode: Graphics change mode failed" 
                );
		}        
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::setRenderTarget( const RenderImageInterfacePtr & _renderTarget, bool _clear )
	{
  //      if( m_pD3DDevice == nullptr )
  //      {
  //          LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setRenderTarget device not created"
  //              );

  //          return false;
  //      }

		//HRESULT hr;
  //      LPDIRECT3DSURFACE9 pDepth = NULL;

		//DX8RenderTexturePtr renderTarget = stdex::intrusive_static_cast<DX8RenderTexturePtr>(_renderTarget);
		////begin_scene_( m_currentRenderTarget );

		//if( renderTarget != m_curRenderTexture )
		//{
		//	LPDIRECT3DSURFACE9 pSurf = NULL;

		//	if( renderTarget != nullptr )
		//	{
		//		IDirect3DTexture9 * dxTexture = renderTarget->getDXTextureInterface();
  //              DXCALL( m_serviceProvider, dxTexture, GetSurfaceLevel, ( 0, &pSurf ) )
		//		{
		//			return false;
		//		}

		//		pDepth = renderTarget->getDepthInterface();
		//	}
		//	else
		//	{
		//		pSurf = m_screenSurf;
		//		pDepth = m_screenDepth;
		//	}

		//	DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderTarget, ( 0, pSurf ) )
		//	{
		//		if( renderTarget != NULL )
		//		{
		//			pSurf->Release();
		//		}

		//		return false;
		//	}

		//	if( renderTarget != NULL )
		//	{
		//		pSurf->Release();
		//	}

		//	m_curRenderTexture = renderTarget;
		//}


  //      if( renderTarget == NULL ) 
  //      {
  //          return true;
  //      }

  //      if( renderTarget->isDirty() && _clear )
		//{
		//	this->clear_( 0 );

		//	renderTarget->setDirty( false );
		//}

        return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::supportTextureFormat( PixelFormat _format ) const
	{
		D3DFORMAT dxformat = s_toD3DFormat( _format ); 

		HRESULT hresult = m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_d3dppW.BackBufferFormat, 0, D3DRTYPE_TEXTURE, dxformat );

		if( hresult == D3DERR_NOTAVAILABLE )
		{
			return false;
		}

		IF_DXERRORCHECK(m_serviceProvider, CheckDeviceFormat, hresult)
		{
			return false;
		}

		return true;
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
	bool DX9RenderSystem::supportNPOT_() const
	{
		return m_supportNPOT;
		//return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::syncCPU_()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::initLost_()
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::initLost_ device not created"
                );

            return false;
        }

		// Store render target
		IF_DXCALL( m_serviceProvider, m_pD3DDevice, GetRenderTarget, ( 0, &m_screenSurf ) )
		{
			return false;
		}

		m_screenDepth = NULL;

		//hr = m_pD3DDevice->GetDepthStencilSurface(&m_screenDepth);
		//if( FAILED( hr ) )
		//{
		//	LOGGER_ERROR(m_logSystem)( "Error: DX9RenderSystem::init_lost_ failed to GetDepthStencilSurface (hr:%p)"
		//		, hr 
		//		);
		//}
		
		//this->createSyncTargets_();

		for( TMapVBInfo::iterator 
			it = m_vertexBuffers.begin(), 
			it_end = m_vertexBuffers.end();
		it != it_end;
		++it )
		{
			VBInfo & vbInfo = m_vertexBuffers.get_value(it);

			IF_DXCALL( m_serviceProvider, m_pD3DDevice, CreateVertexBuffer, ( vbInfo.length, vbInfo.usage, vbInfo.fvf, vbInfo.pool, &vbInfo.pVB, NULL ) )
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
			IBInfo& ibInfo = m_indexBuffers.get_value(it);

			IF_DXCALL( m_serviceProvider, m_pD3DDevice, CreateIndexBuffer, ( ibInfo.length, ibInfo.usage, ibInfo.format, ibInfo.pool, &ibInfo.pIB, NULL ) )
			{
				return false;
			}
		}

		// Set common render states
		//m_pD3DDevice->SetVertexShader( D3DFVF_MENGE_VERTEX );

		//hr = m_pD3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
		//if( FAILED( hr ) )
		//{
		//	LOGGER_ERROR(m_logService)( "Error: DX9RenderSystem::init_lost_ failed to SetTextureStageState (hr:%p)"
		//		, hr 
		//		);
		//}

        //this->clear_( 0 );

   //     if( m_caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
   //     {
   //         IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHAREF, (DWORD)0x00000001 ) )
			//{
			//	return false;
			//}

   //         IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL ) )
			//{
			//	return false;
			//}
   //     }

		this->refreshRenderStates_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels, DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * ppTexture )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::d3dCreateTexture_ device not created"
                );

            return false;
        }

		D3DFORMAT dx_format = s_toD3DFormat( Format );

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, CreateTexture, ( Width, Height, MipLevels, Usage, dx_format, Pool, ppTexture, NULL ) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clear_( DWORD _color )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::clear_ device not created"
                );

            return;
        }

		if( m_curRenderTexture != NULL )
		{
			//if( m_curRenderTexture->getDepthInterface() )
			//{
			//	hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
			//	if( FAILED( hr ) )
			//	{
			//		LOGGER_ERROR(m_logSystem)( "Error: DX9RenderSystem::clear failed to Clear (hr:%p)"
			//			, hr 
			//			);
			//	}
			//}
			//else
			{
				DXCALL( m_serviceProvider, m_pD3DDevice, Clear, ( 0, NULL, D3DCLEAR_TARGET, _color, 0.0f, 0 ) );
			}
		}
		else
		{
			//if( bZBuffer )
			//{
            //hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.0f, 0 );
            //if( FAILED( hr ) )
            //{
            //    LOGGER_ERROR(m_logSystem)( "Error: DX9RenderSystem::clear failed to Clear (hr:%p)"
            //        , hr 
            //        );
            //}
			//}
			//else
			//{
			//m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
			//}
			
			//hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 0.0f, 0 );
			DXCALL( m_serviceProvider, m_pD3DDevice, Clear, ( 0, NULL, D3DCLEAR_TARGET, _color, 0.0f, 0 ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureMatrix( size_t _stage, const float* _texture )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureMatrix device not created"
                );

            return;
        }

		if( _texture != nullptr )
		{
			DWORD state = D3DTTFF_COUNT2;

			if( m_textureStageStates[_stage][D3DTSS_TEXTURETRANSFORMFLAGS] != state )
			{
				m_textureStageStates[_stage][D3DTSS_TEXTURETRANSFORMFLAGS] = state;
	
				IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, state ) )
				{
					return;
				}
			}

			D3DTRANSFORMSTATETYPE level = static_cast<D3DTRANSFORMSTATETYPE>( static_cast<DWORD>( D3DTS_TEXTURE0 ) + _stage );
			IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetTransform, ( level, (const D3DMATRIX*)_texture ) )
			{
				return;
			}
		}
		else
		{
			DWORD state = D3DTTFF_DISABLE;

			if( m_textureStageStates[_stage][D3DTSS_TEXTURETRANSFORMFLAGS] != state )
			{
				m_textureStageStates[_stage][D3DTSS_TEXTURETRANSFORMFLAGS] = state;

				IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, state ) )
				{
					return;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setWorldMatrix( const mt::mat4f & _view )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setWorldMatrix device not created"
                );

            return;
        }

		DXCALL( m_serviceProvider, m_pD3DDevice, SetTransform, ( D3DTS_WORLD, (D3DMATRIX*)_view.buff() ) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::loadSurfaceFromSurface_( LPDIRECT3DSURFACE9 pDestSurface, CONST RECT * pDestRect,  LPDIRECT3DSURFACE9 pSrcSurface, CONST RECT * pSrcRect )
	{
		D3DLOCKED_RECT dstLockedRect;
		D3DLOCKED_RECT srcLockedRect;
		RECT dstRect;
		RECT srcRect;
		UINT srcWidth = 0, srcHeight = 0, dstWidth = 0, dstHeight = 0;

		D3DSURFACE_DESC dest_desc;
		IF_DXCALL( m_serviceProvider, pDestSurface, GetDesc, ( &dest_desc ) )
		{
			return false;
		}

		D3DSURFACE_DESC src_desc;
		IF_DXCALL( m_serviceProvider, pSrcSurface, GetDesc, ( &src_desc ) )
		{
			return false;
		}

		if( pDestRect != nullptr )
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

		if( pSrcRect != nullptr )
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

		IF_DXCALL( m_serviceProvider, pDestSurface, LockRect, ( &dstLockedRect, pDestRect, D3DLOCK_DISCARD ) )
		{
			return false;
		}

		IF_DXCALL( m_serviceProvider, pSrcSurface, LockRect, ( &srcLockedRect, pSrcRect, D3DLOCK_READONLY ) )
		{
			pDestSurface->UnlockRect();

			return false;
		}

		if( srcWidth == dstWidth && srcHeight == dstHeight )
		{
			unsigned char* srcdata = (unsigned char*)srcLockedRect.pBits;
			unsigned char* dstdata = (unsigned char*)dstLockedRect.pBits;

			if( src_desc.Format == D3DFMT_R5G6B5 )
			{
				for( UINT i = 0; i != srcHeight; ++i )
				{
					for( UINT j = 0; j != srcWidth; ++j )
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
				for( UINT i = 0; i != srcHeight; ++i )
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
			uint64 temp;

			uint64 sy_48 = (stepy >> 1) - 1;
			for (unsigned int y = (unsigned int)dstRect.top; y < (unsigned int)dstRect.bottom; y++, sy_48+=stepy )
			{
				temp = sy_48 >> 36;
				temp = (temp > 0x800)? temp - 0x800: 0;
				uint64 syf = temp & 0xFFF;
				unsigned int sy1 = temp >> 12;
				unsigned int sy2 = (std::min<unsigned int>)(sy1+1, srcRect.bottom-srcRect.top-1);
				unsigned int syoff1 = sy1 * srcRowPitch;
				unsigned int syoff2 = sy2 * srcRowPitch;

				uint64 sx_48 = (stepx >> 1) - 1;
				for (unsigned int x = (unsigned int)dstRect.left; x < (unsigned int)dstRect.right; x++, sx_48+=stepx)
				{
					temp = sx_48 >> 36;
					temp = (temp > 0x800)? temp - 0x800 : 0;
					unsigned int sxf = temp & 0xFFF;
					unsigned int sx1 = temp >> 12;
					unsigned int sx2 = (std::min<unsigned int>)(sx1+1, srcRect.right-srcRect.left-1);

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
    bool DX9RenderSystem::releaseResources_()
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::restore_ m_pD3DDevice is null"
                );

            return false;
        }

        if( m_screenSurf != nullptr )
        {
            m_screenSurf->Release();
            m_screenSurf = nullptr;
        }
        //if(m_screenDepth) m_screenDepth->Release();

        if( m_currentIB != 0 )
        {
            m_currentIB = 0;

            IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetIndices, ( nullptr ) )
            {
				return false;
            }
        }

        if( m_currentVB != 0 )
        {
            m_currentVB = 0;

            IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetStreamSource, ( 0, nullptr, 0, 0 ) )
            {
                return false;
            }            
        }

        for( TMapVBInfo::iterator 
            it = m_vertexBuffers.begin(), 
            it_end = m_vertexBuffers.end();
        it != it_end;
        ++it )
        {
            VBInfo & vb = m_vertexBuffers.get_value(it);

            if( vb.pVB != nullptr )
            {
                vb.pVB->Release();
                vb.pVB = nullptr;
            }
        }


        for( TMapIBInfo::iterator 
            it = m_indexBuffers.begin(), 
            it_end = m_indexBuffers.end();
        it != it_end;
        ++it )
        {
            IBInfo & ib = m_indexBuffers.get_value(it);

            if( ib.pIB != nullptr )
            {
                ib.pIB->Release();
                ib.pIB = nullptr;
            }
        }

        for( size_t i = 0; i != m_caps.MaxSimultaneousTextures; ++i )
        {
            IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetTexture, ( i, nullptr ) )
			{
				return false;
			}

            m_currentTexture[i] = nullptr;
        }        

        if( m_listener != nullptr )
        {
            m_listener->onRenderSystemDeviceLost();
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::restore_()
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::restore_ m_pD3DDevice is null"
                );

            return false;
        }

        if( this->releaseResources_() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::restore_ release resources"
                );

            return false;
        }
        
		HRESULT hr = m_pD3DDevice->Reset( m_d3dpp );

        if (hr == D3DERR_DEVICELOST)
        {
            return false;
        }
        else if( FAILED( hr ) == true )
		{
            LOGGER_ERROR(m_serviceProvider)( "Error: DX9RenderSystem::restore failed to reset device (hr:%p)"
                , hr 
                );

			return false;
		}

		if( this->initLost_() == false )
		{
            LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::restore_ invalid init lost"
                );

			return false;
		}

		//if(procGfxRestoreFunc) return procGfxRestoreFunc();
        if( m_listener != nullptr )
        {
            if( m_listener->onRenderSystemDeviceRestored() == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::restore_ invalid onRenderSystemDeviceRestored"
                    );

                return false;
            }
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::release_()
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::release_ m_pD3DDevice is null"
                );

            return;
        }

		if( this->releaseResources_() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::release_ invalid release resource"
                );

            return;
        }

		if( m_pD3DDevice != nullptr ) 
		{ 
			m_pD3DDevice->Release(); 
			m_pD3DDevice = nullptr; 
		}

		if( m_pD3D != nullptr ) 
		{ 
			m_pD3D->Release();
			m_pD3D = nullptr; 
		}
	}
	//////////////////////////////////////////////////////////////////////////
	VBHandle DX9RenderSystem::createVertexBuffer( size_t _verticesNum, size_t _vertexSize, bool _dynamic )
	{
        if( m_pD3DDevice == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::createVertexBuffer device not created"
                );

            return 0;
        }

        DWORD Usage = D3DUSAGE_WRITEONLY;

        if( _dynamic == true )
        {
            Usage |= D3DUSAGE_DYNAMIC;
        }

		IDirect3DVertexBuffer9 * vb = nullptr;
		IF_DXCALL(m_serviceProvider, m_pD3DDevice, CreateVertexBuffer, ( _verticesNum * _vertexSize, Usage, Vertex2D_declaration, D3DPOOL_DEFAULT, &vb, NULL ) )
		{
			return 0;
		}

		VBInfo vbInfo;
		vbInfo.length = _verticesNum * _vertexSize;
		vbInfo.vertexSize = _vertexSize;
		vbInfo.usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
		vbInfo.fvf = Vertex2D_declaration;
		vbInfo.pool = D3DPOOL_DEFAULT;
		vbInfo.pVB = vb;
		
		VBHandle newVBHandleCounter = ++m_vbHandleCounter;
		m_vertexBuffers.insert( newVBHandleCounter, vbInfo );

		return m_vbHandleCounter;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::releaseVertexBuffer( VBHandle _vbHandle )
	{
		VBInfo * vb;
		if( m_vertexBuffers.has( _vbHandle, &vb ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::releaseVertexBuffer not found vb handle %d"
                , _vbHandle
                );

            return;
        }
 
        if( vb->pVB )
        {
            vb->pVB->Release();
			vb->pVB = nullptr;
        }

        m_vertexBuffers.erase( _vbHandle );
	}
	//////////////////////////////////////////////////////////////////////////
	IBHandle DX9RenderSystem::createIndexBuffer( size_t _indiciesNum, bool _dynamic )
	{
        if( m_pD3DDevice == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::createIndexBuffer device not created"
                );

            return 0;
        }

        DWORD Usage = D3DUSAGE_WRITEONLY;

        if( _dynamic == true )
        {
            Usage |= D3DUSAGE_DYNAMIC;
        }

		IDirect3DIndexBuffer9 * ib = nullptr;
		IF_DXCALL( m_serviceProvider, m_pD3DDevice, CreateIndexBuffer, ( sizeof(uint16) * _indiciesNum, Usage, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, NULL ) )
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
		m_indexBuffers.insert( newIBHandleCounter, ibInfo );

		return m_ibHandleCounter;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::releaseIndexBuffer( IBHandle _ibHandle )
	{
		IBInfo * info = nullptr;
		if( m_indexBuffers.has( _ibHandle, &info ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::releaseIndexBuffer not found ib handle %d"
                , _ibHandle
                );

            return;
        }
        
        if( info->pIB )
        {
            info->pIB->Release();
			info->pIB = nullptr;
        }

        m_indexBuffers.erase( _ibHandle );
	}
	//////////////////////////////////////////////////////////////////////////
	void * DX9RenderSystem::lockVertexBuffer( VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags )
	{
        VBInfo * vbinfo = nullptr;
        if( m_vertexBuffers.has( _vbHandle, &vbinfo ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::lockVertexBuffer not found %d"
                , _vbHandle
                );

            return nullptr;
        }

        if( _offset + _size > vbinfo->length )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::lockVertexBuffer %d lock %d size %d"
                , _vbHandle
                , _offset + _size
                , vbinfo->length
                );

            return nullptr;
        }

		IDirect3DVertexBuffer9 * vb = vbinfo->pVB;

		void * lock = nullptr;

		IF_DXCALL( m_serviceProvider, vb, Lock, ( _offset, _size, &lock, _flags ) )
		{
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::lockVertexBuffer vertex buffer %d invalid"
                , _vbHandle
                );

			return nullptr;
		}

		return lock;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::unlockVertexBuffer( VBHandle _vbHandle )
	{
        VBInfo * vbinfo;

        if( m_vertexBuffers.has( _vbHandle, &vbinfo ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::unlockVertexBuffer not found %d"
                , _vbHandle
                );

            return false;
        }

        IDirect3DVertexBuffer9 * vb = vbinfo->pVB;

		IF_DXCALL( m_serviceProvider, vb, Unlock, () )
        {
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::unlockVertexBuffer vertex buffer %d invalid"
				, _vbHandle
				);

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void * DX9RenderSystem::lockIndexBuffer( IBHandle _ibHandle, size_t _offset, size_t _size, uint32 _flags )
	{
        IBInfo * ibinfo;

        if( m_indexBuffers.has( _ibHandle, &ibinfo ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::lockIndexBuffer not found %d"
                , _ibHandle
                );

            return nullptr;
        }

        IDirect3DIndexBuffer9 * ib = ibinfo->pIB;
        
		void * lock = nullptr;
		IF_DXCALL( m_serviceProvider, ib, Lock, ( _offset, _size, &lock, _flags ) )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::lockIndexBuffer index buffer %d invalid"
                , _ibHandle
                );

            return nullptr;
        }

		return lock;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::unlockIndexBuffer( IBHandle _ibHandle )
	{
        IBInfo * ibinfo;

        if( m_indexBuffers.has( _ibHandle, &ibinfo ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::unlockIndexBuffer not found %d"
                , _ibHandle
                );

            return nullptr;
        }

        IDirect3DIndexBuffer9 * ib = ibinfo->pIB;

		IF_DXCALL( m_serviceProvider, ib, Unlock, () )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::unlockIndexBuffer index buffer %d invalid"
                , _ibHandle
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setVertexBuffer( VBHandle _vbHandle )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setVertexBuffer device not created"
                );

            return;
        }

		if( _vbHandle == 0 )
		{
			return;
		}

        VBInfo * vbInfo;
		if( m_vertexBuffers.has( _vbHandle, &vbInfo ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setVertexBuffer not found %d"
                , _vbHandle
                );

			return;
		}

        IDirect3DVertexBuffer9 * vb = vbInfo->pVB;

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetStreamSource, ( 0, vb, 0, vbInfo->vertexSize ) )
		{
            return;
		}

        m_currentVB = _vbHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex )
	{
        (void) _baseVertexIndex;
        
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setIndexBuffer device not created"
                );

            return;
        }

		if( _ibHandle == 0 )
		{
			return;
		}
		
        IBInfo * ibInfo;
		if( m_indexBuffers.has( _ibHandle, &ibInfo ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setIndexBuffer not found %d"
                , _ibHandle
                );

			return;
		}

		IDirect3DIndexBuffer9 * ib = ibInfo->pIB;

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetIndices, ( ib ) )
		{
			return;
		}

        m_currentIB = _ibHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::drawIndexedPrimitive( EPrimitiveType _type, size_t _baseVertexIndex,
		size_t _minIndex, size_t _verticesNum, size_t _startIndex, size_t _indexCount )
	{
        (void)_baseVertexIndex;

        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::drawIndexedPrimitive device not created"
                );

            return;
        }

		D3DPRIMITIVETYPE primitiveType = s_toD3DPrimitiveType( _type );

		UINT primCount = s_getPrimitiveCount( _type, _indexCount );

		DXCALL( m_serviceProvider, m_pD3DDevice, DrawIndexedPrimitive
			, (primitiveType, _baseVertexIndex, _minIndex, _verticesNum, _startIndex, primCount ) 
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTexture( size_t _stage, const RenderImageInterfacePtr & _texture )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTexture device not created"
                );

            return;
        }

        RenderImageInterface * image = _texture.get();

        if( image != nullptr )
        {
            DX9Texture * t = static_cast<DX9Texture *>(image);

            m_currentTexture[_stage] = t->getDXTextureInterface();
        }
        else
        {
            m_currentTexture[_stage] = nullptr;
        }

        IDirect3DTexture9 * texture = m_currentTexture[_stage];

		DXCALL( m_serviceProvider, m_pD3DDevice, SetTexture, ( _stage, texture ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setSrcBlendFactor( EBlendFactor _src )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setSrcBlendFactor device not created"
                );

            return;
        }

		DWORD factor = s_toD3DBlend( _src );

		if( m_renderStates[D3DRS_SRCBLEND] == factor )
		{
			return;
		}

		m_renderStates[D3DRS_SRCBLEND] = factor;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_SRCBLEND, factor ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDstBlendFactor( EBlendFactor _dst )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setDstBlendFactor device not created"
                );

            return;
        }

		DWORD factor = s_toD3DBlend( _dst );

		if( m_renderStates[D3DRS_DESTBLEND] == factor )
		{
			return;
		}

		m_renderStates[D3DRS_DESTBLEND] = factor;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_DESTBLEND, factor ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{	
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureAddressing device not created"
                );

            return;
        }

        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureAddressing device not created"
                );

            return;
        }

		D3DTEXTUREADDRESS adrU = s_toD3DTextureAddress( _modeU );	

		if( m_samplerStates[_stage][D3DSAMP_ADDRESSU] != adrU )
		{
			m_samplerStates[_stage][D3DSAMP_ADDRESSU] = adrU;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetSamplerState, ( _stage, D3DSAMP_ADDRESSU, adrU ) );
		}

        D3DTEXTUREADDRESS adrV = s_toD3DTextureAddress( _modeV );

		if( m_samplerStates[_stage][D3DSAMP_ADDRESSV] != adrV )
		{
			m_samplerStates[_stage][D3DSAMP_ADDRESSV] = adrV;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetSamplerState, ( _stage, D3DSAMP_ADDRESSV, adrV ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureFactor( uint32 _color )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureFactor device not created"
                );

            return;
        }

		DWORD color = _color;

		if( m_renderStates[D3DRS_TEXTUREFACTOR] == color )
		{
			return;
		}

		m_renderStates[D3DRS_TEXTUREFACTOR] = color;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_TEXTUREFACTOR, color ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setCullMode( ECullMode _mode )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setCullMode device not created"
                );

            return;
        }

		D3DCULL mode = s_toD3DCullMode( _mode );

		if( m_renderStates[D3DRS_CULLMODE] == mode )
		{
			return;
		}

		m_renderStates[D3DRS_CULLMODE] = mode;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_CULLMODE, mode ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferTestEnable( bool _depthTest )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setDepthBufferTestEnable device not created"
                );

            return;
        }

		D3DZBUFFERTYPE test = _depthTest ? D3DZB_TRUE : D3DZB_FALSE;

		if( m_renderStates[D3DRS_ZENABLE] == test )
		{
			return;
		}

		m_renderStates[D3DRS_ZENABLE] = test;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ZENABLE, test ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setDepthBufferWriteEnable device not created"
                );

            return;
        }

		DWORD dWrite = _depthWrite ? TRUE : FALSE;

		if( m_renderStates[D3DRS_ZWRITEENABLE] == dWrite )
		{
			return;
		}

		m_renderStates[D3DRS_ZWRITEENABLE] = dWrite;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ZWRITEENABLE, dWrite ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setDepthBufferCmpFunc device not created"
                );

            return;
        }

		D3DCMPFUNC func = s_toD3DCmpFunc( _depthFunction );

		if( m_renderStates[D3DRS_ZFUNC] == func )
		{
			return;
		}

		m_renderStates[D3DRS_ZFUNC] = func;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ZFUNC, func ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setFillMode( EFillMode _mode )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setFillMode device not created"
                );

            return;
        }

		D3DFILLMODE mode = s_toD3DFillMode( _mode );

		if( m_renderStates[D3DRS_FILLMODE] == mode )
		{
			return;
		}

		m_renderStates[D3DRS_FILLMODE] = mode;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_FILLMODE, mode ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setColorBufferWriteEnable device not created"
                );

            return;
        }

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

		if( m_renderStates[D3DRS_COLORWRITEENABLE] == value )
		{
			return;
		}

		m_renderStates[D3DRS_COLORWRITEENABLE] = value;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_COLORWRITEENABLE, value ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setShadeType( EShadeType _sType )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setShadeType device not created"
                );

            return;
        }

		D3DSHADEMODE mode = s_toD3DShadeMode( _sType );

		if( m_renderStates[D3DRS_SHADEMODE] == mode )
		{
			return;
		}

		m_renderStates[D3DRS_SHADEMODE] = mode;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_SHADEMODE, mode ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaTestEnable( bool _alphaTest )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setAlphaTestEnable device not created"
                );

            return;
        }

		DWORD alphaTest = _alphaTest ? TRUE : FALSE;

		if( m_renderStates[D3DRS_ALPHATESTENABLE] == alphaTest )
		{
			return;
		}

		m_renderStates[D3DRS_ALPHATESTENABLE] = alphaTest;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHATESTENABLE, alphaTest ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setAlphaBlendEnable device not created"
                );

            return;
        }

		DWORD alphaBlend = _alphaBlend ? TRUE : FALSE;

		if( m_renderStates[D3DRS_ALPHABLENDENABLE] == alphaBlend )
		{
			return;
		}

		m_renderStates[D3DRS_ALPHABLENDENABLE] = alphaBlend;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHABLENDENABLE, alphaBlend ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setAlphaCmpFunc device not created"
                );

            return;
        }

		D3DCMPFUNC func = s_toD3DCmpFunc( _alphaFunc );

		if( m_renderStates[D3DRS_ALPHAFUNC] != func )
		{
			m_renderStates[D3DRS_ALPHAFUNC] = func;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHAFUNC, func ) );
		}

		DWORD alpha = _alpha;

		if( m_renderStates[D3DRS_ALPHAREF] != alpha )
		{
			m_renderStates[D3DRS_ALPHAREF] = alpha;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHAREF, alpha ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setLightingEnable( bool _light )
	{
        if( m_pD3DDevice == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setLightingEnable device not created"
                );

            return;
        }

		DWORD value = _light ? TRUE : FALSE;

		if( m_renderStates[D3DRS_LIGHTING] == value )
		{
			return;
		}

		m_renderStates[D3DRS_LIGHTING] = value;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_LIGHTING, value ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureStageColorOp device not created"
                );

            return;
        }
				
		D3DTEXTUREOP colorOp = s_toD3DTextureOp( _textrueOp );
		if( m_textureStageStates[_stage][D3DTSS_COLOROP] != colorOp )
		{
			m_textureStageStates[_stage][D3DTSS_COLOROP] = colorOp;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_COLOROP, colorOp ) );
		}

		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		if( m_textureStageStates[_stage][D3DTSS_COLORARG1] != arg1 )
		{
			m_textureStageStates[_stage][D3DTSS_COLORARG1] = arg1;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_COLORARG1, arg1 ) );
		}

		DWORD arg2 = s_toD3DTextureArg( _arg2 );
		if( m_textureStageStates[_stage][D3DTSS_COLORARG2] != arg2 )
		{
			m_textureStageStates[_stage][D3DTSS_COLORARG2] = arg2;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_COLORARG2, arg2 ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureStageAlphaOp device not created"
                );

            return;
        }

		D3DTEXTUREOP alphaOp = s_toD3DTextureOp( _textrueOp );
		if( m_textureStageStates[_stage][D3DTSS_ALPHAOP] != alphaOp )
		{
			m_textureStageStates[_stage][D3DTSS_ALPHAOP] = alphaOp;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_ALPHAOP, alphaOp ) );
		}

		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		if( m_textureStageStates[_stage][D3DTSS_ALPHAARG1] != arg1 )
		{
			m_textureStageStates[_stage][D3DTSS_ALPHAARG1] = arg1;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_ALPHAARG1, arg1 ) );
		}

		DWORD arg2 = s_toD3DTextureArg( _arg2 );
		if( m_textureStageStates[_stage][D3DTSS_ALPHAARG2] != arg2 )
		{
			m_textureStageStates[_stage][D3DTSS_ALPHAARG2] = arg2;

			DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_ALPHAARG2, arg2 ) );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureStageTexCoordIndex( size_t _stage, size_t _index )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureStageAlphaOp device not created"
                );

            return;
        }
		
		DWORD index = _index;

		if( m_textureStageStates[_stage][D3DTSS_TEXCOORDINDEX] == index )
		{
			return;
		}

		m_textureStageStates[_stage][D3DTSS_TEXCOORDINDEX] = index;
		
		DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_TEXCOORDINDEX, index ) );		
    }
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureStageFilter device not created"
                );

            return;
        }

		D3DSAMPLERSTATETYPE textureFilterType = s_toD3DTextureFilterType( _filterType );
		D3DTEXTUREFILTERTYPE textureFilter = s_toD3DTextureFilter( _filter );

		if( m_samplerStates[_stage][textureFilterType] == textureFilter )
		{
			return;
		}

		m_samplerStates[_stage][textureFilterType] = textureFilter;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetSamplerState, ( _stage, textureFilterType, textureFilter ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setVertexDeclaration( size_t _vertexSize, uint32 _declaration )
	{
        (void)_vertexSize;

        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setVertexDeclaration device not created"
                );

            return;
        }

		DXCALL( m_serviceProvider, m_pD3DDevice, SetFVF, ( _declaration ) );
	}
    //////////////////////////////////////////////////////////////////////////
    RenderShaderInterface * DX9RenderSystem::createShader( const void * _code, size_t _len )
    {
        (void)_len;

        LPDIRECT3DPIXELSHADER9 pixelShader;
        
        IF_DXCALL( m_serviceProvider, m_pD3DDevice, CreatePixelShader, ( (DWORD *)_code, &pixelShader ) )
		{
			return nullptr;
		}

        DX9RenderShader * dx9_shader = new DX9RenderShader(pixelShader);

        return dx9_shader;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setShader( RenderShaderInterface * _shader )
    {
        DX9RenderShader * dx9_shader = static_cast<DX9RenderShader *>(_shader);

        LPDIRECT3DPIXELSHADER9 pixelShader = dx9_shader->getPixelShader();

        DXCALL( m_serviceProvider, m_pD3DDevice, SetPixelShader, ( pixelShader ) );
    }
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setVSync( bool _vSync )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setVSync device not created"
                );

            return;
        }

        m_waitForVSync = _vSync;

        this->updateVSyncDPP_();

		if( this->restore_() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "DX9RenderSystem::setVSync: Graphics change mode failed" 
                );
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::updateVSyncDPP_()
    {
        if( m_waitForVSync == true )
        {
            m_d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            m_d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        }
        else
        { 
            m_d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            m_d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
		//D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
		//mt::mat4f scale;
		//mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );
		
		//D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
		//mt::mat4f translation;
        //mt::ident_m4( translation );
		//mt::make_translation_m4( translation, -0.5f, -0.5f, 0.0f );

		//D3DXMatrixMultiply(&matProj, &matProj, &tmp);
		//mt::mat4f transform;
		//mt::mul_m4_m4( transform, scale, translation );

		//D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
		//mt::mat4f ortho;
		mt::make_projection_ortho_lh_m4( _projectionMatrix, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );
				
		//D3DXMatrixMultiply(&matProj, &matProj, &tmp);
		//mt::mul_m4_m4( _projectionMatrix, transform, ortho );
					
		//D3DXMatrixOrthoOffCenterLH()
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
		//D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
		//mt::mat4f scale;
		//mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );

		//D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
		//mt::mat4f translation;
		//mt::make_translation_m4( translation, -0.5f, -0.5f, 0.0f );

		//D3DXMatrixMultiply(&matProj, &matProj, &tmp);
		//mt::mat4f transform;
		//mt::mul_m4_m4( transform, scale, translation );

		//D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
		//mt::mat4f frustum;
		mt::make_projection_frustum_m4( _projectionMatrix, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

		//D3DXMatrixMultiply(&matProj, &matProj, &tmp);
		//mt::mul_m4_m4( _projectionMatrix, transform, frustum );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float _zn, float _zf )
	{
		//D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
		//mt::mat4f scale;
		//mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );

		//D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
		//mt::mat4f translation;
		//mt::make_translation_m4( translation, -0.5f, +0.5f, 0.0f );

		//D3DXMatrixMultiply(&matProj, &matProj, &tmp);
		//mt::mat4f transform;
		//mt::mul_m4_m4( transform, scale, translation );

		//mt::mat4f projection_fov;
		mt::make_projection_fov_m4( _projectionMatrix, _fov, _aspect, _zn, _zf );

		//mt::mul_m4_m4( _projectionMatrix, transform, projection_fov );
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport )
    {
        (void)_viewport;

        mt::mat4f wm;
        mt::ident_m4( wm );

        mt::inv_m4( _viewMatrix, wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _at, const mt::vec3f & _up )
    {
        mt::make_lookat_m4( _viewMatrix, _eye, _at, _up );
    }
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clear( uint32 _color )
	{
		this->clear_( _color );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setSeparateAlphaBlendMode()
	{
		//empty, not supported
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
