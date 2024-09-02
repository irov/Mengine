#include "DX9RenderSystem.h"

#include "Interface/RenderServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"

#include "DX9RenderEnum.h"
#include "DX9RenderErrorHelper.h"

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

#include "DX9RenderImageLockedFactoryStorage.h"

#include "Kernel/StatisticHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionStatistic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"
#include "Config/StdMath.h"

#include "math/uv4.h"

#include "Config/Algorithm.h"

#ifndef MENGINE_D3D9_DLL_NAME
#define MENGINE_D3D9_DLL_NAME "d3d9.dll"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Mengine::DX9RenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IDirect3D9 * (WINAPI * PDIRECT3DCREATE9)(UINT);
    //////////////////////////////////////////////////////////////////////////
    DX9RenderSystem::DX9RenderSystem()
        : m_pD3D( nullptr )
        , m_pD3DDevice( nullptr )
        , m_d3dpp( nullptr )
        , m_fullscreen( true )
        , m_depth( false )
        , m_adapterToUse( D3DADAPTER_DEFAULT )
        , m_deviceType( D3DDEVTYPE_HAL )
        , m_vertexBufferEnable( false )
        , m_indexBufferEnable( false )
        , m_vertexAttributeEnable( false )
        , m_vertexShaderEnable( false )
        , m_fragmentShaderEnable( false )
        , m_waitForVSync( false )
        , m_lostDevice( false )
        , m_supportA8( false )
        , m_supportL8( false )
        , m_supportR8G8B8( false )
        , m_supportNonPow2( false )
    {
        mt::ident_m4( &m_projectionMatrix );
        mt::ident_m4( &m_modelViewMatrix );
        mt::ident_m4( &m_worldMatrix );
        mt::ident_m4( &m_totalWVPInvMatrix );

        Algorithm::fill_n( m_textureEnable, MENGINE_MAX_TEXTURE_STAGES, false );
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
        return m_renderSystemName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::_initializeService()
    {
        const Char * Render_D3D9_DLL = CONFIG_VALUE( "DirectX9", "D3D9_DLL", MENGINE_D3D9_DLL_NAME );

        DynamicLibraryInterfacePtr d3d9Library = PLATFORM_SERVICE()
            ->loadDynamicLibrary( Render_D3D9_DLL, MENGINE_DOCUMENT_FACTORABLE );

        if( d3d9Library == nullptr )
        {
            LOGGER_ERROR( "failed to load d3d9 dll '%s'"
                , Render_D3D9_DLL
            );

            return false;
        }

        m_d3d9Library = d3d9Library;

        PDIRECT3DCREATE9 pDirect3DCreate9 = (PDIRECT3DCREATE9)d3d9Library->getSymbol( "Direct3DCreate9" );

        if( pDirect3DCreate9 == nullptr )
        {
            LOGGER_ERROR( "failed to get 'Direct3DCreate9' proc address" );

            return false;
        }

        IDirect3D9 * pD3D = (*pDirect3DCreate9)(D3D_SDK_VERSION);

        if( pD3D == nullptr )
        {
            LOGGER_ERROR( "can't create D3D interface" );

            return false;
        }

        m_pD3D = pD3D;

        m_adapterToUse = D3DADAPTER_DEFAULT;
        m_deviceType = D3DDEVTYPE_HAL;

        UINT AdapterCount = m_pD3D->GetAdapterCount();
        for( UINT Adapter = 0; Adapter != AdapterCount; ++Adapter )
        {
            D3DADAPTER_IDENTIFIER9 AdapterId;
            if( m_pD3D->GetAdapterIdentifier( Adapter, 0, &AdapterId ) != D3D_OK )
            {
                continue;
            }

            if( StdString::strstr( AdapterId.Description, "PerfHUD" ) != 0 )
            {
                m_adapterToUse = Adapter;
                m_deviceType = D3DDEVTYPE_REF;
                break;
            }
        }

        // Set up Windowed presentation parameters
        D3DDISPLAYMODE Mode;
        MENGINE_IF_DXCALL( m_pD3D, GetAdapterDisplayMode, (m_adapterToUse, &Mode) )
        {
            LOGGER_ERROR( "can't determine desktop video mode" );

            return false;
        }

        m_displayMode = Mode;

        // Get adapter info
        D3DADAPTER_IDENTIFIER9 AdapterId;
        MENGINE_IF_DXCALL( m_pD3D, GetAdapterIdentifier, (m_adapterToUse, 0, &AdapterId) )
        {
            LOGGER_ERROR( "can't determine adapter identifier" );

            return false;
        }

        LOGGER_MESSAGE( "D3D Adapter Driver: %s", AdapterId.Driver );
        LOGGER_MESSAGE( "D3D Adapter Description: %s", AdapterId.Description );
        LOGGER_MESSAGE( "D3D Adapter DeviceName: %s", AdapterId.DeviceName );

        LOGGER_MESSAGE( "D3D Adapter Version: %hu.%hu.%hu.%hu"
            , HIWORD( AdapterId.DriverVersion.HighPart )
            , LOWORD( AdapterId.DriverVersion.HighPart )
            , HIWORD( AdapterId.DriverVersion.LowPart )
            , LOWORD( AdapterId.DriverVersion.LowPart )
        );

        LOGGER_INFO( "render", "D3D Adapter VendorId: %lu", AdapterId.VendorId );
        LOGGER_INFO( "render", "D3D Adapter DeviceId: %lu", AdapterId.DeviceId );
        LOGGER_INFO( "render", "D3D Adapter SubSysId: %lu", AdapterId.SubSysId );
        LOGGER_INFO( "render", "D3D Adapter Revision: %lu", AdapterId.Revision );

        m_renderSystemName = STRINGIZE_STRING_LOCAL( "DX9" );

        m_factoryRenderVertexAttribute = Helper::makeFactoryPoolWithListener<DX9RenderVertexAttribute, 8>( this, &DX9RenderSystem::onDestroyDX9VertexAttribute_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderVertexShader = Helper::makeFactoryPoolWithListener<DX9RenderVertexShader, 16>( this, &DX9RenderSystem::onDestroyDX9VertexShader_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderFragmentShader = Helper::makeFactoryPoolWithListener<DX9RenderFragmentShader, 16>( this, &DX9RenderSystem::onDestroyDX9FragmentShader_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgram = Helper::makeFactoryPoolWithListener<DX9RenderProgram, 16>( this, &DX9RenderSystem::onDestroyDX9Program_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgramVariable = Helper::makeFactoryPool<DX9RenderProgramVariable, 64>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryVertexBuffer = Helper::makeFactoryPoolWithListener<DX9RenderVertexBuffer, 8>( this, &DX9RenderSystem::onDestroyDX9VertexBuffer_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryIndexBuffer = Helper::makeFactoryPoolWithListener<DX9RenderIndexBuffer, 8>( this, &DX9RenderSystem::onDestroyDX9IndexBuffer_, MENGINE_DOCUMENT_FACTORABLE );

        m_factoryRenderImage = Helper::makeFactoryPoolWithListener<DX9RenderImage, 128>( this, &DX9RenderSystem::onDestroyDX9RenderImage_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderImageTarget = Helper::makeFactoryPoolWithListener<DX9RenderImageTarget, 16>( this, &DX9RenderSystem::onDestroyDX9RenderImageTarget_, MENGINE_DOCUMENT_FACTORABLE );

        m_factoryRenderTargetTexture = Helper::makeFactoryPoolWithListener<DX9RenderTargetTexture, 16>( this, &DX9RenderSystem::onDestroyDX9RenderTargetTexture_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderTargetOffscreen = Helper::makeFactoryPoolWithListener<DX9RenderTargetOffscreen, 16>( this, &DX9RenderSystem::onDestroyDX9RenderTargetOffscreen_, MENGINE_DOCUMENT_FACTORABLE );

        DX9RenderImageLockedFactoryStorage::initialize( Helper::makeFactoryPool<DX9RenderImageLocked, 8>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::_finalizeService()
    {
        m_deferredCompilePrograms.clear();

        this->release_();

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_renderResourceHandlers );

        m_renderResourceHandlers.clear();

        MENGINE_ASSERTION_STATISTIC_EMPTY( STATISTIC_RENDER_TEXTURE_COUNT );
        MENGINE_ASSERTION_STATISTIC_EMPTY( STATISTIC_RENDER_INDEX_BUFFER_COUNT );
        MENGINE_ASSERTION_STATISTIC_EMPTY( STATISTIC_RENDER_VERTEX_BUFFER_COUNT );

        m_d3d9Library = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexAttribute );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderVertexShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderFragmentShader );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgram );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderProgramVariable );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryVertexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryIndexBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderImage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderImageTarget );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderTargetTexture );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderTargetOffscreen );

        m_factoryRenderVertexAttribute = nullptr;
        m_factoryRenderVertexShader = nullptr;
        m_factoryRenderFragmentShader = nullptr;
        m_factoryRenderProgram = nullptr;
        m_factoryRenderProgramVariable = nullptr;
        m_factoryVertexBuffer = nullptr;
        m_factoryIndexBuffer = nullptr;
        m_factoryRenderImage = nullptr;
        m_factoryRenderImageTarget = nullptr;
        m_factoryRenderTargetTexture = nullptr;
        m_factoryRenderTargetOffscreen = nullptr;

        DX9RenderImageLockedFactoryStorage::finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    D3DMULTISAMPLE_TYPE DX9RenderSystem::findMatchingMultiSampleType_( uint32_t _MultiSampleCount )
    {
        D3DMULTISAMPLE_TYPE MultiSampleType = D3DMULTISAMPLE_NONE;
        for( uint32_t MultiSampleIndex = _MultiSampleCount; MultiSampleIndex != 0; --MultiSampleIndex )
        {
            D3DMULTISAMPLE_TYPE testMultiSampleType = Helper::getMultiSampleType( MultiSampleIndex );

            HRESULT hr_checkDeviceMultiSampleType = m_pD3D->CheckDeviceMultiSampleType(
                m_adapterToUse, m_deviceType, m_displayMode.Format,
                TRUE, testMultiSampleType, NULL
            );

            if( FAILED( hr_checkDeviceMultiSampleType ) )
            {
                LOGGER_ASSERTION( "can't support multi sample count '%u' error [%ld]"
                    , testMultiSampleType
                    , hr_checkDeviceMultiSampleType
                );

                continue;
            }

            MultiSampleType = testMultiSampleType;
            break;
        }

        return MultiSampleType;
    }
    //////////////////////////////////////////////////////////////////////////
    D3DFORMAT DX9RenderSystem::findMatchingZFormat_( const D3DCAPS9 & _d3dCaps, D3DFORMAT _backBufferFormat )
    {
        const D3DFORMAT DepthFormats[] = {D3DFMT_D32
            , D3DFMT_D24S8
            , D3DFMT_D24X4S4
            , D3DFMT_D24X8
            , D3DFMT_D16
            , D3DFMT_D15S1
            , (D3DFORMAT)0
        };

        const D3DFORMAT * pFormatList = DepthFormats;

        while( *pFormatList )
        {
            if( SUCCEEDED( m_pD3D->CheckDeviceFormat( _d3dCaps.AdapterOrdinal, _d3dCaps.DeviceType, _backBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, *pFormatList ) ) )
            {
                if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( _d3dCaps.AdapterOrdinal, _d3dCaps.DeviceType, _backBufferFormat, _backBufferFormat, *pFormatList ) ) )
                {
                    break;
                }
            }

            ++pFormatList;
        }

        return *pFormatList;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::createRenderWindow( const RenderWindowDesc * _windowDesc )
    {
        m_windowResolution = _windowDesc->resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_fullscreen = _windowDesc->fullscreen;
        m_depth = _windowDesc->depth;
        m_waitForVSync = _windowDesc->waitForVSync;
        
        D3DCAPS9 d3dCaps;
        MENGINE_IF_DXCALL( m_pD3D, GetDeviceCaps, (m_adapterToUse, m_deviceType, &d3dCaps) )
        {
            return false;
        }

        D3DMULTISAMPLE_TYPE multiSampleType = this->findMatchingMultiSampleType_( _windowDesc->MultiSampleCount );

        ZeroMemory( &m_d3dppW, sizeof( m_d3dppW ) );

        m_d3dppW.BackBufferWidth = m_windowResolution.getWidth();
        m_d3dppW.BackBufferHeight = m_windowResolution.getHeight();
        m_d3dppW.BackBufferFormat = m_displayMode.Format;
        m_d3dppW.BackBufferCount = 1;

        m_d3dppW.MultiSampleType = D3DMULTISAMPLE_NONE;
        m_d3dppW.MultiSampleQuality = 0;

        m_d3dppW.SwapEffect = D3DSWAPEFFECT_DISCARD;

        Win32PlatformServiceExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getUnknown();

        HWND windowHandle = win32Platform->getWindowHandle();

        m_d3dppW.hDeviceWindow = windowHandle;
        m_d3dppW.Windowed = TRUE;

        if( m_depth == true )
        {
            D3DFORMAT AutoDepthStencilFormat = this->findMatchingZFormat_( d3dCaps, m_d3dppW.BackBufferFormat );

            m_d3dppW.EnableAutoDepthStencil = TRUE;
            m_d3dppW.AutoDepthStencilFormat = AutoDepthStencilFormat;
        }
        else
        {
            m_d3dppW.EnableAutoDepthStencil = FALSE;
            m_d3dppW.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
        }

        m_d3dppW.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

        ZeroMemory( &m_d3dppFS, sizeof( m_d3dppFS ) );

        m_d3dppFS.MultiSampleType = multiSampleType;
        m_d3dppFS.MultiSampleQuality = 0;
        m_d3dppFS.Windowed = FALSE;

        m_d3dppFS.BackBufferWidth = m_windowResolution.getWidth();
        m_d3dppFS.BackBufferHeight = m_windowResolution.getHeight();
        m_d3dppFS.BackBufferCount = 1;

        m_d3dppFS.hDeviceWindow = windowHandle;
        m_d3dppFS.SwapEffect = D3DSWAPEFFECT_DISCARD;
        m_d3dppFS.BackBufferFormat = m_displayMode.Format;

        if( m_depth == true )
        {
            D3DFORMAT AutoDepthStencilFormat = this->findMatchingZFormat_( d3dCaps, m_d3dppFS.BackBufferFormat );

            m_d3dppFS.EnableAutoDepthStencil = TRUE;
            m_d3dppFS.AutoDepthStencilFormat = AutoDepthStencilFormat;
        }
        else
        {
            m_d3dppFS.EnableAutoDepthStencil = FALSE;
            m_d3dppFS.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
        }

        m_d3dppFS.FullScreen_RefreshRateInHz = m_displayMode.RefreshRate;

        LOGGER_INFO( "render", "fullscreen refresh rate [%u]"
            , m_d3dppFS.FullScreen_RefreshRateInHz
        );

        this->updateVSyncDPP_();

        m_d3dpp = m_fullscreen == true ? &m_d3dppFS : &m_d3dppW;

        // Create D3D Device

        LOGGER_INFO( "render", "vertex shader version [%lu] [%s]"
            , d3dCaps.VertexShaderVersion
            , d3dCaps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) ? "true" : "false"
        );

        LOGGER_INFO( "render", "pixel shader version [%lu] [%s] [%s]"
            , d3dCaps.PixelShaderVersion
            , d3dCaps.PixelShaderVersion < D3DPS_VERSION( 1, 1 ) ? "true" : "false"
            , d3dCaps.PixelShaderVersion >= D3DPS_VERSION( 2, 0 ) ? "true" : "false"
        );

        HRESULT hr;

        if( (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 || d3dCaps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        {
            LOGGER_ASSERTION( "can't support D3DCREATE_HARDWARE_VERTEXPROCESSING try to create D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE" );

            hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, windowHandle
                , D3DCREATE_SOFTWARE_VERTEXPROCESSING
                , m_d3dpp, &m_pD3DDevice );
        }
        else
        {
            DWORD device_flags = D3DCREATE_FPU_PRESERVE;

            if( d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
            {
                device_flags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
            }
            else
            {
                LOGGER_ASSERTION( "force set D3DCREATE_SOFTWARE_VERTEXPROCESSING" );

                device_flags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            }

            hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, windowHandle
                , device_flags
                , m_d3dpp, &m_pD3DDevice );

            if( FAILED( hr ) )
            {
                const Char * message = Helper::getDX9ErrorMessage( hr );

                LOGGER_ASSERTION( "can't create D3D device D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE error: %s (hr:%x) Try another"
                    , message
                    , (uint32_t)hr
                );

                ::Sleep( 100 );

                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, windowHandle
                    , device_flags
                    , m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                const Char * message = Helper::getDX9ErrorMessage( hr );

                LOGGER_ASSERTION( "can't create D3D device D3DCREATE_HARDWARE_VERTEXPROCESSING error: %s (hr:%x) Try another"
                    , message
                    , (uint32_t)hr
                );

                ::Sleep( 100 );

                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, windowHandle
                    , device_flags
                    , m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                const Char * message = Helper::getDX9ErrorMessage( hr );

                LOGGER_ASSERTION( "can't create D3D device D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE error: %s (hr:%x) Try another"
                    , message
                    , (uint32_t)hr
                );

                ::Sleep( 100 );

                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, windowHandle
                    , D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE
                    , m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                const Char * message = Helper::getDX9ErrorMessage( hr );

                LOGGER_ASSERTION( "can't create D3D device D3DCREATE_MIXED_VERTEXPROCESSING error: %s (hr:%x) Try another"
                    , message
                    , (uint32_t)hr
                );

                ::Sleep( 100 );

                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, windowHandle
                    , D3DCREATE_MIXED_VERTEXPROCESSING
                    , m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                const Char * message = Helper::getDX9ErrorMessage( hr );

                LOGGER_ASSERTION( "can't create D3D device D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE error: %s (hr:%x) Try another"
                    , message
                    , (uint32_t)hr
                );

                ::Sleep( 100 );

                hr = m_pD3D->CreateDevice( m_adapterToUse, m_deviceType, windowHandle
                    , D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE
                    , m_d3dpp, &m_pD3DDevice );
            }

            if( FAILED( hr ) )
            {
                const Char * message = Helper::getDX9ErrorMessage( hr );

                LOGGER_ASSERTION( "can't create D3D device D3DDEVTYPE_REF | D3DCREATE_SOFTWARE_VERTEXPROCESSING error: %s (hr:%x) Try another"
                    , message
                    , (uint32_t)hr
                );

                ::Sleep( 100 );

                hr = m_pD3D->CreateDevice( m_adapterToUse, D3DDEVTYPE_REF, windowHandle
                    , D3DCREATE_SOFTWARE_VERTEXPROCESSING
                    , m_d3dpp, &m_pD3DDevice );
            }
        }

        if( FAILED( hr ) )
        {
            const Char * message = Helper::getDX9ErrorMessage( hr );

            LOGGER_ERROR( "can't create D3D device error: %s (hr:%x, hwnd:%p) BackBuffer Size %u:%u Format %u"
                , message
                , (uint32_t)hr
                , (void *)windowHandle
                , m_d3dpp->BackBufferWidth
                , m_d3dpp->BackBufferHeight
                , m_d3dpp->BackBufferFormat
            );

            return false;
        }

        //Get Devivce Caps after create device
        MENGINE_DXCALL( m_pD3DDevice, GetDeviceCaps, (&m_d3dCaps) );

        LOGGER_INFO( "render", "window resolution [%ux%u] format '%s'"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
            , Helper::getD3DFormatName( m_displayMode.Format )
        );

        for( const DX9RenderProgramPtr & program : m_deferredCompilePrograms )
        {
            if( program->compile( m_pD3DDevice ) == false )
            {
                return false;
            }
        }

        m_deferredCompilePrograms.clear();

        m_supportA8 = this->supportTextureFormat( PF_A8 );
        m_supportL8 = this->supportTextureFormat( PF_L8 );
        m_supportR8G8B8 = this->supportTextureFormat( PF_R8G8B8 );
        m_supportNonPow2 = this->supportTextureNonPow2( m_d3dCaps );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setProjectionMatrix( const mt::mat4f & _projectionMatrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        float DirectX9_PerfectPixelOffsetX = CONFIG_VALUE( "DirectX9", "PerfectPixelOffsetX", -0.5f );
        float DirectX9_PerfectPixelOffsetY = CONFIG_VALUE( "DirectX9", "PerfectPixelOffsetY", 0.f );

        float perfect_x = DirectX9_PerfectPixelOffsetX / (m_windowViewport.end.x - m_windowViewport.begin.x);
        float perfect_y = DirectX9_PerfectPixelOffsetY / (m_windowViewport.end.y - m_windowViewport.begin.y);

        mt::mat4f vmperfect;
        mt::make_translation_m4( &vmperfect, perfect_x, perfect_y, 0.f );

        mt::mul_m4_m4( &m_projectionMatrix, _projectionMatrix, vmperfect );

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setViewMatrix( const mt::mat4f & _modelViewMatrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        m_modelViewMatrix = _modelViewMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setWorldMatrix( const mt::mat4f & _worldMatrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        m_worldMatrix = _worldMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX9RenderSystem::updateImagePixelFormat_( EPixelFormat _pixelFormat ) const
    {
        switch( _pixelFormat )
        {
        case PF_A8:
            {
                if( m_supportA8 == true )
                {
                    return PF_A8;
                }
                else
                {
                    return PF_A8R8G8B8;
                }
            }break;
        case PF_R8G8B8:
            {
                if( m_supportR8G8B8 == true )
                {
                    return PF_R8G8B8;
                }
                else
                {
                    return PF_X8R8G8B8;
                }
            }break;
        default:
            {
            }break;
        }

        return _pixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr DX9RenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX9RenderImagePtr renderImage = m_factoryRenderImage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderImage, "invalid create render texture" );

        renderImage->setDirect3DDevice9( m_pD3DDevice );

        EPixelFormat fix_format = this->updateImagePixelFormat_( _format );

        if( renderImage->initialize( _mipmaps, _width, _height, fix_format ) == false )
        {
            LOGGER_ERROR( "can't initialize image [%ux%u] format [%u] (doc %s)"
                , _width
                , _height
                , _format
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        LOGGER_INFO( "render", "texture normal created [%ux%u] format [%u] (doc %s)"
            , renderImage->getHWWidth()
            , renderImage->getHWHeight()
            , renderImage->getHWPixelFormat()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        m_renderResourceHandlers.push_back( renderImage.get() );

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_NEW );
        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_COUNT );

#if defined(MENGINE_STATISTIC_ENABLE)
        uint32_t hwWidth = renderImage->getHWHeight();
        uint32_t hwHeight = renderImage->getHWHeight();
        EPixelFormat hwPixelFormat = renderImage->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_MEMORY_USAGE, memoryUse );
#endif

        return renderImage;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX9RenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX9RenderTargetTexturePtr renderTargetTexture = m_factoryRenderTargetTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetTexture );

        renderTargetTexture->setDirect3DDevice9( m_pD3DDevice );

        EPixelFormat fix_format = this->updateImagePixelFormat_( _format );

        if( renderTargetTexture->initialize( _width, _height, fix_format ) == false )
        {
            LOGGER_ERROR( "can't initialize offscreen target %ux%u format %u (doc %s)"
                , renderTargetTexture->getHWWidth()
                , renderTargetTexture->getHWHeight()
                , renderTargetTexture->getHWPixelFormat()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        m_renderResourceHandlers.push_back( renderTargetTexture.get() );

        LOGGER_INFO( "render", "offscreen target created %ux%u format %u (doc %s)"
            , renderTargetTexture->getHWWidth()
            , renderTargetTexture->getHWHeight()
            , renderTargetTexture->getHWPixelFormat()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_NEW );
        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_COUNT );

#if defined(MENGINE_STATISTIC_ENABLE)
        uint32_t hwWidth = renderTargetTexture->getHWHeight();
        uint32_t hwHeight = renderTargetTexture->getHWHeight();
        EPixelFormat hwPixelFormat = renderTargetTexture->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_MEMORY_USAGE, memoryUse );
#endif

        return renderTargetTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX9RenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX9RenderTargetOffscreenPtr renderTargetOffscreen = m_factoryRenderTargetOffscreen->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetOffscreen );

        renderTargetOffscreen->setDirect3DDevice9( m_pD3DDevice );

        EPixelFormat fix_format = this->updateImagePixelFormat_( _format );

        if( renderTargetOffscreen->initialize( _width, _height, fix_format ) == false )
        {
            LOGGER_ERROR( "can't initialize offscreen target %ux%u format %u (doc %s)"
                , renderTargetOffscreen->getHWWidth()
                , renderTargetOffscreen->getHWHeight()
                , renderTargetOffscreen->getHWPixelFormat()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        m_renderResourceHandlers.push_back( renderTargetOffscreen.get() );

        LOGGER_INFO( "render", "offscreen target created %ux%u format %u (doc %s)"
            , renderTargetOffscreen->getHWWidth()
            , renderTargetOffscreen->getHWHeight()
            , renderTargetOffscreen->getHWPixelFormat()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_NEW );
        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_COUNT );

#if defined(MENGINE_STATISTIC_ENABLE)
        uint32_t hwWidth = renderTargetOffscreen->getHWHeight();
        uint32_t hwHeight = renderTargetOffscreen->getHWHeight();
        EPixelFormat hwPixelFormat = renderTargetOffscreen->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_MEMORY_USAGE, memoryUse );
#endif

        return renderTargetOffscreen;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr DX9RenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX9RenderTargetTexturePtr renderTargetTexture = stdex::intrusive_static_cast<DX9RenderTargetTexturePtr>(_renderTarget);

        DX9RenderImageTargetPtr renderImageTarget = m_factoryRenderImageTarget->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderImageTarget );

        renderImageTarget->initialize( renderTargetTexture );

        LOGGER_INFO( "render", "render image target created" );

        return renderImageTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialStageCacheInterfacePtr DX9RenderSystem::createRenderMaterialStageCache( const RenderMaterialStage * _stage, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _stage );
        MENGINE_UNUSED( _doc );

        //Empty

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::resetDevice_()
    {
        if( m_fullscreen == false )
        {
            D3DDISPLAYMODE Mode;
            MENGINE_IF_DXCALL( m_pD3D, GetAdapterDisplayMode, (m_adapterToUse, &Mode) )
            {
                return false;
            }

            if( Mode.Format == D3DFMT_UNKNOWN )
            {
                LOGGER_ASSERTION( "can't determine desktop video mode D3DFMT_UNKNOWN" );

                return false;
            }

            m_d3dppW.BackBufferFormat = Mode.Format;
        }

        if( this->restore_() == false )
        {
            LOGGER_ASSERTION( "restore failed" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::beginScene()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        HRESULT cooperativeLevel = m_pD3DDevice->TestCooperativeLevel();

        if( cooperativeLevel == D3DERR_DEVICELOST )
        {
            m_lostDevice = true;

            LOGGER_MESSAGE( "device lost [begin scene]" );

            ::Sleep( 200 );

            return false;
        }
        else if( cooperativeLevel == D3DERR_DEVICENOTRESET )
        {
            m_lostDevice = true;

            LOGGER_MESSAGE( "device reset [begin scene]" );

            if( this->resetDevice_() == false )
            {
                return false;
            }
        }
        else if( FAILED( cooperativeLevel ) )
        {
            LOGGER_ASSERTION( "invalid TestCooperativeLevel [%ld]"
                , cooperativeLevel
            );

            if( this->releaseResources_() == false )
            {
                LOGGER_ASSERTION( "release resources" );
            }

            return false;
        }

        m_lostDevice = false;

        MENGINE_IF_DXCALL( m_pD3DDevice, BeginScene, () )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::endScene()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        MENGINE_DXCALL( m_pD3DDevice, EndScene, () );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::swapBuffers()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        HRESULT cooperativeLevel = m_pD3DDevice->TestCooperativeLevel();

        if( cooperativeLevel == D3DERR_DEVICELOST )
        {
            m_lostDevice = true;

            LOGGER_MESSAGE( "device lost [swap buffers]" );

            return;
        }

        if( cooperativeLevel == D3DERR_DEVICENOTRESET )
        {
            m_lostDevice = true;

            LOGGER_MESSAGE( "device reset [swap buffers]" );

            if( this->resetDevice_() == false )
            {
                return;
            }
        }

        HRESULT hr = m_pD3DDevice->Present( NULL, NULL, NULL, NULL );

        if( hr == D3DERR_DEVICELOST )
        {
            m_lostDevice = true;

            LOGGER_ASSERTION( "device lost [present]" );
        }
        else if( FAILED( hr ) )
        {
            LOGGER_ASSERTION( "failed to swap buffers [%ld]"
                , cooperativeLevel
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, double _depth, int32_t _stencil )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

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

        ColorValue_ARGB argb = _color.getAsARGB();

        float d3d_depth = (float)_depth;

        MENGINE_DXCALL( m_pD3DDevice, Clear, (0, NULL, frameBufferFlags, argb, d3d_depth, _stencil) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setScissor( const Viewport & _viewport )
    {
        mt::mat4f pm;
        mt::mul_m4_m4( &pm, m_projectionMatrix, m_modelViewMatrix );

        mt::vec2f b;
        mt::mul_v2_v2_m4( &b, _viewport.begin, pm );

        mt::vec2f e;
        mt::mul_v2_v2_m4( &e, _viewport.end, pm );

        mt::vec2f vs = m_viewport.size();

        float bx = (b.x + 1.f) * 0.5f * vs.x;
        float by = (1.f - (b.y + 1.f) * 0.5f) * vs.y;
        float ex = (e.x + 1.f) * 0.5f * vs.x;
        float ey = (1.f - (e.y + 1.f) * 0.5f) * vs.y;

        bx = MENGINE_MAX( bx, m_viewport.begin.x );
        by = MENGINE_MAX( by, m_viewport.begin.y );
        ex = MENGINE_MIN( ex, m_viewport.end.x );
        ey = MENGINE_MIN( ey, m_viewport.end.y );

        RECT r;
        r.left = (uint32_t)bx;
        r.top = (uint32_t)by;
        r.right = (uint32_t)ex;
        r.bottom = (uint32_t)ey;

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SCISSORTESTENABLE, TRUE) );
        MENGINE_DXCALL( m_pD3DDevice, SetScissorRect, (&r) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::removeScissor()
    {
        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SCISSORTESTENABLE, FALSE) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setViewport( const Viewport & _viewport )
    {
        if( m_viewport.equalViewport( _viewport, 0.5f ) == true )
        {
            return;
        }

        m_viewport = _viewport;

        this->updateViewport_( m_viewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::updateViewport_( const Viewport & _viewport )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DVIEWPORT9 VP;
        VP.X = (DWORD)Math::floorf( _viewport.begin.x + 0.5f );
        VP.Y = (DWORD)Math::floorf( _viewport.begin.y + 0.5f );

        float width = _viewport.getWidth();
        float height = _viewport.getHeight();

        VP.Width = (DWORD)Math::floorf( width + 0.5f );
        VP.Height = (DWORD)Math::floorf( height + 0.5f );

        VP.MinZ = 0.f;
        VP.MaxZ = 1.f;

        MENGINE_IF_DXCALL( m_pD3DDevice, SetViewport, (&VP) )
        {
            LOGGER_ASSERTION( "failed viewport (%lu, %lu, %lu, %lu)"
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
        if( m_windowResolution == _resolution && m_fullscreen == _fullscreen )
        {
            return;
        }

        m_fullscreen = _fullscreen;

        m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_d3dpp = _fullscreen == true ? &m_d3dppFS : &m_d3dppW;
        m_d3dpp->BackBufferWidth = m_windowResolution.getWidth();
        m_d3dpp->BackBufferHeight = m_windowResolution.getHeight();

        //nScreenBPP = _bpp;

        if( this->restore_() == false )
        {
            LOGGER_ASSERTION( "graphics change mode failed" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderSystem::getAvailableTextureMemory() const
    {
        UINT availableTextureMem = m_pD3DDevice->GetAvailableTextureMem();

        return (uint32_t)availableTextureMem;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DDevice9 * DX9RenderSystem::getDirect3DDevice9() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::supportTextureFormat( EPixelFormat _format ) const
    {
        D3DFORMAT dxformat = Helper::toD3DFormat( _format );

        HRESULT hresult = m_pD3D->CheckDeviceFormat( m_adapterToUse, m_deviceType, m_displayMode.Format, 0, D3DRTYPE_TEXTURE, dxformat );

        if( hresult == D3DERR_NOTAVAILABLE )
        {
            return false;
        }

        MENGINE_IF_DXERRORCHECK( CheckDeviceFormat, hresult )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::supportTextureNonPow2( const D3DCAPS9 & _d3dCaps ) const
    {
        if( (_d3dCaps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderSystem::getMaxCombinedTextureImageUnits() const
    {
        DWORD MaxSimultaneousTextures = m_d3dCaps.MaxSimultaneousTextures;

        if( MaxSimultaneousTextures > MENGINE_MAX_TEXTURE_STAGES )
        {
            return MENGINE_MAX_TEXTURE_STAGES;
        }

        return (uint32_t)MaxSimultaneousTextures;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDeviceLostPrepare()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::onDeviceLostRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onWindowChangeFullscreenPrepare( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        //Empty

        return true;
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
    void DX9RenderSystem::setTextureMatrix( uint32_t _stage, const mt::mat4f & _matrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ASSERTION( "no support stage %u (max %u)"
                , _stage
                , MaxCombinedTextureImageUnits
            );

            return;
        }

        DWORD state = D3DTTFF_COUNT2;
        MENGINE_IF_DXCALL( m_pD3DDevice, SetTextureStageState, (_stage, D3DTSS_TEXTURETRANSFORMFLAGS, state) )
        {
            return;
        }

        D3DTRANSFORMSTATETYPE level = static_cast<D3DTRANSFORMSTATETYPE>(static_cast<DWORD>(D3DTS_TEXTURE0) + _stage);
        MENGINE_IF_DXCALL( m_pD3DDevice, SetTransform, (level, (const D3DMATRIX *)_matrix.buff()) )
        {
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::releaseResources_()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        if( m_vertexBufferEnable == true )
        {
            m_vertexBufferEnable = false;

            MENGINE_IF_DXCALL( m_pD3DDevice, SetStreamSource, (0, NULL, 0, 0) )
            {
                LOGGER_ASSERTION( "stream source not reset" );
            }
        }

        if( m_indexBufferEnable == true )
        {
            m_indexBufferEnable = false;

            MENGINE_IF_DXCALL( m_pD3DDevice, SetIndices, (NULL) )
            {
                LOGGER_ASSERTION( "indices not reset" );
            }
        }

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            if( m_textureEnable[i] == false )
            {
                continue;
            }

            m_textureEnable[i] = false;

            MENGINE_IF_DXCALL( m_pD3DDevice, SetTexture, (i, NULL) )
            {
                LOGGER_ASSERTION( "texture [%u] not reset"
                    , i
                );
            }
        }

        if( m_vertexShaderEnable == true )
        {
            m_vertexShaderEnable = false;

            MENGINE_DXCALL( m_pD3DDevice, SetVertexShader, (NULL) );
        }

        if( m_fragmentShaderEnable == true )
        {
            m_fragmentShaderEnable = false;

            MENGINE_DXCALL( m_pD3DDevice, SetPixelShader, (NULL) );
        }

        if( m_vertexAttributeEnable == true )
        {
            m_vertexAttributeEnable = false;

            MENGINE_DXCALL( m_pD3DDevice, SetVertexDeclaration, (NULL) );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::restore_()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        if( this->releaseResources_() == false )
        {
            LOGGER_ASSERTION( "release resources" );

            return false;
        }

        RENDER_SERVICE()
            ->onDeviceLostPrepare();

        for( DX9RenderResourceHandler * handler : m_renderResourceHandlers )
        {
            handler->onRenderReset();
        }

        HRESULT cooperativeLevel = m_pD3DDevice->Reset( m_d3dpp );

        if( cooperativeLevel == D3DERR_DEVICELOST )
        {
            LOGGER_MESSAGE( "device lost [reset]" );

            ::Sleep( 200 );

            m_lostDevice = true;

            return true;
        }
        else if( cooperativeLevel == D3DERR_INVALIDCALL )
        {
            const Char * message = Helper::getDX9ErrorMessage( cooperativeLevel );

            MENGINE_ERROR_FATAL( "failed to reset device: %s (hr:%x)"
                , message
                , (uint32_t)cooperativeLevel
            );

            return false;
        }
        else if( FAILED( cooperativeLevel ) == true )
        {
            const Char * message = Helper::getDX9ErrorMessage( cooperativeLevel );

            LOGGER_ASSERTION( "failed to reset device: %s (hr:%x)"
                , message
                , (uint32_t)cooperativeLevel
            );

            return false;
        }

        m_lostDevice = false;

        for( DX9RenderResourceHandler * handler : m_renderResourceHandlers )
        {
            if( handler->onRenderRestore() == false )
            {
                return false;
            }
        }

        if( RENDER_SERVICE()
            ->onDeviceLostRestore() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::release_()
    {
        if( m_pD3DDevice != nullptr )
        {
            if( this->releaseResources_() == false )
            {
                LOGGER_ASSERTION( "invalid release resource" );

                return;
            }

            ULONG ref = m_pD3DDevice->Release();
            MENGINE_UNUSED( ref );

            MENGINE_ASSERTION( ref == 0, "D3DDevice has refcount [%lu]", ref );

            m_pD3DDevice = nullptr;
        }

        if( m_pD3D != nullptr )
        {
            ULONG ref = m_pD3D->Release();
            MENGINE_UNUSED( ref );

            MENGINE_ASSERTION( ref == 0, "D3D has refcount [%lu]", ref );

            m_pD3D = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr DX9RenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        DX9RenderVertexBufferPtr vertexBuffer = m_factoryVertexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexBuffer );

        vertexBuffer->setDirect3DDevice9( m_pD3DDevice );

        if( vertexBuffer->initialize( _vertexSize, _bufferType ) == false )
        {
            LOGGER_ERROR( "invalid initialize vertex buffer [%u] type [%u]"
                , _vertexSize
                , _bufferType
            );

            return nullptr;
        }

        m_renderResourceHandlers.push_back( vertexBuffer.get() );

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_VERTEX_BUFFER_COUNT );

        return vertexBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        if( _vertexBuffer == nullptr )
        {
            if( m_vertexBufferEnable == false )
            {
                return true;
            }

            m_vertexBufferEnable = false;

            MENGINE_IF_DXCALL( m_pD3DDevice, SetStreamSource, (0, NULL, 0, 0) )
            {
                return false;
            }

            return true;
        }

        DX9RenderVertexBuffer * dx9VertexBuffer = _vertexBuffer.getT<DX9RenderVertexBuffer *>();
        dx9VertexBuffer->enable();

        m_vertexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr DX9RenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        DX9RenderIndexBufferPtr indexBuffer = m_factoryIndexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( indexBuffer );

        indexBuffer->setDirect3DDevice9( m_pD3DDevice );

        if( indexBuffer->initialize( _indexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        m_renderResourceHandlers.push_back( indexBuffer.get() );

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_INDEX_BUFFER_COUNT );

        return indexBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        if( _indexBuffer == nullptr )
        {
            if( m_indexBufferEnable == false )
            {
                return true;
            }

            m_indexBufferEnable = false;

            MENGINE_IF_DXCALL( m_pD3DDevice, SetIndices, (NULL) )
            {
                return false;
            }

            return true;
        }

        DX9RenderIndexBuffer * dx9IndexBuffer = _indexBuffer.getT<DX9RenderIndexBuffer *>();
        dx9IndexBuffer->enable();

        m_indexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc )
    {
        MENGINE_UNUSED( _stageCache );

#if defined(MENGINE_DEBUG)
        DWORD pNumPasses;
        MENGINE_DXCALL( m_pD3DDevice, ValidateDevice, (&pNumPasses) );
#endif

        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DPRIMITIVETYPE primitiveType = Helper::toD3DPrimitiveType( _desc.primitiveType );

        UINT primitiveCount = Helper::getPrimitiveCount( _desc.primitiveType, _desc.indexCount );

        MENGINE_DXCALL( m_pD3DDevice, DrawIndexedPrimitive
            , (primitiveType, _desc.baseVertexIndex, _desc.minIndex, _desc.vertexCount, _desc.startIndex, primitiveCount)
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _image )
    {
        MENGINE_UNUSED( _program );

        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ASSERTION( "no support stage %u (max %u)"
                , _stage
                , MaxCombinedTextureImageUnits
            );

            return;
        }

        if( _image != nullptr )
        {
            DX9RenderImageExtensionInterface * extension = _image->getUnknown();

            extension->bind( _stage );

            m_textureEnable[_stage] = true;
        }
        else
        {
            MENGINE_DXCALL( m_pD3DDevice, SetTexture, (_stage, NULL) );

            m_textureEnable[_stage] = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DWORD srcBlendFactor = Helper::toD3DBlendFactor( _src );
        DWORD dstBlendFactor = Helper::toD3DBlendFactor( _dst );
        DWORD blendOp = Helper::toD3DBlendOp( _op );

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SRCBLEND, srcBlendFactor) );
        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_DESTBLEND, dstBlendFactor) );
        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_BLENDOP, blendOp) );

        if( _separate == false )
        {
            MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SEPARATEALPHABLENDENABLE, FALSE) );
        }
        else
        {
            MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SEPARATEALPHABLENDENABLE, TRUE) );

            DWORD srcSeparateBlendFactor = Helper::toD3DBlendFactor( _separateSrc );
            DWORD dstSeparateBlendFactor = Helper::toD3DBlendFactor( _separateDst );
            DWORD blendSeparateOp = Helper::toD3DBlendOp( _separateOp );

            MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_SRCBLENDALPHA, srcSeparateBlendFactor) );
            MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_DESTBLENDALPHA, dstSeparateBlendFactor) );
            MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_BLENDOPALPHA, blendSeparateOp) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ASSERTION( "no support stage %u (max %u)"
                , _stage
                , MaxCombinedTextureImageUnits
            );

            return;
        }

        D3DTEXTUREADDRESS adrU = Helper::toD3DTextureAddress( _modeU );
        D3DTEXTUREADDRESS adrV = Helper::toD3DTextureAddress( _modeV );

        MENGINE_DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_ADDRESSU, adrU) );
        MENGINE_DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_ADDRESSV, adrV) );
        MENGINE_DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_BORDERCOLOR, _border) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureFactor( uint32_t _color )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DWORD color = _color;

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_TEXTUREFACTOR, color) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setCullMode( ECullMode _mode )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DCULL mode = Helper::toD3DCullMode( _mode );

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_CULLMODE, mode) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setDepthBufferTestEnable( bool _depthTest )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DZBUFFERTYPE test = _depthTest == true ? D3DZB_TRUE : D3DZB_FALSE;

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZENABLE, test) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DWORD dWrite = _depthWrite == true ? TRUE : FALSE;

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZWRITEENABLE, dWrite) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DCMPFUNC func = Helper::toD3DCmpFunc( _depthFunction );

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZFUNC, func) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setFillMode( EFillMode _mode )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DFILLMODE mode = Helper::toD3DFillMode( _mode );

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_FILLMODE, mode) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

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

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_COLORWRITEENABLE, value) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DWORD alphaBlend = _alphaBlend ? TRUE : FALSE;

        MENGINE_DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ALPHABLENDENABLE, alphaBlend) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ASSERTION( "no support stage %u (max %u)"
                , _stage
                , MaxCombinedTextureImageUnits
            );

            return;
        }

        D3DTEXTUREFILTERTYPE dx_minification = Helper::toD3DTextureFilter( _minification );
        D3DTEXTUREFILTERTYPE dx_mipmap = Helper::toD3DTextureFilter( _mipmap );
        D3DTEXTUREFILTERTYPE dx_magnification = Helper::toD3DTextureFilter( _magnification );

        MENGINE_DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MINFILTER, dx_minification) );
        MENGINE_DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MIPFILTER, dx_mipmap) );
        MENGINE_DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MAGFILTER, dx_magnification) );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr DX9RenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentInterfacePtr & _doc )
    {
        DX9RenderVertexAttributePtr attribute = m_factoryRenderVertexAttribute->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( attribute, "invalid create attribute '%s'"
            , _name.c_str()
        );

        if( attribute->initialize( _name, _elementSize ) == false )
        {
            LOGGER_ERROR( "invalid initialize attribute '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return attribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr DX9RenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        DX9RenderFragmentShaderPtr shader = m_factoryRenderFragmentShader->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "invalid create shader '%s'"
            , _name.c_str()
        );

        if( shader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr DX9RenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        DX9RenderVertexShaderPtr shader = m_factoryRenderVertexShader->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "invalid create shader '%s'"
            , _name.c_str()
        );

        if( shader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "invalid initialize shader '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr DX9RenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _samplerCount );

        DX9RenderProgramPtr program = m_factoryRenderProgram->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( program, "invalid create program '%s'"
            , _name.c_str()
        );

        if( program->initialize( _name, _vertex, _fragment, _vertexAttribute ) == false )
        {
            LOGGER_ERROR( "invalid initialize program '%s'"
                , _name.c_str()
            );

            return nullptr;
        }

        if( m_pD3DDevice != nullptr )
        {
            if( program->compile( m_pD3DDevice ) == false )
            {
                LOGGER_ERROR( "invalid compile program '%s'"
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

            dx9_program->enable( m_pD3DDevice );

            const RenderVertexAttributeInterfacePtr & vertexAttribute = dx9_program->getVertexAttribute();
            const RenderVertexShaderInterfacePtr & vertexShader = dx9_program->getVertexShader();
            const RenderFragmentShaderInterfacePtr & fragmentShader = dx9_program->getFragmentShader();

            bool vertexAttributeEnable = vertexAttribute != nullptr;
            bool vertexShaderEnable = vertexShader != nullptr;
            bool fragmentShaderEnable = fragmentShader != nullptr;

            if( m_vertexAttributeEnable == true && vertexAttributeEnable == false )
            {
                m_vertexAttributeEnable = false;

                MENGINE_DXCALL( m_pD3DDevice, SetVertexDeclaration, (NULL) );
            }

            if( m_vertexShaderEnable == true && vertexShaderEnable == false )
            {
                m_vertexShaderEnable = false;

                MENGINE_DXCALL( m_pD3DDevice, SetVertexShader, (NULL) );
            }

            if( m_fragmentShaderEnable == true && fragmentShaderEnable == false )
            {
                m_fragmentShaderEnable = false;

                MENGINE_DXCALL( m_pD3DDevice, SetPixelShader, (NULL) );
            }
        }
        else
        {
            if( m_vertexShaderEnable == true )
            {
                m_vertexShaderEnable = false;

                MENGINE_DXCALL( m_pD3DDevice, SetVertexShader, (NULL) );
            }

            if( m_fragmentShaderEnable == true )
            {
                m_vertexShaderEnable = false;

                MENGINE_DXCALL( m_pD3DDevice, SetPixelShader, (NULL) );
            }

            if( m_vertexAttributeEnable == true )
            {
                m_vertexShaderEnable = false;

                MENGINE_DXCALL( m_pD3DDevice, SetVertexDeclaration, (NULL) );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        DX9RenderProgramPtr dx9_program = stdex::intrusive_static_cast<DX9RenderProgramPtr>(_program);

        dx9_program->bindMatrix( m_pD3DDevice, m_worldMatrix, m_modelViewMatrix, m_projectionMatrix, m_totalWVPInvMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr DX9RenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc )
    {
        DX9RenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( variable, "invalid create program variable"
        );

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "invalid initialize program variable" );

            return nullptr;
        }

        return variable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderSystem::setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable )
    {
        if( _variable == nullptr )
        {
            return true;
        }

        DX9RenderProgramVariablePtr dx9_variable = stdex::intrusive_static_cast<DX9RenderProgramVariablePtr>(_variable);

        bool successful = dx9_variable->apply( m_pD3DDevice, _program );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::setVSync( bool _vSync )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        if( m_waitForVSync == _vSync )
        {
            return;
        }

        m_waitForVSync = _vSync;

        this->updateVSyncDPP_();

        if( this->restore_() == false )
        {
            LOGGER_ASSERTION( "graphics change mode failed" );
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
    void DX9RenderSystem::onDestroyDX9VertexAttribute_( DX9RenderVertexAttribute * _attribute )
    {
        _attribute->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9VertexShader_( DX9RenderVertexShader * _shader )
    {
        _shader->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9FragmentShader_( DX9RenderFragmentShader * _shader )
    {
        _shader->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9Program_( DX9RenderProgram * _program )
    {
        _program->release();
        _program->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9VertexBuffer_( DX9RenderVertexBuffer * _buffer )
    {
        _buffer->finalize();

        STATISTIC_DEC_INTEGER( STATISTIC_RENDER_VERTEX_BUFFER_COUNT );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9IndexBuffer_( DX9RenderIndexBuffer * _buffer )
    {
        _buffer->finalize();

        STATISTIC_DEC_INTEGER( STATISTIC_RENDER_INDEX_BUFFER_COUNT );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9RenderImage_( DX9RenderImage * _image )
    {
        _image->finalize();

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_FREE );
        STATISTIC_DEC_INTEGER( STATISTIC_RENDER_TEXTURE_COUNT );

#if defined(MENGINE_STATISTIC_ENABLE)
        uint32_t hwWidth = _image->getHWWidth();
        uint32_t hwHeight = _image->getHWHeight();
        EPixelFormat hwPixelFormat = _image->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        STATISTIC_DEL_INTEGER( STATISTIC_RENDER_TEXTURE_MEMORY_USAGE, memoryUse );
#endif
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

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_FREE );
        STATISTIC_DEC_INTEGER( STATISTIC_RENDER_TEXTURE_COUNT );

#if defined(MENGINE_STATISTIC_ENABLE)
        uint32_t hwWidth = _targetTexture->getHWWidth();
        uint32_t hwHeight = _targetTexture->getHWHeight();
        EPixelFormat hwPixelFormat = _targetTexture->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        STATISTIC_DEL_INTEGER( STATISTIC_RENDER_TEXTURE_MEMORY_USAGE, memoryUse );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::onDestroyDX9RenderTargetOffscreen_( DX9RenderTargetOffscreen * _targetOffscreen )
    {
        _targetOffscreen->finalize();

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_FREE );
        STATISTIC_DEC_INTEGER( STATISTIC_RENDER_TEXTURE_COUNT );

#if defined(MENGINE_STATISTIC_ENABLE)
        uint32_t hwWidth = _targetOffscreen->getHWWidth();
        uint32_t hwHeight = _targetOffscreen->getHWHeight();
        EPixelFormat hwPixelFormat = _targetOffscreen->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        STATISTIC_DEL_INTEGER( STATISTIC_RENDER_TEXTURE_MEMORY_USAGE, memoryUse );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderSystem::updateWVPInvMatrix_()
    {
        mt::mat4f totalWVPMatrix = m_worldMatrix * m_modelViewMatrix * m_projectionMatrix;

        mt::transpose_m4_m4( &m_totalWVPInvMatrix, totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
}