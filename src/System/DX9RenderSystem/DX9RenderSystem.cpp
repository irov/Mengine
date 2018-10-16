#include "DX9RenderSystem.h"

#include "Interface/RenderServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/ConfigInterface.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

#include "DX9RenderImage.h"
#include "DX9RenderImageTarget.h"
#include "DX9RenderTargetTexture.h"
#include "DX9RenderTargetOffscreen.h"

#include "DX9RenderVertexShader.h"
#include "DX9RenderFragmentShader.h"
#include "DX9RenderProgram.h"
#include "DX9RenderVertexAttribute.h"
#include "DX9RenderVertexBuffer.h"
#include "DX9RenderIndexBuffer.h"
#include "DX9RenderProgramVariable.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryAssertion.h"

#include <algorithm>
#include <cmath>
#include <stdio.h>

#include "Kernel/Logger.h"


//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Mengine::DX9RenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IDirect3D9* (WINAPI *PDIRECT3DCREATE9)(UINT);
    //////////////////////////////////////////////////////////////////////////
    DX9RenderSystem::DX9RenderSystem()
        : m_pD3D( nullptr )
        , m_pD3DDevice( nullptr )
        , m_hd3d9( NULL )
        , m_adapterToUse( D3DADAPTER_DEFAULT )
        , m_deviceType( D3DDEVTYPE_HAL )
        , m_waitForVSync( false )
        , m_vertexBufferEnable( false )
        , m_indexBufferEnable( false )
        , m_frames( 0 )
        , m_dxMaxCombinedTextureImageUnits( 0 )
        , m_textureMemoryUse( 0U )
        , m_textureCount( 0U )
    {
        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            m_textureEnable[i] = false;
        }

        mt::ident_m4( m_projectionMatrix );
        mt::ident_m4( m_modelViewMatrix );
        mt::ident_m4( m_worldMatrix );
        mt::ident_m4( m_totalWVPInvMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderSystem::~DX9RenderSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderPlatform DX9RenderSystem::getRenderPlatformType() const
    {
        return RP_DX9;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX9RenderSystem::getRenderPlatformName() const
    {
        return m_renderPlatformName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::_initializeService()
    {
        m_frames = 0;

        WString d3d9_dll = CONFIG_VALUE( "Render", "D3D9_DLL", L"d3d9.dll" );

        m_hd3d9 = ::LoadLibrary( d3d9_dll.c_str() );

        if( m_hd3d9 == nullptr )
        {
            LOGGER_ERROR( "Failed to load d3d9.dll"
            );

            return false;
        }

        PDIRECT3DCREATE9 pDirect3DCreate9 = (PDIRECT3DCREATE9)::GetProcAddress( m_hd3d9, "Direct3DCreate9" );
        if( pDirect3DCreate9 == NULL )
        {
            LOGGER_ERROR( "Failed to get Direct3DCreate9 proc address"
            );

            return false;
        }

        // Init D3D
        LOGGER_INFO( "Initializing DX9RenderSystem..."
        );

        m_pD3D = pDirect3DCreate9( D3D_SDK_VERSION ); // D3D_SDK_VERSION

        if( m_pD3D == nullptr )
        {
            LOGGER_ERROR( "Can't create D3D interface"
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

        // Set up Windowed presentation parameters
        D3DDISPLAYMODE Mode;
        IF_DXCALL( m_pD3D, GetAdapterDisplayMode, (m_adapterToUse, &Mode) )
        {
            LOGGER_ERROR( "DX9RenderSystem::createRenderWindow Can't determine desktop video mode"
            );

            return false;
        }

        m_displayMode = Mode;

        // Get adapter info
        D3DADAPTER_IDENTIFIER9 AdID;
        IF_DXCALL( m_pD3D, GetAdapterIdentifier, (m_adapterToUse, 0, &AdID) )
        {
            LOGGER_ERROR( "Can't determine adapter identifier"
            );

            return false;
        }

        LOGGER_WARNING( "DirectX Driver: %s", AdID.Driver );
        LOGGER_WARNING( "Description: %s", AdID.Description );
        LOGGER_WARNING( "DeviceName: %s", AdID.DeviceName );

        LOGGER_WARNING( "Version: %d.%d.%d.%d"
            , HIWORD( AdID.DriverVersion.HighPart )
            , LOWORD( AdID.DriverVersion.HighPart )
            , HIWORD( AdID.DriverVersion.LowPart )
            , LOWORD( AdID.DriverVersion.LowPart )
        );

        LOGGER_WARNING( "VendorId: %d", AdID.VendorId );
        LOGGER_WARNING( "DeviceId: %d", AdID.DeviceId );
        LOGGER_WARNING( "SubSysId: %d", AdID.SubSysId );
        LOGGER_WARNING( "Revision: %d", AdID.Revision );

        D3DCAPS9 caps;
        IF_DXCALL( m_pD3D, GetDeviceCaps, (m_adapterToUse, m_deviceType, &caps) )
        {
            return false;
        }

        //if( caps.MaxSimultaneousTextures < MENGINE_MAX_TEXTURE_STAGES )
        //{
        //	LOGGER_ERROR("Render dont't support %d texture stages (%d support)"
        //		, MENGINE_MAX_TEXTURE_STAGES
        //		, caps.MaxSimultaneousTextures
        //		);

        //	return false;
        //}

        m_dxMaxCombinedTextureImageUnits = caps.MaxSimultaneousTextures;

        if( m_dxMaxCombinedTextureImageUnits > MENGINE_MAX_TEXTURE_STAGES )
        {
            m_dxMaxCombinedTextureImageUnits = MENGINE_MAX_TEXTURE_STAGES;
        }

        m_renderPlatformName = STRINGIZE_STRING_LOCAL( "DX9" );

        m_factoryRenderVertexAttribute = new FactoryPool<DX9RenderVertexAttribute, 8>();
        m_factoryRenderVertexShader = new FactoryPool<DX9RenderVertexShader, 16>();
        m_factoryRenderFragmentShader = new FactoryPool<DX9RenderFragmentShader, 16>();
        m_factoryRenderProgram = new FactoryPool<DX9RenderProgram, 16>();
        m_factoryRenderProgramVariable = new FactoryPool<DX9RenderProgramVariable, 64>();
        m_factoryVertexBuffer = new FactoryDefault<DX9RenderVertexBuffer>();
        m_factoryIndexBuffer = new FactoryDefault<DX9RenderIndexBuffer>();

        m_factoryDX9Image = Helper::makeFactoryPoolWithListener<DX9RenderImage, 128>( this, &DX9RenderSystem::onDestroyDX9RenderImage_ );
        m_factoryDX9ImageTarget = Helper::makeFactoryPoolWithListener<DX9RenderImageTarget, 16>( this, &DX9RenderSystem::onDestroyDX9RenderImageTarget_ );

        m_factoryDX9TargetTexture = Helper::makeFactoryPoolWithListener<DX9RenderTargetTexture, 16>( this, &DX9RenderSystem::onDestroyDX9RenderTargetTexture_ );
        m_factoryDX9TargetOffscreen = Helper::makeFactoryPoolWithListener<DX9RenderTargetOffscreen, 16>( this, &DX9RenderSystem::onDestroyDX9RenderTargetOffscreen_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::_finalizeService()
    {
        m_deferredCompileVertexShaders.clear();
        m_deferredCompileFragmentShaders.clear();
        m_deferredCompilePrograms.clear();

        this->release_();

        if( m_hd3d9 != NULL )
        {
            FreeLibrary( m_hd3d9 );
            m_hd3d9 = NULL;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexAttribute );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderFragmentShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgram );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgramVariable );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryVertexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryIndexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDX9Image );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDX9TargetTexture );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDX9TargetOffscreen );

        m_factoryRenderVertexAttribute = nullptr;
        m_factoryRenderVertexShader = nullptr;
        m_factoryRenderFragmentShader = nullptr;
        m_factoryRenderProgram = nullptr;
        m_factoryRenderProgramVariable = nullptr;
        m_factoryVertexBuffer = nullptr;
        m_factoryIndexBuffer = nullptr;

        m_factoryDX9Image = nullptr;

        m_factoryDX9TargetTexture = nullptr;
        m_factoryDX9TargetOffscreen = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::createRenderWindow( const Resolution & _resolution, uint32_t _bits,
        bool _fullscreen, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount )
    {
        (void)_bits;
        (void)_FSAAType;
        (void)_FSAAQuality;

        m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_fullscreen = _fullscreen;
        m_waitForVSync = _waitForVSync;

        D3DMULTISAMPLE_TYPE multiSampleType = D3DMULTISAMPLE_NONE;
        for( uint32_t MultiSampleIndex = _MultiSampleCount; MultiSampleIndex != 0; --MultiSampleIndex )
        {
            D3DMULTISAMPLE_TYPE testMultiSampleType = s_getMultiSampleType( MultiSampleIndex );

            HRESULT hr_checkDeviceMultiSampleType = m_pD3D->CheckDeviceMultiSampleType(
                m_adapterToUse, m_deviceType, m_displayMode.Format,
                TRUE, testMultiSampleType, NULL
            );

            if( FAILED( hr_checkDeviceMultiSampleType ) )
            {
                LOGGER_ERROR( "DX9RenderSystem::createRenderWindow can't support multi sample count '%u' error [%p]"
                    , testMultiSampleType
                    , hr_checkDeviceMultiSampleType
                );

                continue;
            }

            multiSampleType = testMultiSampleType;
            break;
        }

        ZeroMemory( &m_d3dppW, sizeof( m_d3dppW ) );
        ZeroMemory( &m_d3dppFS, sizeof( m_d3dppFS ) );

        m_d3dppW.BackBufferWidth = m_windowResolution.getWidth();
        m_d3dppW.BackBufferHeight = m_windowResolution.getHeight();
        m_d3dppW.BackBufferFormat = m_displayMode.Format;
        m_d3dppW.BackBufferCount = 1;

        m_d3dppW.MultiSampleType = D3DMULTISAMPLE_NONE;
        m_d3dppW.MultiSampleQuality = 0;

        m_d3dppW.SwapEffect = D3DSWAPEFFECT_FLIP;

        HWND windowHandle = PLATFORM_SERVICE()
            ->getWindowHandle();

        m_d3dppW.hDeviceWindow = windowHandle;

        m_d3dppW.Windowed = TRUE;

        m_d3dppW.EnableAutoDepthStencil = FALSE;
        m_d3dppW.AutoDepthStencilFormat = D3DFMT_UNKNOWN;

        //m_d3dppW.Flags			= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

        m_d3dppW.FullScreen_RefreshRateInHz = 0;

        m_d3dppFS.MultiSampleType = multiSampleType;
        m_d3dppFS.MultiSampleQuality = 0;
        m_d3dppFS.Windowed = FALSE;

        m_d3dppFS.BackBufferWidth = m_windowResolution.getWidth();
        m_d3dppFS.BackBufferHeight = m_windowResolution.getHeight();
        m_d3dppFS.BackBufferCount = 1;

        m_d3dppFS.hDeviceWindow = (HWND)windowHandle;

        if( multiSampleType == D3DMULTISAMPLE_NONE )
        {
            m_d3dppFS.SwapEffect = D3DSWAPEFFECT_FLIP;
        }
        else
        {
            m_d3dppFS.SwapEffect = D3DSWAPEFFECT_DISCARD;
        }

        m_d3dppFS.BackBufferFormat = m_displayMode.Format;

        m_d3dppFS.EnableAutoDepthStencil = FALSE;
        m_d3dppFS.AutoDepthStencilFormat = D3DFMT_UNKNOWN;

        m_d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

        this->updateVSyncDPP_();

        if( _fullscreen == true )
        {
            m_d3dpp = &m_d3dppFS;
        }
        else
        {
            m_d3dpp = &m_d3dppW;
        }

        // Create D3D Device

        D3DCAPS9 caps;
        IF_DXCALL( m_pD3D, GetDeviceCaps, (m_adapterToUse, m_deviceType, &caps) )
        {
            return false;
        }

        LOGGER_WARNING( "DX9RenderSystem::createRenderWindow VertexShaderVersion [%u] [%u]"
            , caps.VertexShaderVersion
            , caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 )
        );

        LOGGER_WARNING( "DX9RenderSystem::createRenderWindow PixelShaderVersion [%u] [%u] [%u]"
            , caps.PixelShaderVersion
            , caps.PixelShaderVersion < D3DPS_VERSION( 1, 1 )
            , caps.PixelShaderVersion >= D3DPS_VERSION( 2, 0 )
        );

        HRESULT hr;

        if( (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
            caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        {
            LOGGER_ERROR( "DX9RenderSystem::createRenderWindow can't support D3DCREATE_HARDWARE_VERTEXPROCESSING try to create D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE"
            );

            hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
                D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                m_d3dpp, &m_pD3DDevice );
        }
        else
        {
            hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
                D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
                m_d3dpp, &m_pD3DDevice );

            if( FAILED( hr ) )
            {
                LOGGER_ERROR( "DX9RenderSystem::createRenderWindow Can't create D3D device D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE (hr:%u) Try another"
                    , hr
                );

                Sleep( 100 );
                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
                    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
                    m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                LOGGER_ERROR( "DX9RenderSystem::createRenderWindow Can't create D3D device D3DCREATE_HARDWARE_VERTEXPROCESSING (hr:%u) Try another"
                    , hr
                );

                Sleep( 100 );
                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
                    D3DCREATE_HARDWARE_VERTEXPROCESSING,
                    m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                LOGGER_ERROR( "DX9RenderSystem::createRenderWindow Can't create D3D device D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE (hr:%u) Try another"
                    , hr
                );

                Sleep( 100 );
                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
                    D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
                    m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                LOGGER_ERROR( "DX9RenderSystem::createRenderWindow Can't create D3D device D3DCREATE_MIXED_VERTEXPROCESSING (hr:%u) Try another"
                    , hr
                );

                Sleep( 100 );
                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
                    D3DCREATE_MIXED_VERTEXPROCESSING,
                    m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                LOGGER_ERROR( "DX9RenderSystem::createRenderWindow Can't create D3D device D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE (hr:%u) Try another"
                    , hr
                );

                Sleep( 100 );
                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, (HWND)windowHandle,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
                    m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                LOGGER_ERROR( "DX9RenderSystem::createRenderWindow Can't create D3D device D3DDEVTYPE_REF | D3DCREATE_SOFTWARE_VERTEXPROCESSING (hr:%u) Try another"
                    , hr
                );

                Sleep( 100 );
                hr = m_pD3D->CreateDevice( m_adapterToUse, D3DDEVTYPE_REF, (HWND)windowHandle,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                    m_d3dpp, &m_pD3DDevice );
            }
        }

        if( FAILED( hr ) )
        {
            LOGGER_ERROR( "DX9RenderSystem::createRenderWindow Can't create D3D device (hr:%u, hwnd:%u) BackBuffer Size %d:%d Format %d"
                , hr
                , (HWND)windowHandle
                , m_d3dpp->BackBufferWidth
                , m_d3dpp->BackBufferHeight
                , m_d3dpp->BackBufferFormat
            );

            return false;
        }

        LOGGER_INFO( "Mode: %d x %d x %s\n"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
            , s_getD3DFormatName( m_displayMode.Format )
        );

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ALPHATESTENABLE, FALSE) );

        LOGGER_WARNING( "DX9RenderSystem initalized successfully!" );

        for( const DX9RenderVertexShaderPtr & shader : m_deferredCompileVertexShaders )
        {
            if( shader->compile( m_pD3DDevice ) == false )
            {
                return false;
            }
        }

        m_deferredCompileVertexShaders.clear();

        for( const DX9RenderFragmentShaderPtr & shader : m_deferredCompileFragmentShaders )
        {
            if( shader->compile( m_pD3DDevice ) == false )
            {
                return false;
            }
        }

        m_deferredCompileFragmentShaders.clear();

        for( const DX9RenderProgramPtr & program : m_deferredCompilePrograms )
        {
            if( program->compile( m_pD3DDevice ) == false )
            {
                return false;
            }
        }

        m_deferredCompilePrograms.clear();

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
    void DX9RenderSystem::setProjectionMatrix( const mt::mat4f & _projectionMatrix )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setProjectionMatrix device not created"
            );

            return;
        }

        mt::mat4f vmperfect;
        float offset_x = -0.5f / (m_windowViewport.end.x - m_windowViewport.begin.x);
        float offset_y = 0.5f / (m_windowViewport.end.y - m_windowViewport.begin.y);

        mt::make_translation_m4( vmperfect, offset_x, offset_y, 0.f );

        mt::mul_m4_m4( m_projectionMatrix, _projectionMatrix, vmperfect );

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setViewMatrix( const mt::mat4f & _modelViewMatrix )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setModelViewMatrix device not created"
            );

            return;
        }

        m_modelViewMatrix = _modelViewMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setWorldMatrix( const mt::mat4f & _worldMatrix )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setWorldMatrix device not created"
            );

            return;
        }

        m_worldMatrix = _worldMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr DX9RenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
    {
        (void)_depth;

        IDirect3DTexture9 * dxTextureInterface = nullptr;

        if( this->d3dCreateTexture_( _width, _height, _mipmaps, 0, _format, D3DPOOL_MANAGED, &dxTextureInterface ) == false )
        {
            LOGGER_ERROR( "DX9RenderSystem.createImage: can't create texture %dx%d %d"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( dxTextureInterface, GetLevelDesc, (0, &texDesc) )
        {
            return nullptr;
        }

        DX9RenderImagePtr dxTexture = this->createDX9RenderImage_( dxTextureInterface, ERIM_NORMAL, _mipmaps, texDesc.Width, texDesc.Height, _channels, _depth, _format );

        LOGGER_INFO( "DX9RenderSystem.createImage: texture created %dx%d %d:%d depth %d"
            , texDesc.Width
            , texDesc.Height
            , texDesc.Format
            , _channels
            , _depth
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
            LOGGER_ERROR( "DX9RenderSystem.createDynamicImage: can't create texture %dx%d %d"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( dxTextureInterface, GetLevelDesc, (0, &texDesc) )
        {
            return nullptr;
        }

        DX9RenderImagePtr dxTexture = this->createDX9RenderImage_( dxTextureInterface, ERIM_DYNAMIC, 1, texDesc.Width, texDesc.Height, _channels, _depth, _format );

        LOGGER_INFO( "DX9RenderSystem.createDynamicImage: texture created %dx%d %d:%d"
            , texDesc.Width
            , texDesc.Height
            , texDesc.Format
            , _channels
        );

        return dxTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX9RenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _format )
    {
        DX9RenderTargetTexturePtr target = m_factoryDX9TargetTexture->createObject();

        if( target->initialize( m_pD3DDevice, _width, _height, _channels, _format ) == false )
        {
            LOGGER_ERROR( "DX9RenderSystem.createRenderTargetTexture: can't initialize offscreen target %dx%d format %d"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        m_renderResourceHandlers.push_back( target.get() );

        LOGGER_INFO( "DX9RenderSystem.createRenderTargetTexture: offscreen target created %dx%d %d"
            , _width
            , _height
            , _format
        );

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX9RenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _format )
    {
        DX9RenderTargetOffscreenPtr target = m_factoryDX9TargetOffscreen->createObject();

        if( target->initialize( m_pD3DDevice, _width, _height, _channels, _format ) == false )
        {
            LOGGER_ERROR( "DX9RenderSystem.createRenderTargetOffscreen: can't initialize offscreen target %dx%d format %d"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        LOGGER_INFO( "DX9RenderSystem.createRenderTargetOffscreen: offscreen target created %dx%d %d"
            , _width
            , _height
            , _format
        );

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr DX9RenderSystem::createRenderTargetImage( const RenderTargetInterfacePtr & _renderTarget )
    {
        DX9RenderTargetTexturePtr dx9RenderTarget = stdex::intrusive_static_cast<DX9RenderTargetTexturePtr>(_renderTarget);

        DX9RenderImageTargetPtr dx9TextureImageTarget = m_factoryDX9ImageTarget->createObject();

        dx9TextureImageTarget->initialize( dx9RenderTarget );

        return dx9TextureImageTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::resetDevice_()
    {
        if( m_fullscreen == false )
        {
            D3DDISPLAYMODE Mode;
            IF_DXCALL( m_pD3D, GetAdapterDisplayMode, (m_adapterToUse, &Mode) )
            {
                return false;
            }

            if( Mode.Format == D3DFMT_UNKNOWN )
            {
                LOGGER_ERROR( "Can't determine desktop video mode D3DFMT_UNKNOWN"
                );

                return false;
            }

            m_d3dppW.BackBufferFormat = Mode.Format;
        }

        if( this->restore_() == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::resetDevice_: restore failed"
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
            LOGGER_ERROR( "DX9RenderSystem::beginScene device not created"
            );

            return false;
        }

        HRESULT hr = m_pD3DDevice->TestCooperativeLevel();

        if( hr == D3DERR_DEVICELOST )
        {
            ::Sleep( 100 );

            return false;
        }
        else if( hr == D3DERR_DEVICENOTRESET )
        {
            LOGGER_WARNING( "DX9RenderSystem::beginScene: D3DERR_DEVICENOTRESET"
            );

            if( this->resetDevice_() == false )
            {
                ::Sleep( 50 );

                return false;
            }
        }
        else if( FAILED( hr ) )
        {
            LOGGER_ERROR( "DX9RenderSystem::beginScene: invalid TestCooperativeLevel %d"
                , hr
            );

            if( this->releaseResources_() == false )
            {
                LOGGER_ERROR( "DX9RenderSystem::restore_ release resources"
                );
            }

            return false;
        }

        IF_DXCALL( m_pD3DDevice, BeginScene, () )
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
            LOGGER_ERROR( "DX9RenderSystem::endScene device not created"
            );

            return;
        }

        DXCALL( m_pD3DDevice, EndScene, () );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::swapBuffers()
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::swapBuffers device not created"
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
            LOGGER_ERROR( "DX9RenderSystem::swapBuffers failed to swap buffers %x"
                , hr
            );
        }

        ++m_frames;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::clearFrameBuffer device not created"
            );

            return;
        }

        DWORD frameBufferFlags = 0;

        if( (_frameBufferTypes & FBT_COLOR) != 0 )
        {
            frameBufferFlags |= D3DCLEAR_TARGET;
        }

        if( (_frameBufferTypes & FBT_DEPTH) != 0 )
        {
            frameBufferFlags |= D3DCLEAR_ZBUFFER;
        }

        if( (_frameBufferTypes & FBT_STENCIL) != 0 )
        {
            frameBufferFlags |= D3DCLEAR_STENCIL;
        }

        DXCALL( m_pD3DDevice, Clear, (0, NULL, frameBufferFlags, _color, _depth, _stencil) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setScissor( const Viewport & _viewport )
    {
        mt::mat4f pm;
        mt::mul_m4_m4( pm, m_projectionMatrix, m_modelViewMatrix );

        mt::vec2f b;
        mt::mul_v2_v2_m4( b, _viewport.begin, pm );

        mt::vec2f e;
        mt::mul_v2_v2_m4( e, _viewport.end, pm );

        mt::vec2f vs = m_viewport.size();

        float bx = (b.x + 1.f) * 0.5f * vs.x;
        float by = (1.f - (b.y + 1.f) * 0.5f) * vs.y;
        float ex = (e.x + 1.f) * 0.5f * vs.x;
        float ey = (1.f - (e.y + 1.f) * 0.5f) * vs.y;

        RECT r;
        r.left = (uint32_t)bx;
        r.top = (uint32_t)by;
        r.right = (uint32_t)ex;
        r.bottom = (uint32_t)ey;

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SCISSORTESTENABLE, TRUE) );
        DXCALL( m_pD3DDevice, SetScissorRect, (&r) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::removeScissor()
    {
        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SCISSORTESTENABLE, FALSE) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setViewport( const Viewport & _viewport )
    {
        m_viewport = _viewport;

        this->updateViewport_( m_viewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::updateViewport_( const Viewport & _viewport )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setViewport device not created"
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

        IF_DXCALL( m_pD3DDevice, SetViewport, (&VP) )
        {
            LOGGER_ERROR( "DX9RenderSystem::setRenderViewport: failed viewport (%d, %d, %d, %d)"
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

        mt::vec2f windowSize;
        m_windowResolution.calcSize( windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_d3dpp = _fullscreen ? &m_d3dppFS : &m_d3dppW;
        m_d3dpp->BackBufferWidth = m_windowResolution.getWidth();
        m_d3dpp->BackBufferHeight = m_windowResolution.getHeight();

        //nScreenBPP = _bpp;

        if( this->restore_() == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::changeWindowMode: Graphics change mode failed"
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderSystem::getTextureMemoryUse() const
    {
        return m_textureMemoryUse;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderSystem::getTextureCount() const
    {
        return m_textureCount;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::supportTextureFormat( PixelFormat _format ) const
    {
        D3DFORMAT dxformat = s_toD3DFormat( _format );

        HRESULT hresult = m_pD3D->CheckDeviceFormat( m_adapterToUse, m_deviceType, m_displayMode.Format, 0, D3DRTYPE_TEXTURE, dxformat );

        if( hresult == D3DERR_NOTAVAILABLE )
        {
            return false;
        }

        IF_DXERRORCHECK( CheckDeviceFormat, hresult )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::supportTextureNonPow2() const
    {
        D3DCAPS9 caps;
        IF_DXCALL( m_pD3D, GetDeviceCaps, (m_adapterToUse, m_deviceType, &caps) )
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
    uint32_t DX9RenderSystem::getMaxCombinedTextureImageUnits() const
    {
        return m_dxMaxCombinedTextureImageUnits;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onWindowChangeFullscreenPrepare( bool _fullscreen )
    {
        (void)_fullscreen;
        //Empty
    }
    void DX9RenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        (void)_fullscreen;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onWindowMovedOrResized()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onWindowClose()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::d3dCreateTexture_( uint32_t Width, uint32_t Height, uint32_t MipLevels, DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * _ppD3DTexture )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::d3dCreateTexture_ device not created"
            );

            return false;
        }

        D3DFORMAT dx_format = s_toD3DFormat( Format );

        IF_DXCALL( m_pD3DDevice, CreateTexture, (Width, Height, MipLevels, Usage, dx_format, Pool, _ppD3DTexture, NULL) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureMatrix( uint32_t _stage, const mt::mat4f & _matrix )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTextureMatrix device not created"
            );

            return;
        }

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTextureMatrix no support stage %d (max %d)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        DWORD state = D3DTTFF_COUNT2;

        IF_DXCALL( m_pD3DDevice, SetTextureStageState, (_stage, D3DTSS_TEXTURETRANSFORMFLAGS, state) )
        {
            return;
        }

        D3DTRANSFORMSTATETYPE level = static_cast<D3DTRANSFORMSTATETYPE>(static_cast<DWORD>(D3DTS_TEXTURE0) + _stage);
        IF_DXCALL( m_pD3DDevice, SetTransform, (level, (const D3DMATRIX*)_matrix.buff()) )
        {
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::releaseResources_()
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::restore_ m_pD3DDevice is null"
            );

            return false;
        }

        if( m_vertexBufferEnable == true )
        {
            m_vertexBufferEnable = false;

            IF_DXCALL( m_pD3DDevice, SetIndices, (nullptr) )
            {
                LOGGER_ERROR( "DX9RenderSystem::releaseResources_ indices not reset"
                );

                //return false;
            }
        }

        if( m_indexBufferEnable == true )
        {
            m_indexBufferEnable = false;

            IF_DXCALL( m_pD3DDevice, SetStreamSource, (0, nullptr, 0, 0) )
            {
                LOGGER_ERROR( "DX9RenderSystem::releaseResources_ stream source not reset"
                );

                //return false;
            }
        }

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            if( m_textureEnable[i] == false )
            {
                continue;
            }

            IF_DXCALL( m_pD3DDevice, SetTexture, (i, nullptr) )
            {
                LOGGER_ERROR( "DX9RenderSystem::releaseResources_ texture %d not reset"
                    , i
                );

                //return false;
            }

            m_textureEnable[i] = false;
        }

        IF_DXCALL( m_pD3DDevice, SetVertexShader, (nullptr) )
        {
            return false;
        }

        IF_DXCALL( m_pD3DDevice, SetPixelShader, (nullptr) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::restore_()
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::restore_ m_pD3DDevice is null"
            );

            return false;
        }

        if( this->releaseResources_() == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::restore_ release resources"
            );

            return false;
        }

        RENDER_SERVICE()
            ->onDeviceLostPrepare();

        for( DX9RenderResourceHandler * handler : m_renderResourceHandlers )
        {
            handler->onRenderReset();
        }

        HRESULT hr = m_pD3DDevice->Reset( m_d3dpp );

        if( hr == D3DERR_DEVICELOST )
        {
            ::Sleep( 100 );

            return false;
        }
        else if( FAILED( hr ) == true )
        {
            LOGGER_ERROR( "DX9RenderSystem::restore_ failed to reset device (hr:%p)"
                , hr
            );

            return false;
        }

        for( DX9RenderResourceHandler * handler : m_renderResourceHandlers )
        {
            if( handler->onRenderRestore() == false )
            {
                return false;
            }
        }

        RENDER_SERVICE()
            ->onDeviceLostRestore();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::release_()
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::release_ m_pD3DDevice is null"
            );

            return;
        }

        if( this->releaseResources_() == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::release_ invalid release resource"
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
    RenderVertexBufferInterfacePtr DX9RenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType )
    {
        DX9RenderVertexBufferPtr buffer = m_factoryVertexBuffer->createObject();

        if( buffer->initialize( m_pD3DDevice, _vertexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        m_renderResourceHandlers.push_back( buffer.get() );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        m_vertexBufferEnable = false;

        if( _vertexBuffer == nullptr )
        {
            IF_DXCALL( m_pD3DDevice, SetStreamSource, (0, nullptr, 0, 0) )
            {
                return false;
            }

            return true;
        }

        if( _vertexBuffer->enable() == false )
        {
            return false;
        }

        m_vertexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr DX9RenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType )
    {
        DX9RenderIndexBufferPtr buffer = m_factoryIndexBuffer->createObject();

        if( buffer->initialize( m_pD3DDevice, _indexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        m_renderResourceHandlers.push_back( buffer.get() );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        m_indexBufferEnable = false;

        if( _indexBuffer == nullptr )
        {
            IF_DXCALL( m_pD3DDevice, SetIndices, (nullptr) )
            {
                return false;
            }

            return true;
        }

        if( _indexBuffer->enable() == false )
        {
            return false;
        }

        m_indexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::drawIndexedPrimitive( EPrimitiveType _type, uint32_t _vertexBase,
        uint32_t _minIndex, uint32_t _vertexCount, uint32_t _indexStart, uint32_t _indexCount )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::drawIndexedPrimitive device not created"
            );

            return;
        }

        D3DPRIMITIVETYPE primitiveType = s_toD3DPrimitiveType( _type );

        UINT primCount = s_getPrimitiveCount( _type, _indexCount );

        DXCALL( m_pD3DDevice, DrawIndexedPrimitive
            , (primitiveType, _vertexBase, _minIndex, _vertexCount, _indexStart, primCount)
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTexture device not created"
            );

            return;
        }

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTexture no support stage %d (max %d)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        if( _texture != nullptr )
        {
            _texture->bind( _stage );

            m_textureEnable[_stage] = true;
        }
        else
        {
            DXCALL( m_pD3DDevice, SetTexture, (_stage, nullptr) );

            m_textureEnable[_stage] = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setSrcBlendFactor device not created"
            );

            return;
        }

        DWORD src_factor = s_toD3DBlendFactor( _src );
        DWORD dst_factor = s_toD3DBlendFactor( _dst );
        DWORD blend_op = s_toD3DBlendOp( _op );

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SRCBLEND, src_factor) );
        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_DESTBLEND, dst_factor) );
        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_BLENDOP, blend_op) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTextureAddressing device not created"
            );

            return;
        }

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTextureAddressing no support stage %d (max %d)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        D3DTEXTUREADDRESS adrU = s_toD3DTextureAddress( _modeU );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_ADDRESSU, adrU) );

        D3DTEXTUREADDRESS adrV = s_toD3DTextureAddress( _modeV );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_ADDRESSV, adrV) );

        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_BORDERCOLOR, _border) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureFactor( uint32_t _color )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTextureFactor device not created"
            );

            return;
        }

        DWORD color = _color;

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_TEXTUREFACTOR, color) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setCullMode( ECullMode _mode )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setCullMode device not created"
            );

            return;
        }

        D3DCULL mode = s_toD3DCullMode( _mode );

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_CULLMODE, mode) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setDepthBufferTestEnable( bool _depthTest )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setDepthBufferTestEnable device not created"
            );

            return;
        }

        D3DZBUFFERTYPE test = _depthTest ? D3DZB_TRUE : D3DZB_FALSE;

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZENABLE, test) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setDepthBufferWriteEnable device not created"
            );

            return;
        }

        DWORD dWrite = _depthWrite ? TRUE : FALSE;

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZWRITEENABLE, dWrite) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setDepthBufferCmpFunc device not created"
            );

            return;
        }

        D3DCMPFUNC func = s_toD3DCmpFunc( _depthFunction );

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZFUNC, func) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setFillMode( EFillMode _mode )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setFillMode device not created"
            );

            return;
        }

        D3DFILLMODE mode = s_toD3DFillMode( _mode );

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_FILLMODE, mode) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setColorBufferWriteEnable device not created"
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

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_COLORWRITEENABLE, value) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setAlphaBlendEnable device not created"
            );

            return;
        }

        DWORD alphaBlend = _alphaBlend ? TRUE : FALSE;

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ALPHABLENDENABLE, alphaBlend) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTextureStageFilter device not created"
            );

            return;
        }

        if( _stage >= m_dxMaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "DX9RenderSystem::setTextureStageFilter no support stage %d (max %d)"
                , _stage
                , m_dxMaxCombinedTextureImageUnits
            );

            return;
        }

        D3DTEXTUREFILTERTYPE dx_minification = s_toD3DTextureFilter( _minification );
        D3DTEXTUREFILTERTYPE dx_mipmap = s_toD3DTextureFilter( _mipmap );
        D3DTEXTUREFILTERTYPE dx_magnification = s_toD3DTextureFilter( _magnification );

        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MINFILTER, dx_minification) );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MIPFILTER, dx_mipmap) );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MAGFILTER, dx_magnification) );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr DX9RenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize )
    {
        DX9RenderVertexAttributePtr attribute = m_factoryRenderVertexAttribute->createObject();

        if( attribute == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::createVertexAttribute invalid create attribute %s"
                , _name.c_str()
            );

            return nullptr;
        }

        if( attribute->initialize( _name, _elementSize ) == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::createVertexAttribute invalid initialize attribute %s"
                , _name.c_str()
            );

            return nullptr;
        }

        return attribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr DX9RenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        DX9RenderFragmentShaderPtr shader = m_factoryRenderFragmentShader->createObject();

        if( shader == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::createFragmentShader invalid create shader %s"
                , _name.c_str()
            );

            return nullptr;
        }

        if( shader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::createFragmentShader invalid initialize shader %s"
                , _name.c_str()
            );

            return nullptr;
        }

        if( m_pD3DDevice != nullptr )
        {
            if( shader->compile( m_pD3DDevice ) == false )
            {
                LOGGER_ERROR( "DX9RenderSystem::createFragmentShader invalid compile shader %s"
                    , _name.c_str()
                );

                return nullptr;
            }
        }
        else
        {
            m_deferredCompileFragmentShaders.emplace_back( shader );
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr DX9RenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        DX9RenderVertexShaderPtr shader = m_factoryRenderVertexShader->createObject();

        if( shader == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::createVertexShader invalid create shader %s"
                , _name.c_str()
            );

            return nullptr;
        }

        if( shader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::createVertexShader invalid initialize shader %s"
                , _name.c_str()
            );

            return nullptr;
        }

        if( m_pD3DDevice != nullptr )
        {
            if( shader->compile( m_pD3DDevice ) == false )
            {
                LOGGER_ERROR( "DX9RenderSystem::createVertexShader invalid compile shader %s"
                    , _name.c_str()
                );

                return nullptr;
            }
        }
        else
        {
            m_deferredCompileVertexShaders.emplace_back( shader );
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr DX9RenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount )
    {
        (void)_samplerCount;

        DX9RenderProgramPtr program = m_factoryRenderProgram->createObject();

        if( program == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::createProgram invalid create program %s"
                , _name.c_str()
            );

            return nullptr;
        }

        if( program->initialize( _name, _vertex, _fragment, _vertexAttribute ) == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::createProgram invalid initialize program %s"
                , _name.c_str()
            );

            return nullptr;
        }

        if( m_pD3DDevice != nullptr )
        {
            if( program->compile( m_pD3DDevice ) == false )
            {
                LOGGER_ERROR( "DX9RenderSystem::createProgram invalid compile program %s"
                    , _name.c_str()
                );

                return nullptr;
            }
        }
        else
        {
            m_deferredCompilePrograms.emplace_back( program );
        }

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        if( _program != nullptr )
        {
            DX9RenderProgramPtr dx9_program = stdex::intrusive_static_cast<DX9RenderProgramPtr>(_program);

            if( dx9_program->enable( m_pD3DDevice ) == false )
            {
                return;
            }
        }
        else
        {
            DXCALL( m_pD3DDevice, SetVertexShader, (nullptr) );

            DXCALL( m_pD3DDevice, SetPixelShader, (nullptr) );
        }
        //None
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        DX9RenderProgramPtr dx9_program = stdex::intrusive_static_cast<DX9RenderProgramPtr>(_program);

        dx9_program->bindMatrix( m_pD3DDevice, m_worldMatrix, m_modelViewMatrix, m_projectionMatrix, m_totalWVPInvMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr DX9RenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        DX9RenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject();

        if( variable == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::createProgramVariable invalid create program variable"
            );

            return nullptr;
        }

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::createProgramVariable invalid initialize program variable"
            );

            return nullptr;
        }

        return variable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::applyProgramVariable( const RenderProgramVariableInterfacePtr & _variable, const RenderProgramInterfacePtr & _program )
    {
        DX9RenderProgramVariablePtr dx9_variable = stdex::intrusive_static_cast<DX9RenderProgramVariablePtr>(_variable);

        bool successful = dx9_variable->apply( m_pD3DDevice, _program );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setVSync( bool _vSync )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::setVSync device not created"
            );

            return;
        }

        m_waitForVSync = _vSync;

        this->updateVSyncDPP_();

        if( this->restore_() == false )
        {
            LOGGER_ERROR( "DX9RenderSystem::setVSync: Graphics change mode failed"
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
    void DX9RenderSystem::clear( uint8_t _r, uint8_t _g, uint8_t _b )
    {
        if( m_pD3DDevice == nullptr )
        {
            LOGGER_ERROR( "DX9RenderSystem::clear_ device not created"
            );

            return;
        }

        DXCALL( m_pD3DDevice, Clear, (0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( _r, _g, _b ), 0.f, 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderImagePtr DX9RenderSystem::createDX9RenderImage_( LPDIRECT3DTEXTURE9 _pD3DTexture, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, uint32_t _hwDepth, PixelFormat _hwPixelFormat )
    {
        m_textureCount++;

        DX9RenderImagePtr dx9RenderImage = m_factoryDX9Image->createObject();

        dx9RenderImage->initialize( m_pD3DDevice, _pD3DTexture, _mode, _mipmaps, _hwWidth, _hwHeight, _hwChannels, _hwDepth, _hwPixelFormat );

        m_renderResourceHandlers.push_back( dx9RenderImage.get() );

        uint32_t memoryUse = Helper::getTextureMemorySize( _hwWidth, _hwHeight, _hwChannels, _hwDepth, _hwPixelFormat );

        m_textureMemoryUse += memoryUse;

        return dx9RenderImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9RenderImage_( DX9RenderImage * _image )
    {
        (void)_image;

        m_textureCount--;

        uint32_t hwWidth = _image->getHWWidth();
        uint32_t hwHeight = _image->getHWHeight();
        uint32_t hwChannels = _image->getHWChannels();
        PixelFormat hwPixelFormat = _image->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwChannels, 1, hwPixelFormat );

        m_textureMemoryUse -= memoryUse;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9RenderImageTarget_( DX9RenderImageTarget * _imageTarget )
    {
        _imageTarget->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9RenderTargetTexture_( DX9RenderTargetTexture * _targetTexture )
    {
        _targetTexture->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9RenderTargetOffscreen_( DX9RenderTargetOffscreen * _targetOffscreen )
    {
        _targetOffscreen->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::updateWVPInvMatrix_()
    {
        mt::mat4f totalWVPMatrix = m_worldMatrix * m_modelViewMatrix * m_projectionMatrix;

        mt::transpose_m4( m_totalWVPInvMatrix, totalWVPMatrix );
    }
}

