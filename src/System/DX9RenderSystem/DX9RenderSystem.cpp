#	include "DX9RenderSystem.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/PlatformInterface.h"

#	include "DX9RenderEnum.h"
#	include "DX9ErrorHelper.h"

#	include "DX9RenderTargetOffscreen.h"

#	include <algorithm>
#	include <cmath>
#	include <stdio.h>

#	include "Logger/Logger.h"


//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Menge::DX9RenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{    
	//////////////////////////////////////////////////////////////////////////
	typedef IDirect3D9* (WINAPI *PDIRECT3DCREATE9)(UINT);
	//////////////////////////////////////////////////////////////////////////
	DX9RenderSystem::DX9RenderSystem()
		: m_pD3D(nullptr)
		, m_pD3DDevice(nullptr)
        , m_hd3d9(NULL)
        , m_adapterToUse(D3DADAPTER_DEFAULT)
        , m_deviceType(D3DDEVTYPE_HAL)
        , m_waitForVSync(false)
		, m_oldRenderTarget(nullptr)
		, m_vertexBufferEnable(false)
		, m_indexBufferEnable(false)
		, m_vertexDeclaration(0)
		, m_frames(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderSystem::~DX9RenderSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & DX9RenderSystem::getRenderPlatformName() const
	{
		return m_renderPlatform;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::_initialize()
	{
		m_frames = 0;

		m_hd3d9 = ::LoadLibrary( L"d3d9.dll" );

		if( m_hd3d9 == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Failed to load d3d9.dll"
                );

			return false;
		}

		PDIRECT3DCREATE9 pDirect3DCreate9 = (PDIRECT3DCREATE9)::GetProcAddress( m_hd3d9, "Direct3DCreate9" );
		if( pDirect3DCreate9 == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("Failed to get Direct3DCreate9 proc address"
                );

			return false;
		}

		// Init D3D
		LOGGER_INFO(m_serviceProvider)( "Initializing DX9RenderSystem..." 
            );

		m_pD3D = pDirect3DCreate9( D3D_SDK_VERSION ); // D3D_SDK_VERSION

		if( m_pD3D == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Can't create D3D interface" 
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
			LOGGER_ERROR(m_serviceProvider)("Can't determine adapter identifier" 
				);
			
			return false;
		}

        IF_DXCALL(m_serviceProvider, m_pD3D, GetAdapterIdentifier, ( m_adapterToUse, 0, &AdID ) )
        {
            LOGGER_ERROR(m_serviceProvider)("Can't determine adapter identifier" 
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
			LOGGER_ERROR(m_serviceProvider)("Can't determine desktop video mode"
                );

			return false;
		}

		if( Mode.Format == D3DFMT_UNKNOWN )
		{
			LOGGER_ERROR(m_serviceProvider)("Can't determine desktop video mode D3DFMT_UNKNOWN"
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

			if( Mode.Width != screenWidth || Mode.Height != screenHeight )
			{
				continue;
			}

			//if(nScreenBPP==16 && (_format_id(Mode.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
			if( s_lessD3DFormats( Format, Mode.Format ) == true )
			{
				Format = Mode.Format;
			}
		}
		
		if( Format == D3DFMT_UNKNOWN )
		{
			LOGGER_ERROR(m_serviceProvider)("Can't find appropriate full screen video mode" 
				);

			return false;
		}

		ZeroMemory( &m_d3dppFS, sizeof(m_d3dppFS) );
		m_d3dppFS.BackBufferFormat = Format;

		D3DCAPS9 caps;
		IF_DXCALL( m_serviceProvider, m_pD3D, GetDeviceCaps, (m_adapterToUse, m_deviceType, &caps) )
		{
			return false;
		}

		if( caps.MaxSimultaneousTextures < MENGE_MAX_TEXTURE_STAGES )
		{
			LOGGER_ERROR(m_serviceProvider)("Render dont't support %d texture stages (%d support)"
				, MENGE_MAX_TEXTURE_STAGES
				, caps.MaxSimultaneousTextures
				);

			return false;
		}
	
		m_renderPlatform = STRINGIZE_STRING_LOCAL( m_serviceProvider, "DX9" );
			
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::_finalize()
    {
        this->release_();

        if( m_hd3d9 != NULL )
        {
            FreeLibrary( m_hd3d9 );
            m_hd3d9 = NULL;
        }
    }
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::createRenderWindow( const Resolution & _resolution, uint32_t _bits, 
		bool _fullscreen, bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
        (void)_bits;
        (void)_FSAAType;
        (void)_FSAAQuality;

		m_windowResolution = _resolution;		

		m_fullscreen = _fullscreen;
        m_waitForVSync = _waitForVSync;
				
		m_d3dppW.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
		m_d3dppW.Windowed = TRUE;
		//m_d3dppW.Flags			= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		m_d3dppW.BackBufferWidth = m_windowResolution.getWidth();
		m_d3dppW.BackBufferHeight = m_windowResolution.getHeight();
		m_d3dppW.BackBufferCount = 1;

		WindowHandle windowHandle = PLATFORM_SERVICE( m_serviceProvider )
			->getWindowHandle();

		m_d3dppW.hDeviceWindow = (HWND)windowHandle;

		m_d3dppW.SwapEffect = D3DSWAPEFFECT_DISCARD;
		
		// Set up Windowed presentation parameters
        D3DDISPLAYMODE Mode;
		IF_DXCALL( m_serviceProvider, m_pD3D, GetAdapterDisplayMode, (D3DADAPTER_DEFAULT, &Mode) ) 
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::createRenderWindow Can't determine desktop video mode" 
                );

			return false;
		}

		m_d3dppW.BackBufferFormat = Mode.Format;

		m_d3dppW.EnableAutoDepthStencil = FALSE;
		m_d3dppW.AutoDepthStencilFormat = D3DFMT_UNKNOWN;

		m_d3dppW.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		
		m_d3dppFS.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
        m_d3dppFS.Windowed = FALSE;

		m_d3dppFS.BackBufferWidth = m_windowResolution.getWidth();
		m_d3dppFS.BackBufferHeight = m_windowResolution.getHeight();
		m_d3dppFS.BackBufferCount = 1;

		m_d3dppFS.hDeviceWindow = (HWND)windowHandle;

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

		m_screenBits = s_getD3DFormatBits( m_d3dpp->BackBufferFormat );

		//_fullscreen ? MENGE_LOG_INFO( "fullscreen mode" ) : MENGE_LOG_INFO( "windowed mode" );

		//MENGE_LOG_INFO( "attempting to create d3ddevice: %dx%dx%d\nBackbuffer format %d\nDepthstencil format %d",
		//	d3dpp->BackBufferWidth, d3dpp->BackBufferHeight, m_screenBits, d3dpp->BackBufferFormat, d3dpp->AutoDepthStencilFormat );

		// Create D3D Device
		HRESULT hr;

		hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
			m_d3dpp, &m_pD3DDevice );

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
				m_d3dpp, &m_pD3DDevice );
		}

		if( FAILED( hr ) )
		{
			Sleep( 100 );
			hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
				D3DCREATE_MIXED_VERTEXPROCESSING |D3DCREATE_FPU_PRESERVE ,
				m_d3dpp, &m_pD3DDevice );

			if( FAILED( hr ) )
			{
				Sleep( 100 );
				hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE ,
					m_d3dpp, &m_pD3DDevice );
			}
		}

		if( FAILED ( hr ) )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::createRenderWindow Can't create D3D device (hr:%u, hwnd:%u)"
				, hr
				, (HWND)windowHandle
				);

			return false;
		}

		LOGGER_INFO(m_serviceProvider)( "Mode: %d x %d x %s\n"
			, m_windowResolution.getWidth()
			, m_windowResolution.getHeight()
			, s_getD3DFormatName( m_d3dpp->BackBufferFormat )
			);

		DWORD FVF_UV = (MENGINE_RENDER_VERTEX_UV_COUNT << D3DFVF_TEXCOUNT_SHIFT) & D3DFVF_TEXCOUNT_MASK;

		m_vertexDeclaration = D3DFVF_XYZ | D3DFVF_DIFFUSE | FVF_UV;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetFVF, (m_vertexDeclaration) );

		LOGGER_WARNING( m_serviceProvider )("DX9RenderSystem initalized successfully!");
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect )
	{	
		(void)_image;
		(void)_rect;

		return false;
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

 		DXCALL( m_serviceProvider, m_pD3DDevice, SetTransform, ( D3DTS_VIEW, (D3DMATRIX*)_modelview.buff() ) );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr DX9RenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
		(void)_depth;

		IDirect3DTexture9 * dxTextureInterface = nullptr;
        
		if( this->d3dCreateTexture_( _width, _height, _mipmaps, 0,	_format, D3DPOOL_MANAGED, &dxTextureInterface ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem.createImage: can't create texture %dx%d %d"
				, _width
				, _height
				, _format
                );

			return nullptr;
		}

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( m_serviceProvider, dxTextureInterface, GetLevelDesc, ( 0, &texDesc ) )
		{
			return nullptr;
		}

		DX9RenderImage * dxTexture = m_factoryDX9Texture.createObject();
        dxTexture->initialize( m_serviceProvider, dxTextureInterface, ERIM_NORMAL, texDesc.Width, texDesc.Height, _channels, _format );

		LOGGER_INFO(m_serviceProvider)( "DX9RenderSystem.createImage: texture created %dx%d %d:%d"
			, texDesc.Width
			, texDesc.Height
			, texDesc.Format
            , _channels
			);

		return dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr DX9RenderSystem::createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
		(void)_depth;

		IDirect3DTexture9 * dxTextureInterface = nullptr;

		if( this->d3dCreateTexture_( _width, _height, 1, 0, _format, D3DPOOL_MANAGED, &dxTextureInterface ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem.createDynamicImage: can't create texture %dx%d %d"
				, _width
				, _height
				, _format
				);

			return nullptr;
		}

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( m_serviceProvider, dxTextureInterface, GetLevelDesc, ( 0, &texDesc ) )
		{
			return nullptr;
		}

        DX9RenderImage * dxTexture = m_factoryDX9Texture.createObject();
        dxTexture->initialize( m_serviceProvider, dxTextureInterface, ERIM_DYNAMIC, texDesc.Width, texDesc.Height, _channels, _format );		
        
		LOGGER_INFO(m_serviceProvider)( "DX9RenderSystem.createDynamicImage: texture created %dx%d %d:%d"
			, texDesc.Width
			, texDesc.Height
			, texDesc.Format
            , _channels
			);

		return dxTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTargetInterface * DX9RenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format )
	{
		DX9RenderTargetOffscreen * target = new DX9RenderTargetOffscreen();

		target->setServiceProvider( m_serviceProvider );

		if( target->initialize( m_pD3DDevice, _width, _height, _format ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem.createRenderTargetOffscreen: can't initialize offscreen target %dx%d format %d"
				, _width
				, _height
				, _format
				);

			return nullptr;
		}
		
        LOGGER_INFO(m_serviceProvider)( "DX9RenderSystem.createRenderTargetOffscreen: offscreen target created %dx%d %d"
			, _width
			, _height
			, _format
            );

		return target;
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
                LOGGER_ERROR(m_serviceProvider)("Can't determine desktop video mode D3DFMT_UNKNOWN"
                    );

                return false;
            }

            m_d3dppW.BackBufferFormat = Mode.Format;

			m_screenBits = s_getD3DFormatBits( Mode.Format );
        }

        if( this->restore_() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::resetDevice_: restore failed"
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
			LOGGER_ERROR( m_serviceProvider )("DX9RenderSystem::beginScene device not created"
				);

			return false;
		}

		HRESULT hr = m_pD3DDevice->TestCooperativeLevel();

		if( hr == D3DERR_DEVICELOST )
		{
			LOGGER_WARNING( m_serviceProvider )("DX9RenderSystem::beginScene: D3DERR_DEVICELOST"
				);

			::Sleep( 100 );

			return false;
		}
		else if( hr == D3DERR_DEVICENOTRESET )
		{
			LOGGER_WARNING( m_serviceProvider )("DX9RenderSystem::beginScene: D3DERR_DEVICENOTRESET"
				);

			if( this->resetDevice_() == false )
			{
				::Sleep( 50 );

				return false;
			}
		}
		else if( FAILED( hr ) )
		{
			LOGGER_ERROR( m_serviceProvider )("DX9RenderSystem::beginScene: invalid TestCooperativeLevel %d"
				, hr
				);

			if( this->releaseResources_() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("DX9RenderSystem::restore_ release resources"
					);
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
            this->updateViewport_( clear_viewport );

            this->clear_( 0 );

            this->updateViewport_( m_viewport );
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
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::swapBuffers failed to swap buffers" );
		}

		++m_frames;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil )
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
	void DX9RenderSystem::setClipplaneCount( uint32_t _count )
	{
		DWORD CLIPPLANEENABLE = 0U;

		switch( _count )
		{
		case 0:			
			break;
		case 1:
			CLIPPLANEENABLE = D3DCLIPPLANE0;
			break;
		case 2:
			CLIPPLANEENABLE = D3DCLIPPLANE0 | D3DCLIPPLANE1;
			break;
		case 3:
			CLIPPLANEENABLE = D3DCLIPPLANE0 | D3DCLIPPLANE1 | D3DCLIPPLANE2;
			break;
		case 4:
			CLIPPLANEENABLE = D3DCLIPPLANE0 | D3DCLIPPLANE1 | D3DCLIPPLANE2 | D3DCLIPPLANE3;
			break;
		case 5:
			CLIPPLANEENABLE = D3DCLIPPLANE0 | D3DCLIPPLANE1 | D3DCLIPPLANE2 | D3DCLIPPLANE3 | D3DCLIPPLANE4;
			break;
		case 6:
			CLIPPLANEENABLE = D3DCLIPPLANE0 | D3DCLIPPLANE1 | D3DCLIPPLANE2 | D3DCLIPPLANE3 | D3DCLIPPLANE4 | D3DCLIPPLANE5;
			break;
		}

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, (D3DRS_CLIPPLANEENABLE, CLIPPLANEENABLE) );
		//DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, (D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0) );

		//mt::planef p( 1.f, 0.f, 0.f, -1000.f );
		//DXCALL( m_serviceProvider, m_pD3DDevice, SetClipPlane, (0, p.buff()) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setClipplane( uint32_t _i, const mt::planef & _plane )
	{
		DXCALL( m_serviceProvider, m_pD3DDevice, SetClipPlane, (_i, _plane.buff()) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;

		this->updateViewport_( _viewport );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::updateViewport_( const Viewport & _viewport )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setViewport device not created"
                );

            return;
        }

		D3DVIEWPORT9 VP;

		VP.X = (DWORD)::floorf( _viewport.begin.x + 0.5f );
		VP.Y = (DWORD)::floorf( _viewport.begin.y + 0.5f );

		float width = _viewport.getWidth();
		float height = _viewport.getHeight();

		VP.Width = (DWORD)::floorf( width + 0.5f );
		VP.Height = (DWORD)::floorf( height + 0.5f );

		VP.MinZ = 0.f;
		VP.MaxZ = 1.f;

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetViewport, (&VP) )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setRenderViewport: failed viewport (%d, %d, %d, %d)"
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
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::changeWindowMode: Graphics change mode failed" 
				);
		}        
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::lockRenderTarget( const RenderImageInterfacePtr & _renderTarget )
	{
		ERenderImageMode mode = _renderTarget->getMode();

		if( mode != ERIM_RENDER_TARGET )
		{
			return false;
		}

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, GetRenderTarget, (0, &m_oldRenderTarget) )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::lockRenderTarget: can't get render target"
				);

			return false;
		}

		DX9TexturePtr texture = stdex::intrusive_static_cast<DX9TexturePtr>(_renderTarget);

		IDirect3DTexture9 * dx_texture = texture->getDXTextureInterface();

		IDirect3DSurface9 * dx_surface;
		IF_DXCALL( m_serviceProvider, dx_texture, GetSurfaceLevel, (0, &dx_surface) )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::lockRenderTarget: can't get surface level"
				);

			return false;
		}

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderTarget, ( 0, dx_surface ) )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::lockRenderTarget: can't set render target"
				);

			return false;
		}

		dx_surface->Release();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::unlockRenderTarget()
	{
		if( m_oldRenderTarget == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::unlockRenderTarget: can't get surface level"
				);

			return false;
		}

		IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderTarget, ( 0, m_oldRenderTarget ) )
		{
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::unlockRenderTarget: can't set render target"
				);

			return false;
		}

		m_oldRenderTarget->Release();
		m_oldRenderTarget = nullptr;

		return true;
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
	bool DX9RenderSystem::supportTextureNonPow2() const
	{
		D3DCAPS9 caps;
		IF_DXCALL( m_serviceProvider, m_pD3D, GetDeviceCaps, (m_adapterToUse, m_deviceType, &caps) )
		{
			return false;
		}

		if( (caps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0 )
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
	void DX9RenderSystem::syncCPU_()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::d3dCreateTexture_( uint32_t Width, uint32_t Height, uint32_t MipLevels, DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * ppTexture )
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

		DXCALL( m_serviceProvider, m_pD3DDevice, Clear, ( 0, NULL, D3DCLEAR_TARGET, _color, 0.f, 0 ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureMatrix( uint32_t _stage, const float* _texture )
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

			IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, state ) )
			{
				return;
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

			IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_TEXTURETRANSFORMFLAGS, state ) )
			{
				return;
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
    bool DX9RenderSystem::releaseResources_()
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::restore_ m_pD3DDevice is null"
                );

            return false;
        }

		if( m_vertexBufferEnable == true )
        {
			m_vertexBufferEnable = false;

            IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetIndices, ( nullptr ) )
            {
				return false;
            }
        }

		if( m_indexBufferEnable == true )
        {
			m_indexBufferEnable = false;

            IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetStreamSource, ( 0, nullptr, 0, 0 ) )
            {
                return false;
            }
        }
		
        for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
        {
            IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetTexture, ( i, nullptr ) )
			{
				LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::releaseResources_ texture %d not reset"
					, i
					);

				return false;
			}
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

		if( hr == D3DERR_DEVICELOST )
        {
			::Sleep(100);

            return false;
        }
        else if( FAILED( hr ) == true )
		{
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::restore_ failed to reset device (hr:%p)"
                , hr 
                );

			return false;
		}

		DWORD FVF_UV = (MENGINE_RENDER_VERTEX_UV_COUNT << D3DFVF_TEXCOUNT_SHIFT) & D3DFVF_TEXCOUNT_MASK;

		m_vertexDeclaration = D3DFVF_XYZ | D3DFVF_DIFFUSE | FVF_UV;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetFVF, (m_vertexDeclaration) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::release_()
	{
		if( m_pD3DDevice == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("DX9RenderSystem::release_ m_pD3DDevice is null"
				);

			return;
		}

		if( this->releaseResources_() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("DX9RenderSystem::release_ invalid release resource"
				);

			return;
		}

		if( m_pD3DDevice != nullptr )
		{
			ULONG ref = m_pD3DDevice->Release();
			(void)ref;
			m_pD3DDevice = nullptr;
		}

		if( m_pD3D != nullptr )
		{
			ULONG ref = m_pD3D->Release();
			(void)ref;
			m_pD3D = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertexBufferInterfacePtr DX9RenderSystem::createVertexBuffer( uint32_t _verticesNum, bool _dynamic )
	{
		DX9RenderVertexBufferPtr buffer = m_factoryVertexBuffer.createObject();

		if( buffer->initialize( m_serviceProvider, m_pD3DDevice, m_vertexDeclaration, _verticesNum, _dynamic ) == false )
		{
			return nullptr;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
	{
		m_vertexBufferEnable = false;

		if( _vertexBuffer == nullptr )
		{
			IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetStreamSource, (0, nullptr, 0, 0) )
			{
				return false;
			}

			return true;
		}

		DX9RenderVertexBuffer * vb = stdex::intrusive_get<DX9RenderVertexBuffer *>( _vertexBuffer );

		if( vb->enable() == false )
		{
			return false;
		}

		m_vertexBufferEnable = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderIndexBufferInterfacePtr DX9RenderSystem::createIndexBuffer( uint32_t _indiciesNum, bool _dynamic )
	{
		DX9RenderIndexBufferPtr buffer = m_factoryIndexBuffer.createObject();

		if( buffer->initialize( m_serviceProvider, m_pD3DDevice, _indiciesNum, _dynamic ) == false )
		{
			return nullptr;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
	{
		m_indexBufferEnable = false;

		if( _indexBuffer == nullptr )
		{
			IF_DXCALL( m_serviceProvider, m_pD3DDevice, SetIndices, (nullptr) )
			{
				return false;
			}

			return true;
		}

		DX9RenderIndexBuffer * ib = stdex::intrusive_get<DX9RenderIndexBuffer *>( _indexBuffer );

		if( ib->enable() == false )
		{
			return false;
		}

		m_indexBufferEnable = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::drawIndexedPrimitive( EPrimitiveType _type, uint32_t _baseVertexIndex,
		uint32_t _minIndex, uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount )
	{
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
	void DX9RenderSystem::setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTexture device not created"
                );

            return;
        }

		IDirect3DTexture9 * dx_texture = nullptr;
		
		if( _texture != nullptr )
        {
			DX9RenderImage * image = _texture.getT<DX9RenderImage *>();
			
			dx_texture = image->getDXTextureInterface();
        }

		DXCALL( m_serviceProvider, m_pD3DDevice, SetTexture, (_stage, dx_texture) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setSrcBlendFactor device not created"
                );

            return;
        }

		DWORD src_factor = s_toD3DBlend( _src );
		DWORD dst_factor = s_toD3DBlend( _dst );

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, (D3DRS_SRCBLEND, src_factor) );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, (D3DRS_DESTBLEND, dst_factor) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{	
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureAddressing device not created"
                );

            return;
        }

		D3DTEXTUREADDRESS adrU = s_toD3DTextureAddress( _modeU );	
		DXCALL( m_serviceProvider, m_pD3DDevice, SetSamplerState, ( _stage, D3DSAMP_ADDRESSU, adrU ) );

        D3DTEXTUREADDRESS adrV = s_toD3DTextureAddress( _modeV );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetSamplerState, ( _stage, D3DSAMP_ADDRESSV, adrV ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureFactor( uint32_t _color )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureFactor device not created"
                );

            return;
        }

		DWORD color = _color;

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

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, (D3DRS_ZENABLE, test) );
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
		
		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_SHADEMODE, mode ) );
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

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHABLENDENABLE, alphaBlend ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8_t _alpha )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setAlphaCmpFunc device not created"
                );

            return;
        }

		D3DCMPFUNC func = s_toD3DCmpFunc( _alphaFunc );

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHAFUNC, func ) );

		DWORD alpha = _alpha;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_ALPHAREF, alpha ) );
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

		DXCALL( m_serviceProvider, m_pD3DDevice, SetRenderState, ( D3DRS_LIGHTING, value ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureStageColorOp device not created"
                );

            return;
        }
				
		D3DTEXTUREOP colorOp = s_toD3DTextureOp( _textrueOp );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_COLOROP, colorOp ) );

		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_COLORARG1, arg1 ) );

		DWORD arg2 = s_toD3DTextureArg( _arg2 );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_COLORARG2, arg2 ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureStageAlphaOp device not created"
                );

            return;
        }

		D3DTEXTUREOP alphaOp = s_toD3DTextureOp( _textrueOp );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_ALPHAOP, alphaOp ) );

		DWORD arg1 = s_toD3DTextureArg( _arg1 );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_ALPHAARG1, arg1 ) );

		DWORD arg2 = s_toD3DTextureArg( _arg2 );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_ALPHAARG2, arg2 ) );
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureStageAlphaOp device not created"
                );

            return;
        }
		
		DWORD index = _index;

		DXCALL( m_serviceProvider, m_pD3DDevice, SetTextureStageState, ( _stage, D3DTSS_TEXCOORDINDEX, index ) );		
    }
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
	{
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setTextureStageFilter device not created"
                );

            return;
        }

		D3DTEXTUREFILTERTYPE dx_minification = s_toD3DTextureFilter( _minification );
		D3DTEXTUREFILTERTYPE dx_mipmap = s_toD3DTextureFilter( _mipmap );
		D3DTEXTUREFILTERTYPE dx_magnification = s_toD3DTextureFilter( _magnification );

		DXCALL( m_serviceProvider, m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MINFILTER, dx_minification) );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MIPFILTER, dx_mipmap) );
		DXCALL( m_serviceProvider, m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MAGFILTER, dx_magnification) );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderShaderInterfacePtr DX9RenderSystem::createFragmentShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile )
	{
		(void)_name;
		(void)_buffer;
		(void)_size;
		(void)_isCompile;

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderShaderInterfacePtr DX9RenderSystem::createVertexShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile )
	{
		(void)_name;
		(void)_buffer;
		(void)_size;
		(void)_isCompile;

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderProgramInterfacePtr DX9RenderSystem::createProgram( const ConstString & _name, const RenderShaderInterfacePtr & _fragment, const RenderShaderInterfacePtr & _vertex, uint32_t _samplerCount )
	{
		(void)_name;
		(void)_fragment;
		(void)_vertex;
		(void)_samplerCount;

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
	{
		(void)_program;
		//None
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
			LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem::setVSync: Graphics change mode failed" 
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
		mt::make_projection_ortho_lh_m4( _projectionMatrix, _viewport.begin.x + 0.5f, _viewport.end.x + 0.5f, _viewport.begin.y + 0.5f, _viewport.end.y + 0.5f, _near, _far );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
		mt::make_projection_frustum_m4( _projectionMatrix, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float _zn, float _zf )
	{
		mt::make_projection_fov_m4( _projectionMatrix, _fov, _aspect, _zn, _zf );
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
    void DX9RenderSystem::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
    {
        mt::make_lookat_m4( _viewMatrix, _eye, _dir, _up, _sign );
    }
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderSystem::clear( uint32_t _color )
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
