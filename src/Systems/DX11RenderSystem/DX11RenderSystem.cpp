#include "DX11RenderSystem.h"

#include "Interface/RenderServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#   include "Interface/Win32PlatformExtensionInterface.h"
#elif defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
#   include "Interface/SDLPlatformServiceExtensionInterface.h"
#endif

#include "DX11RenderEnum.h"
#include "DX11RenderErrorHelper.h"

#include "DX11RenderImage.h"
#include "DX11RenderImageTarget.h"
#include "DX11RenderTargetTexture.h"
#include "DX11RenderTargetOffscreen.h"
#include "DX11RenderMaterialStageCache.h"

#include "DX11RenderVertexShader.h"
#include "DX11RenderFragmentShader.h"
#include "DX11RenderProgram.h"
#include "DX11RenderVertexAttribute.h"
#include "DX11RenderVertexBuffer.h"
#include "DX11RenderIndexBuffer.h"
#include "DX11RenderProgramVariable.h"

#include "DX11RenderImageLockedFactoryStorage.h"

#include "Kernel/PixelFormatHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Error.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

#include "math/uv4.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Mengine::DX11RenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderSystem::DX11RenderSystem()
        : m_windowFullscreen( true )
        , m_windowDepth( false )
        , m_adapterToUse( 0 )
        , m_multiSampleCount( 0 )
        , m_vertexBufferEnable( false )
        , m_indexBufferEnable( false )
        , m_vertexAttributeEnable( false )
        , m_vertexShaderEnable( false )
        , m_fragmentShaderEnable( false )
        , m_frames( 0 )
        , m_textureMemoryUse( 0 )
        , m_textureCount( 0 )
        , m_vertexBufferCount( 0 )
        , m_indexBufferCount( 0 )
        , m_waitForVSync( false )
        , m_invalidateRasterizerState( true )
    {
        mt::ident_m4( &m_projectionMatrix );
        mt::ident_m4( &m_modelViewMatrix );
        mt::ident_m4( &m_worldMatrix );
        mt::ident_m4( &m_totalWVPInvMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderSystem::~DX11RenderSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderPlatform DX11RenderSystem::getRenderPlatformType() const
    {
        return RP_DX11;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX11RenderSystem::getRenderPlatformName() const
    {
        return m_renderSystemName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::_initializeService()
    {
        m_frames = 0;

        HRESULT result;

        // Create a DirectX graphics interface factory.
        // DirectX 11 (for 11.1 or 11.2 need to use other factory)
        IDXGIFactory1 * factory;
        result = CreateDXGIFactory1( __uuidof(IDXGIFactory1), (void **)&factory );

        if( FAILED( result ) )
        {
            return false;
        }

        // Use the factory to create an adapter for the primary graphics interface (video card).
        IDXGIAdapter * adapter;
        IF_DXCALL( factory, EnumAdapters, (m_adapterToUse, &adapter) )
        {
            return false;
        }

        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef MENGINE_DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_9_1
        };

        // Create the Direct3D 11 API device object and a corresponding context.
        ID3D11Device * d3dDevice;
        ID3D11DeviceContext * d3dContext;
        result = D3D11CreateDevice(
            adapter, // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE( featureLevels ),
            D3D11_SDK_VERSION, // UWP apps must set this to D3D11_SDK_VERSION.
            &d3dDevice, // Returns the Direct3D device created.
            nullptr,
            &d3dContext // Returns the device immediate context.
        );

        if( FAILED( result ) )
        {
            return false;
        }

        m_pD3DDevice.Attach( d3dDevice );
        m_pD3DDeviceContext.Attach( d3dContext );

        IDXGIOutput * adapterOutput;
        // Enumerate the primary adapter output (monitor).
        IF_DXCALL( adapter, EnumOutputs, (0, &adapterOutput) )
        {
            return false;
        }

        // Get the number of modes that fit the DXGI_FORMAT_B8G8R8A8_UNORM display format for the adapter output (monitor).
        UINT DisplayModeListNum;
        IF_DXCALL( adapterOutput, GetDisplayModeList, (DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &DisplayModeListNum, nullptr) )
        {
            return false;
        }

        // Create a list to hold all the possible display modes for this monitor/video card combination.
        m_DisplayModeList.resize( DisplayModeListNum );

        IF_DXCALL( adapterOutput, GetDisplayModeList, (DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &DisplayModeListNum, m_DisplayModeList.data()) )
        {
            return false;
        }

        DXRELEASE( adapterOutput );

        DXGI_ADAPTER_DESC AdapterDesc;
        adapter->GetDesc( &AdapterDesc );

        ZeroMemory( &m_D3DRasterizerStateDesc, sizeof( m_D3DRasterizerStateDesc ) );

        m_D3DRasterizerStateDesc.AntialiasedLineEnable = false;
        m_D3DRasterizerStateDesc.CullMode = D3D11_CULL_BACK;
        m_D3DRasterizerStateDesc.CullMode = D3D11_CULL_NONE;

        m_D3DRasterizerStateDesc.DepthBias = 0;
        m_D3DRasterizerStateDesc.DepthBiasClamp = 0.f;
        m_D3DRasterizerStateDesc.DepthClipEnable = TRUE;
        m_D3DRasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
        m_D3DRasterizerStateDesc.FrontCounterClockwise = TRUE;
        //      rasterDesc.FrontCounterClockwise = false;

        m_D3DRasterizerStateDesc.MultisampleEnable = FALSE;
        m_D3DRasterizerStateDesc.ScissorEnable = FALSE;
        m_D3DRasterizerStateDesc.SlopeScaledDepthBias = 0.f;

        m_invalidateRasterizerState = true;

        LOGGER_MESSAGE_RELEASE( "D3D Adapter Description: %ls", AdapterDesc.Description );

        LOGGER_MESSAGE_RELEASE( "D3D Adapter VendorId: %lu", AdapterDesc.VendorId );
        LOGGER_MESSAGE_RELEASE( "D3D Adapter DeviceId: %lu", AdapterDesc.DeviceId );
        LOGGER_MESSAGE_RELEASE( "D3D Adapter SubSysId: %lu", AdapterDesc.SubSysId );
        LOGGER_MESSAGE_RELEASE( "D3D Adapter Revision: %lu", AdapterDesc.Revision );

        m_renderSystemName = STRINGIZE_STRING_LOCAL( "DX11" );

        m_factoryRenderVertexAttribute = Helper::makeFactoryPoolWithListener<DX11RenderVertexAttribute, 8>( this, &DX11RenderSystem::onDestroyVertexAttribute_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderVertexShader = Helper::makeFactoryPoolWithListener<DX11RenderVertexShader, 16>( this, &DX11RenderSystem::onDestroyVertexShader_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderFragmentShader = Helper::makeFactoryPoolWithListener<DX11RenderFragmentShader, 16>( this, &DX11RenderSystem::onDestroyFragmentShader_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgram = Helper::makeFactoryPoolWithListener<DX11RenderProgram, 16>( this, &DX11RenderSystem::onDestroyProgram_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderProgramVariable = Helper::makeFactoryPool<DX11RenderProgramVariable, 64>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryVertexBuffer = Helper::makeFactoryPoolWithListener<DX11RenderVertexBuffer, 8>( this, &DX11RenderSystem::onDestroyVertexBuffer_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryIndexBuffer = Helper::makeFactoryPoolWithListener<DX11RenderIndexBuffer, 8>( this, &DX11RenderSystem::onDestroyIndexBuffer_, MENGINE_DOCUMENT_FACTORABLE );

        m_factoryRenderImage = Helper::makeFactoryPoolWithListener<DX11RenderImage, 128>( this, &DX11RenderSystem::onDestroyRenderImage_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderImageTarget = Helper::makeFactoryPoolWithListener<DX11RenderImageTarget, 16>( this, &DX11RenderSystem::onDestroyRenderImageTarget_, MENGINE_DOCUMENT_FACTORABLE );

        m_factoryRenderTargetTexture = Helper::makeFactoryPoolWithListener<DX11RenderTargetTexture, 16>( this, &DX11RenderSystem::onDestroyRenderTargetTexture_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderTargetOffscreen = Helper::makeFactoryPoolWithListener<DX11RenderTargetOffscreen, 16>( this, &DX11RenderSystem::onDestroyRenderTargetOffscreen_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRenderMaterialStageCache = Helper::makeFactoryPoolWithListener<DX11RenderMaterialStageCache, 16>( this, &DX11RenderSystem::onDestroyRenderMaterialStageCache_, MENGINE_DOCUMENT_FACTORABLE );

        DX11RenderImageLockedFactoryStorage::initialize( Helper::makeFactoryPool<DX11RenderImageLocked, 64>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::_finalizeService()
    {
#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        m_dxgiSwapChain->SetFullscreenState( FALSE, nullptr );
#endif

        m_deferredCompilePrograms.clear();

        if( this->releaseResources_() == false )
        {
            LOGGER_ERROR( "invalid release resource" );

            return;
        }

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_renderResourceHandlers );

        m_renderResourceHandlers.clear();

        MENGINE_ASSERTION_FATAL( m_textureCount == 0 );
        MENGINE_ASSERTION_FATAL( m_vertexBufferCount == 0 );
        MENGINE_ASSERTION_FATAL( m_indexBufferCount == 0 );

        m_dxgiSwapChain = nullptr;
        m_renderTargetView = nullptr;
        m_depthStencilBuffer = nullptr;
        m_depthStencilView = nullptr;

        m_pD3DRasterizerState = nullptr;

        // Flush the immediate context to force cleanup
        m_pD3DDeviceContext->Flush();

        m_pD3DDeviceContext = nullptr;
        m_pD3DDeviceImmediateContext = nullptr;

        ID3D11Debug * D3DDevice;
        m_pD3DDevice->QueryInterface( __uuidof(ID3D11Debug), reinterpret_cast<void **>(&D3DDevice) );
        D3DDevice->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
        D3DDevice->Release();

        m_pD3DDevice = nullptr;

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
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderMaterialStageCache );

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
        m_factoryRenderMaterialStageCache = nullptr;

        DX11RenderImageLockedFactoryStorage::finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::createRenderWindow( const RenderWindowDesc * _windowDesc )
    {
        m_windowResolution = _windowDesc->resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_windowFullscreen = _windowDesc->fullscreen;
        m_windowDepth = _windowDesc->depth;
        m_waitForVSync = _windowDesc->waitForVSync;
        m_multiSampleCount = _windowDesc->MultiSampleCount;

        // Create a DirectX graphics interface factory.
        // DirectX 11 (for 11.1 or 11.2 need to use other factory)
        IDXGIFactory2 * dxgiFactory;
        HRESULT result = CreateDXGIFactory1( __uuidof(IDXGIFactory2), (void **)&dxgiFactory );

        if( FAILED( result ) )
        {
            LOGGER_ERROR( "invalid create DXGI factory: %s [%x]"
                , Helper::getDX11ErrorMessage( result )
                , result
            );

            return false;
        }

        if( this->createSwapChain_( dxgiFactory ) == false )
        {
            LOGGER_ERROR( "invalid create swap chain" );

            return false;
        }

        if( this->createRenderTargetView_() == false )
        {
            LOGGER_ERROR( "invalid create render target view" );

            return false;
        }

        if( this->createDepthStencilBuffer_() == false )
        {
            LOGGER_ERROR( "invalid create depth stencil buffer" );

            return false;
        }

        if( this->createDepthStencilView_() == false )
        {
            LOGGER_ERROR( "invalid create depth stencil view" );

            return false;
        }

        for( const DX11RenderProgramPtr & program : m_deferredCompilePrograms )
        {
            if( program->compile( m_pD3DDevice ) == false )
            {
                LOGGER_ERROR( "invalid compile program '%s'"
                    , program->getName().c_str()
                );

                return false;
            }
        }

        m_deferredCompilePrograms.clear();

        LOGGER_MESSAGE_RELEASE( "DirectX11 create render window successfully!" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setProjectionMatrix( const mt::mat4f & _projectionMatrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        float DirectX11_PerfectPixelOffsetX = CONFIG_VALUE( "DirectX11", "PerfectPixelOffsetX", 0.f );
        float DirectX11_PerfectPixelOffsetY = CONFIG_VALUE( "DirectX11", "PerfectPixelOffsetY", 0.f );

        float perfect_x = DirectX11_PerfectPixelOffsetX / (m_windowViewport.end.x - m_windowViewport.begin.x);
        float perfect_y = DirectX11_PerfectPixelOffsetY / (m_windowViewport.end.y - m_windowViewport.begin.y);

        mt::mat4f vmperfect;
        mt::make_translation_m4( &vmperfect, perfect_x, perfect_y, 0.f );

        mt::mul_m4_m4( &m_projectionMatrix, _projectionMatrix, vmperfect );

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setViewMatrix( const mt::mat4f & _modelViewMatrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        m_modelViewMatrix = _modelViewMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setWorldMatrix( const mt::mat4f & _worldMatrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        m_worldMatrix = _worldMatrix;

        this->updateWVPInvMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr DX11RenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderImagePtr renderImage = m_factoryRenderImage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderImage, "invalid create render texture" );

        renderImage->setDirect3D11Device( m_pD3DDevice );

        if( renderImage->initialize( _mipmaps, _width, _height, _format ) == false )
        {
            LOGGER_ERROR( "can't initialize image %ux%u format [%u]"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        LOGGER_INFO( "render", "texture normal created %ux%u format %u"
            , renderImage->getHWWidth()
            , renderImage->getHWHeight()
            , renderImage->getHWPixelFormat()
        );

        m_renderResourceHandlers.push_back( renderImage.get() );

#ifdef MENGINE_DEBUG
        bool OPTION_logcreateimage = HAS_OPTION( "logcreateimage" );

        if( OPTION_logcreateimage == true )
        {
            LOGGER_STATISTIC( "create texture [%u:%u] format %u (doc %s)"
                , renderImage->getHWWidth()
                , renderImage->getHWHeight()
                , renderImage->getHWPixelFormat()
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }
#endif

#ifdef MENGINE_DEBUG
        ++m_textureCount;

        uint32_t hwWidth = renderImage->getHWHeight();
        uint32_t hwHeight = renderImage->getHWHeight();
        EPixelFormat hwPixelFormat = renderImage->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        m_textureMemoryUse += memoryUse;
#endif

        return renderImage;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX11RenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetTexturePtr renderTargetTexture = m_factoryRenderTargetTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetTexture );

        renderTargetTexture->setDirect3D11Device( m_pD3DDevice );

        if( renderTargetTexture->initialize( _width, _height, _format ) == false )
        {
            LOGGER_ERROR( "can't initialize offscreen target %ux%u format %u"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        m_renderResourceHandlers.push_back( renderTargetTexture.get() );

        LOGGER_INFO( "render", "offscreen target created %ux%u format %u"
            , _width
            , _height
            , _format
        );

#ifdef MENGINE_DEBUG
        ++m_textureCount;

        uint32_t hwWidth = renderTargetTexture->getHWHeight();
        uint32_t hwHeight = renderTargetTexture->getHWHeight();
        EPixelFormat hwPixelFormat = renderTargetTexture->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        m_textureMemoryUse += memoryUse;
#endif

        return renderTargetTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX11RenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetOffscreenPtr renderTargetOffscreen = m_factoryRenderTargetOffscreen->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetOffscreen );

        renderTargetOffscreen->setDirect3D11Device( m_pD3DDevice );

        ID3D11Texture2D * backBufferPtr;
        IF_DXCALL( m_dxgiSwapChain, GetBuffer, (0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBufferPtr) )
        {
            return nullptr;
        }

        if( renderTargetOffscreen->initialize( _width, _height, backBufferPtr ) == false )
        {
            LOGGER_ERROR( "can't initialize offscreen target %ux%u format %u"
                , _width
                , _height
                , _format
            );

            return nullptr;
        }

        LOGGER_INFO( "render", "offscreen target created %ux%u format %u"
            , _width
            , _height
            , _format
        );

#ifdef MENGINE_DEBUG
        ++m_textureCount;

        uint32_t hwWidth = renderTargetOffscreen->getHWHeight();
        uint32_t hwHeight = renderTargetOffscreen->getHWHeight();
        EPixelFormat hwPixelFormat = renderTargetOffscreen->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        m_textureMemoryUse += memoryUse;
#endif

        return renderTargetOffscreen;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr DX11RenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetTexturePtr renderTargetTexture = stdex::intrusive_static_cast<DX11RenderTargetTexturePtr>(_renderTarget);

        DX11RenderImageTargetPtr renderImageTarget = m_factoryRenderImageTarget->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderImageTarget );

        renderImageTarget->setDirect3D11Device( m_pD3DDevice );

        renderImageTarget->initialize( renderTargetTexture );

        LOGGER_INFO( "render", "render image target created" );

        return renderImageTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialStageCacheInterfacePtr DX11RenderSystem::createRenderMaterialStageCache( const RenderMaterialStage * _stage, const DocumentInterfacePtr & _doc )
    {
        DX11RenderMaterialStageCachePtr materialStageCache = m_factoryRenderMaterialStageCache->createObject( _doc );

        if( materialStageCache->initialize( m_pD3DDevice, _stage ) == false )
        {
            return nullptr;
        }

        return materialStageCache;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::resetDevice_()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::beginScene()
    {
        ID3D11DeviceContext * pD3DDeviceImmediateContext;
        m_pD3DDevice->GetImmediateContext( &pD3DDeviceImmediateContext );

        m_pD3DDeviceImmediateContext.Attach( pD3DDeviceImmediateContext );

        // Bind the render target view and depth stencil buffer to the output render pipeline.
        ID3D11RenderTargetView * d3dRenderTargetViews[1] = {m_renderTargetView.Get()};
        ID3D11DepthStencilView * depthStencilView = m_depthStencilView.Get();
        m_pD3DDeviceContext->OMSetRenderTargets( 1, d3dRenderTargetViews, depthStencilView );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::endScene()
    {
        ID3D11RenderTargetView * nullViews[1] = {NULL};
        m_pD3DDeviceContext->OMSetRenderTargets( 1, nullViews, NULL );

        m_pD3DDeviceImmediateContext = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::swapBuffers()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDeviceContext, "device context not found" );

        // Present the back buffer to the screen since rendering is complete.

        UINT SyncInterval = 1;
        UINT Flags = 0;

        if( m_waitForVSync == false )
        {
            SyncInterval = 0;
            Flags = DXGI_PRESENT_DO_NOT_WAIT;
        }

        HRESULT hr = m_dxgiSwapChain->Present( SyncInterval, Flags );

        switch( hr )
        {
        case ERROR_SUCCESS:
            {
                //Empty
            }break;
        case DXGI_ERROR_WAS_STILL_DRAWING:
            {
                //Empty
            }break;
        case DXGI_ERROR_DEVICE_REMOVED:
            {
                HRESULT hr_reason = m_pD3DDevice->GetDeviceRemovedReason();

                LOGGER_ERROR( "device removed [%x]: %s"
                    , (uint32_t)hr_reason
                    , Helper::getDX11ErrorMessage( hr_reason )
                );
            }break;
        case DXGI_ERROR_DEVICE_RESET:
            {
                LOGGER_ERROR( "device reset" );
            }break;
        default:
            {
                LOGGER_ERROR( "invalid swap chain present [%x]: %s"
                    , (uint32_t)hr
                    , Helper::getDX11ErrorMessage( hr )
                );

                //refresh window
                this->changeWindowMode( m_windowResolution, m_windowFullscreen );
            }break;
        }

        ++m_frames;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, double _depth, int32_t _stencil )
    {
        MENGINE_UNUSED( _frameBufferTypes );

        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDeviceContext, "device context not found" );

        float color[4];

        // Setup the color to clear the buffer to.
        color[0] = _color.getR();
        color[1] = _color.getG();
        color[2] = _color.getB();
        color[3] = _color.getA();

        m_pD3DDeviceContext->ClearRenderTargetView( m_renderTargetView.Get(), color );

        float d3d_depthf = (float)_depth;
        
        m_pD3DDeviceContext->ClearDepthStencilView( m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, d3d_depthf, (UINT8)_stencil );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setScissor( const Viewport & _viewport )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDeviceContext, "device context not found" );

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

        //DXCALL(m_pD3DDevice, SetRenderState, (D3DRS_SCISSORTESTENABLE, TRUE));
        m_D3DRasterizerStateDesc.ScissorEnable = TRUE;

        // scissors
        m_pD3DDeviceContext->RSSetScissorRects( 1, &r );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::removeScissor()
    {
        m_D3DRasterizerStateDesc.ScissorEnable = FALSE;

        m_invalidateRasterizerState = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setViewport( const Viewport & _viewport )
    {
        if( m_viewport.equalViewport( _viewport, 0.5f ) == true )
        {
            return;
        }

        m_viewport = _viewport;

        this->updateViewport_( m_viewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::updateViewport_( const Viewport & _viewport )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3D11_VIEWPORT viewport;
        // Setup the viewport for rendering.
        viewport.Width = _viewport.getWidth();
        viewport.Height = _viewport.getHeight();
        viewport.MinDepth = 0.f;
        viewport.MaxDepth = 1.f;
        viewport.TopLeftX = MT_floorf( _viewport.begin.x + 0.5f );
        viewport.TopLeftY = MT_floorf( _viewport.begin.y + 0.5f );

        // Create the viewport.
        m_pD3DDeviceContext->RSSetViewports( 1, &viewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_dxgiSwapChain, "swap chain not created" );

        // Release all outstanding references to the swap chain's buffers.
        m_renderTargetView = nullptr;
        m_depthStencilBuffer = nullptr;
        m_depthStencilView = nullptr;

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        IF_DXCALL( m_dxgiSwapChain, SetFullscreenState, (_fullscreen, nullptr) )
        {
            return;
        }
#endif

        m_windowFullscreen = _fullscreen;

        uint32_t resolutionWidth = _resolution.getWidth();
        uint32_t resolutionHeight = _resolution.getHeight();

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        DXGI_MODE_DESC modeDesc;
        modeDesc.Width = resolutionWidth;
        modeDesc.Height = resolutionHeight;
        this->updateVSyncDPP_( resolutionWidth, resolutionHeight, &modeDesc.RefreshRate );
        modeDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        IF_DXCALL( m_dxgiSwapChain, ResizeTarget, (&modeDesc) )
        {
            return;
        }
#endif

        IF_DXCALL( m_dxgiSwapChain, ResizeBuffers, (2, resolutionWidth, resolutionHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0) )
        {
            return;
        }

        m_windowResolution = _resolution;

        // create new render targets
        // Get the pointer to the back buffer.
        ID3D11Texture2D * backBuffer;
        IF_DXCALL( m_dxgiSwapChain, GetBuffer, (0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBuffer) )
        {
            return;
        }

        // Create the render target view with the back buffer pointer.
        ID3D11RenderTargetView * renderTargetView;
        IF_DXCALL( m_pD3DDevice, CreateRenderTargetView, (backBuffer, nullptr, &renderTargetView) )
        {
            return;
        }

        m_renderTargetView.Attach( renderTargetView );

        // Release pointer to the back buffer as we no longer need it.
        backBuffer->Release();
        backBuffer = 0;

        // Initialize the description of the depth buffer.
        D3D11_TEXTURE2D_DESC depthBufferDesc;
        ZeroMemory( &depthBufferDesc, sizeof( depthBufferDesc ) );

        // Set up the description of the depth buffer.
        depthBufferDesc.Width = m_windowResolution.getWidth();
        depthBufferDesc.Height = m_windowResolution.getHeight();
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.ArraySize = 1;
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthBufferDesc.CPUAccessFlags = 0;
        depthBufferDesc.MiscFlags = 0;

        // Create the texture for the depth buffer using the filled out description.
        ID3D11Texture2D * depthStencilBuffer;
        IF_DXCALL( m_pD3DDevice, CreateTexture2D, (&depthBufferDesc, nullptr, &depthStencilBuffer) )
        {
            return;
        }

        m_depthStencilBuffer.Attach( depthStencilBuffer );

        // Initialize the depth stencil view.
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory( &depthStencilViewDesc, sizeof( depthStencilViewDesc ) );

        // Set up the depth stencil view description.
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        if( m_multiSampleCount <= 1 )
        {
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        }
        else
        {
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        }

        depthStencilViewDesc.Texture2D.MipSlice = 0;

        // Create the depth stencil view.
        ID3D11DepthStencilView * depthStencilView;
        IF_DXCALL( m_pD3DDevice, CreateDepthStencilView, (m_depthStencilBuffer.Get(), &depthStencilViewDesc, &depthStencilView) )
        {
            return;
        }

        m_depthStencilView.Attach( depthStencilView );

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        this->updateViewport_( m_windowViewport );

        if( this->restore_() == false )
        {
            LOGGER_ERROR( "Graphics change mode failed" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderSystem::getAvailableTextureMemory() const
    {
        //ToDo

        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderSystem::getTextureMemoryUse() const
    {
        return m_textureMemoryUse;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderSystem::getTextureCount() const
    {
        return m_textureCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11DevicePtr & DX11RenderSystem::getDirect3D11Device() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11DeviceContextPtr & DX11RenderSystem::getDirect3D11DeviceContext() const
    {
        return m_pD3DDeviceContext;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderSystem::getMaxCombinedTextureImageUnits() const
    {
        return (uint32_t)D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onWindowChangeFullscreenPrepare( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onWindowMovedOrResized()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onWindowClose()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTextureMatrix( uint32_t _stage, const mt::mat4f & _matrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        MENGINE_UNUSED( _stage );
        MENGINE_UNUSED( _matrix );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::releaseResources_()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDeviceContext, "device not created" );

        m_pD3DDeviceContext->OMSetDepthStencilState( nullptr, 1 );
        m_pD3DDeviceContext->RSSetState( nullptr );
        m_pD3DDeviceContext->OMSetBlendState( nullptr, nullptr, 0xffffffff );

        ID3D11SamplerState * samplerStates[MENGINE_MAX_TEXTURE_STAGES] = {nullptr};
        m_pD3DDeviceContext->PSSetSamplers( 0, MENGINE_MAX_TEXTURE_STAGES, samplerStates );

        if( m_vertexBufferEnable == true )
        {
            m_vertexBufferEnable = false;

            m_pD3DDeviceContext->IASetVertexBuffers( 0, 1, nullptr, nullptr, nullptr );
        }

        if( m_indexBufferEnable == true )
        {
            m_indexBufferEnable = false;

            m_pD3DDeviceContext->IASetIndexBuffer( nullptr, DXGI_FORMAT_UNKNOWN, 0 );
        }

        for( uint32_t index = 0; index != MENGINE_MAX_TEXTURE_STAGES; ++index )
        {
            if( m_textureEnable[index] == false )
            {
                continue;
            }

            m_textureEnable[index] = false;
        }

        for( uint32_t index = 0; index != MENGINE_MAX_TEXTURE_STAGES; ++index )
        {
            ID3D11ShaderResourceView * pShaderResourceViews[1] = {nullptr};
            m_pD3DDeviceContext->PSSetShaderResources( index, 1, pShaderResourceViews );
        }

        ID3D11Buffer * constantBuffers[1] = {nullptr};
        m_pD3DDeviceContext->VSSetConstantBuffers( 0, 1, constantBuffers );

        if( m_vertexShaderEnable == true )
        {
            m_vertexShaderEnable = false;

            m_pD3DDeviceContext->VSSetShader( nullptr, nullptr, 0 );
        }

        if( m_fragmentShaderEnable == true )
        {
            m_fragmentShaderEnable = false;

            m_pD3DDeviceContext->PSSetShader( nullptr, nullptr, 0 );
        }

        if( m_vertexAttributeEnable == true )
        {
            m_vertexAttributeEnable = false;

            m_pD3DDeviceContext->IASetInputLayout( nullptr );
        }

        ID3D11RenderTargetView * nullViews[1] = {nullptr};
        m_pD3DDeviceContext->OMSetRenderTargets( 1, nullViews, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::restore_()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr DX11RenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        DX11RenderVertexBufferPtr buffer = m_factoryVertexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer );

        buffer->setDirect3D11Device( m_pD3DDevice );

        if( buffer->initialize( _vertexSize, _bufferType ) == false )
        {
            LOGGER_ERROR( "invalid initialize vertex buffer [%u] type [%u]"
                , _vertexSize
                , _bufferType
            );

            return nullptr;
        }

        m_renderResourceHandlers.push_back( buffer.get() );

#ifdef MENGINE_DEBUG
        ++m_vertexBufferCount;
#endif

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        if( _vertexBuffer == nullptr )
        {
            if( m_vertexBufferEnable == false )
            {
                return true;
            }

            m_vertexBufferEnable = false;

            return true;
        }

        DX11RenderVertexBuffer * dx11VertexBuffer = _vertexBuffer.getT<DX11RenderVertexBuffer *>();
        dx11VertexBuffer->enable( m_pD3DDeviceImmediateContext );

        m_vertexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr DX11RenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        DX11RenderIndexBufferPtr buffer = m_factoryIndexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer );

        buffer->setDirect3D11Device( m_pD3DDevice );

        if( buffer->initialize( _indexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        m_renderResourceHandlers.push_back( buffer.get() );

#ifdef MENGINE_DEBUG
        ++m_indexBufferCount;
#endif

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        if( _indexBuffer == nullptr )
        {
            if( m_indexBufferEnable == false )
            {
                return true;
            }

            m_indexBufferEnable = false;
            m_pD3DDeviceContext->IASetIndexBuffer( nullptr, DXGI_FORMAT_UNKNOWN, 0 );

            return true;
        }

        DX11RenderIndexBuffer * dx11IndexBuffer = _indexBuffer.getT<DX11RenderIndexBuffer *>();
        dx11IndexBuffer->enable( m_pD3DDeviceImmediateContext );

        m_indexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3D11_PRIMITIVE_TOPOLOGY primitiveType = Helper::toD3DPrimitiveType( _desc.primitiveType );

        if( primitiveType != D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
        {
            return;
        }

        if( _stageCache != nullptr )
        {
            DX11RenderMaterialStageCache * dx11MaterialStageCache = _stageCache.getT<DX11RenderMaterialStageCache *>();
            dx11MaterialStageCache->begin( m_pD3DDeviceContext );
        }

        if( m_invalidateRasterizerState == true )
        {
            if( this->updateRasterizerState_() == false )
            {
                return;
            }
        }

        m_pD3DDeviceContext->RSSetState( m_pD3DRasterizerState.Get() );

        UINT primCount = Helper::getPrimitiveCount( _desc.primitiveType, _desc.indexCount );

        m_pD3DDeviceContext->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        m_pD3DDeviceContext->DrawIndexed( primCount * 3, _desc.startIndex, _desc.baseVertexIndex );

        m_pD3DDeviceContext->RSSetState( nullptr );

        if( _stageCache != nullptr )
        {
            DX11RenderMaterialStageCache * dx11MaterialStageCache = _stageCache.getT<DX11RenderMaterialStageCache *>();
            dx11MaterialStageCache->end( m_pD3DDeviceContext );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        MENGINE_UNUSED( _program );

        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "no support stage [%u] (max %u)"
                , _stage
                , MaxCombinedTextureImageUnits
            );

            return;
        }

        if( _texture != nullptr )
        {
            DX11RenderImageExtensionInterface * extension = _texture->getUnknown();
            extension->bind( m_pD3DDeviceImmediateContext, _stage );

            m_textureEnable[_stage] = true;
        }
        else
        {
            //m_pD3DDeviceContext->PSSetShaderResources( _stage, 1, nullptr );

            m_textureEnable[_stage] = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate )
    {
        MENGINE_UNUSED( _src );
        MENGINE_UNUSED( _dst );
        MENGINE_UNUSED( _op );
        MENGINE_UNUSED( _separateSrc );
        MENGINE_UNUSED( _separateDst );
        MENGINE_UNUSED( _separateOp );
        MENGINE_UNUSED( _separate );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        MENGINE_UNUSED( _stage );
        MENGINE_UNUSED( _modeU );
        MENGINE_UNUSED( _modeV );
        MENGINE_UNUSED( _border );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTextureFactor( uint32_t _color )
    {
        MENGINE_UNUSED( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setCullMode( ECullMode _mode )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDeviceContext, "context not created" );

        m_D3DRasterizerStateDesc.CullMode = Helper::toD3DCullMode( _mode );

        m_invalidateRasterizerState = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setDepthBufferTestEnable( bool _depthTest )
    {
        MENGINE_UNUSED( _depthTest );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        MENGINE_UNUSED( _depthWrite );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        MENGINE_UNUSED( _depthFunction );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setFillMode( EFillMode _mode )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDeviceContext, "context not created" );

        m_D3DRasterizerStateDesc.FillMode = Helper::toD3DFillMode( _mode );

        m_invalidateRasterizerState = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
    {
        MENGINE_UNUSED( _r );
        MENGINE_UNUSED( _g );
        MENGINE_UNUSED( _b );
        MENGINE_UNUSED( _a );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        MENGINE_UNUSED( _alphaBlend );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
    {
        MENGINE_UNUSED( _minification );
        MENGINE_UNUSED( _mipmap );
        MENGINE_UNUSED( _magnification );

        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "no support stage [%u] (max %u)"
                , _stage
                , MaxCombinedTextureImageUnits
            );

            return;
        }

        // TODO: somehow needs to find only 1 filter

     /*   D3DTEXTUREFILTERTYPE dx_minification = Helper::toD3DTextureFilter( _minification );
        D3DTEXTUREFILTERTYPE dx_mipmap = Helper::toD3DTextureFilter( _mipmap );
        D3DTEXTUREFILTERTYPE dx_magnification = Helper::toD3DTextureFilter( _magnification );

        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MINFILTER, dx_minification) );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MIPFILTER, dx_mipmap) );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MAGFILTER, dx_magnification) );
        */
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr DX11RenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentInterfacePtr & _doc )
    {
        DX11RenderVertexAttributePtr attribute = m_factoryRenderVertexAttribute->createObject( _doc );

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
    RenderFragmentShaderInterfacePtr DX11RenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        DX11RenderFragmentShaderPtr shader = m_factoryRenderFragmentShader->createObject( _doc );

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
    RenderVertexShaderInterfacePtr DX11RenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        DX11RenderVertexShaderPtr shader = m_factoryRenderVertexShader->createObject( _doc );

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
    RenderProgramInterfacePtr DX11RenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertexShader, const RenderFragmentShaderInterfacePtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _samplerCount );

        DX11RenderProgramPtr program = m_factoryRenderProgram->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( program, "invalid create program '%s'"
            , _name.c_str()
        );

        if( program->initialize( _name, _vertexShader, _fragmentShader, _vertexAttribute ) == false )
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
    void DX11RenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        if( _program != nullptr )
        {
            DX11RenderProgram * dx9_program = _program.getT<DX11RenderProgram *>();

            dx9_program->enable( m_pD3DDeviceImmediateContext );

            const RenderVertexAttributeInterfacePtr & vertexAttribute = dx9_program->getVertexAttribute();
            const RenderVertexShaderInterfacePtr & vertexShader = dx9_program->getVertexShader();
            const RenderFragmentShaderInterfacePtr & fragmentShader = dx9_program->getFragmentShader();

            bool vertexAttributeEnable = vertexAttribute != nullptr;
            bool vertexShaderEnable = vertexShader != nullptr;
            bool fragmentShaderEnable = fragmentShader != nullptr;

            if( m_vertexAttributeEnable == true && vertexAttributeEnable == false )
            {
                m_vertexAttributeEnable = false;

                m_pD3DDeviceContext->IASetInputLayout( nullptr );
            }

            if( m_vertexShaderEnable == true && vertexShaderEnable == false )
            {
                m_vertexShaderEnable = false;

                m_pD3DDeviceContext->VSSetShader( nullptr, nullptr, 0 );
            }

            if( m_fragmentShaderEnable == true && fragmentShaderEnable == false )
            {
                m_fragmentShaderEnable = false;

                m_pD3DDeviceContext->PSSetShader( nullptr, nullptr, 0 );
            }
        }
        else
        {
            if( m_vertexShaderEnable == true )
            {
                m_vertexShaderEnable = false;

                m_pD3DDeviceContext->VSSetShader( nullptr, nullptr, 0 );
            }

            if( m_fragmentShaderEnable == true )
            {
                m_vertexShaderEnable = false;

                m_pD3DDeviceContext->PSSetShader( nullptr, nullptr, 0 );
            }

            if( m_vertexAttributeEnable == true )
            {
                m_vertexShaderEnable = false;

                m_pD3DDeviceContext->IASetInputLayout( nullptr );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        DX11RenderProgramPtr dx9_program = stdex::intrusive_static_cast<DX11RenderProgramPtr>(_program);

        dx9_program->bindMatrix( m_pD3DDeviceImmediateContext, m_worldMatrix, m_modelViewMatrix, m_projectionMatrix, m_totalWVPInvMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr DX11RenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc )
    {
        DX11RenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( variable, "invalid create program variable" );

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "invalid initialize program variable" );

            return nullptr;
        }

        return variable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable )
    {
        if( _variable == nullptr )
        {
            return true;
        }

        DX11RenderProgramVariable * dx9_variable = _variable.getT<DX11RenderProgramVariable *>();

        bool successful = dx9_variable->apply( m_pD3DDevice, m_pD3DDeviceImmediateContext, _program );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setVSync( bool _vSync )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_dxgiSwapChain, "swap chain not created" );

        if( m_waitForVSync == _vSync )
        {
            return;
        }

        m_waitForVSync = _vSync;

        if( this->restore_() == false )
        {
            LOGGER_ERROR( "Graphics change mode failed" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::createSwapChain_( IDXGIFactory2 * _dxgiFactory )
    {
        // Create Swap Chain
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;

        // Initialize the swap chain description.
        ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );

        // Set the width and height of the back buffer.
        swapChainDesc.Width = m_windowResolution.getWidth();
        swapChainDesc.Height = m_windowResolution.getHeight();

        // Set regular 32-bit surface for the back buffer.
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.Stereo = FALSE;

        // Turn multisampling off.
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;

        // Set the usage of the back buffer.
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;

        // Set the scan line ordering and scaling to unspecified.
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

        // Discard the back buffer contents after presenting.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        // Don't set the advanced flags.
        swapChainDesc.Flags = 0;

        IDXGISwapChain1 * dxgiSwapChain;

        // Set the handle for the window to render to.
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        Win32PlatformServiceExtensionInterface * win32Extension = PLATFORM_SERVICE()
            ->getUnknown();

        HWND hWnd = win32Extension->getWindowHandle();

        IF_DXCALL( _dxgiFactory, CreateSwapChainForHwnd, (m_pD3DDevice.Get(), hWnd, &swapChainDesc, NULL, NULL, &dxgiSwapChain) )
        {
            return false;
        }

        IF_DXCALL( _dxgiFactory, MakeWindowAssociation, (hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER) )
        {
            return false;
        }
#elif defined(MENGINE_ENVIRONMENT_PLATFORM_SDL) && defined(MENGINE_PLATFORM_WINDOWS)
        SDLPlatformServiceExtensionInterface * sdlExtension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        IInspectable * iWindow = sdlExtension->getWindowHandle();

        IF_DXCALL( _dxgiFactory, CreateSwapChainForCoreWindow, (m_pD3DDevice.Get(), iWindow, &swapChainDesc, NULL, &dxgiSwapChain) )
        {
            return false;
        }
#else
#   error "unsupported platform"
#endif

        m_dxgiSwapChain.Attach( dxgiSwapChain );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::createRenderTargetView_()
    {
        // Get the pointer to the back buffer.
        ID3D11Texture2D * backBuffer;
        IF_DXCALL( m_dxgiSwapChain, GetBuffer, (0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBuffer) )
        {
            return false;
        }

        // Create the render target view with the back buffer pointer.
        ID3D11RenderTargetView * renderTargetView;
        IF_DXCALL( m_pD3DDevice, CreateRenderTargetView, (backBuffer, nullptr, &renderTargetView) )
        {
            return false;
        }

        m_renderTargetView.Attach( renderTargetView );

        // Release pointer to the back buffer as we no longer need it.
        backBuffer->Release();
        backBuffer = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::createDepthStencilBuffer_()
    {
        // Initialize the description of the depth buffer.
        D3D11_TEXTURE2D_DESC depthBufferDesc;
        ZeroMemory( &depthBufferDesc, sizeof( depthBufferDesc ) );

        // Set up the description of the depth buffer.
        depthBufferDesc.Width = m_windowResolution.getWidth();
        depthBufferDesc.Height = m_windowResolution.getHeight();
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.ArraySize = 1;
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthBufferDesc.CPUAccessFlags = 0;
        depthBufferDesc.MiscFlags = 0;

        // Create the texture for the depth buffer using the filled out description.
        ID3D11Texture2D * depthStencilBuffer;
        IF_DXCALL( m_pD3DDevice, CreateTexture2D, (&depthBufferDesc, nullptr, &depthStencilBuffer) )
        {
            return false;
        }

        m_depthStencilBuffer.Attach( depthStencilBuffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::createDepthStencilView_()
    {
        // Initialize the depth stencil view.
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory( &depthStencilViewDesc, sizeof( depthStencilViewDesc ) );

        // Set up the depth stencil view description.
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        if( m_multiSampleCount <= 1 )
        {
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        }
        else
        {
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        }

        depthStencilViewDesc.Texture2D.MipSlice = 0;

        // Create the depth stencil view.
        ID3D11DepthStencilView * depthStencilView;
        IF_DXCALL( m_pD3DDevice, CreateDepthStencilView, (m_depthStencilBuffer.Get(), &depthStencilViewDesc, &depthStencilView) )
        {
            return false;
        }

        m_depthStencilView.Attach( depthStencilView );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::updateRasterizerState_() const
    {
        // Create the rasterizer state from the description we just filled out.
        ID3D11RasterizerState * rasterizerState;
        IF_DXCALL( m_pD3DDevice, CreateRasterizerState, (&m_D3DRasterizerStateDesc, &rasterizerState) )
        {
            return false;
        }

        m_pD3DRasterizerState.Attach( rasterizerState );

        m_invalidateRasterizerState = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::updateVSyncDPP_( UINT _width, UINT _height, DXGI_RATIONAL * const _refreshRate )
    {
        // Set the refresh rate of the back buffer.
        if( m_waitForVSync == true )
        {
            for( const DXGI_MODE_DESC & displayModeDesc : m_DisplayModeList )
            {
                if( displayModeDesc.Width == _width && displayModeDesc.Height == _height )
                {
                    _refreshRate->Numerator = displayModeDesc.RefreshRate.Numerator;
                    _refreshRate->Denominator = displayModeDesc.RefreshRate.Denominator;

                    return;
                }
            }

            LOGGER_ERROR( "not found display mode" );
        }

        _refreshRate->Numerator = 60;
        _refreshRate->Denominator = 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyVertexAttribute_( DX11RenderVertexAttribute * _attribute )
    {
        _attribute->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyVertexShader_( DX11RenderVertexShader * _shader )
    {
        _shader->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyFragmentShader_( DX11RenderFragmentShader * _shader )
    {
        _shader->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyProgram_( DX11RenderProgram * _program )
    {
        _program->release();
        _program->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyVertexBuffer_( DX11RenderVertexBuffer * _buffer )
    {
        _buffer->finalize();

#ifdef MENGINE_DEBUG
        --m_vertexBufferCount;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyIndexBuffer_( DX11RenderIndexBuffer * _buffer )
    {
        _buffer->finalize();

#ifdef MENGINE_DEBUG
        --m_indexBufferCount;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderImage_( DX11RenderImage * _image )
    {
        _image->finalize();

#ifdef MENGINE_DEBUG
        --m_textureCount;

        uint32_t hwWidth = _image->getHWWidth();
        uint32_t hwHeight = _image->getHWHeight();
        EPixelFormat hwPixelFormat = _image->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        m_textureMemoryUse -= memoryUse;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderImageTarget_( DX11RenderImageTarget * _imageTarget )
    {
        _imageTarget->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderTargetTexture_( DX11RenderTargetTexture * _targetTexture )
    {
        _targetTexture->finalize();

#ifdef MENGINE_DEBUG
        --m_textureCount;

        uint32_t hwWidth = _targetTexture->getHWWidth();
        uint32_t hwHeight = _targetTexture->getHWHeight();
        EPixelFormat hwPixelFormat = _targetTexture->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        m_textureMemoryUse -= memoryUse;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderTargetOffscreen_( DX11RenderTargetOffscreen * _targetOffscreen )
    {
        _targetOffscreen->finalize();

#ifdef MENGINE_DEBUG
        --m_textureCount;

        uint32_t hwWidth = _targetOffscreen->getHWWidth();
        uint32_t hwHeight = _targetOffscreen->getHWHeight();
        EPixelFormat hwPixelFormat = _targetOffscreen->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

        m_textureMemoryUse -= memoryUse;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderMaterialStageCache_( DX11RenderMaterialStageCache * _materialStageCache )
    {
        _materialStageCache->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::updateWVPInvMatrix_()
    {
        mt::mat4f totalWVPMatrix = m_worldMatrix * m_modelViewMatrix * m_projectionMatrix;

        mt::transpose_m4_m4( &m_totalWVPInvMatrix, totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
}