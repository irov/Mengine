#include "DX11RenderSystem.h"

#include "Interface/RenderServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#   include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"
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
#include "DX11RenderSurface.h"

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
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"

#include "math/uv4.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Mengine::DX11RenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderSystem::DX11RenderSystem()
        : m_windowFullscreen( true )
        , m_adapterToUse( 0 )
        , m_multiSampleCount( 0 )
        , m_vertexBufferEnable( false )
        , m_indexBufferEnable( false )
        , m_vertexAttributeEnable( false )
        , m_vertexShaderEnable( false )
        , m_fragmentShaderEnable( false )
        , m_frames( 0 )
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
        MENGINE_IF_DX11_CALL( factory, EnumAdapters, (m_adapterToUse, &adapter) )
        {
            return false;
        }

        m_factoryRenderDevice = Helper::makeFactoryPoolWithListener<DX11RenderDevice, 2>( this, &DX11RenderSystem::onDestroyRenderDevice_, MENGINE_DOCUMENT_FACTORABLE );

        DX11RenderDevicePtr renderDevice = m_factoryRenderDevice->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( renderDevice, "invalid create render device" );

        if( renderDevice->initialize( adapter ) == false )
        {
            return false;
        }

        m_renderDevice = renderDevice;
        m_pD3DDevice = renderDevice->getDirect3D11Device();
        m_pD3DDeviceContext = renderDevice->getDirect3D11DeviceContext();

        IDXGIOutput * adapterOutput;
        // Enumerate the primary adapter output (monitor).
        MENGINE_IF_DX11_CALL( adapter, EnumOutputs, (0, &adapterOutput) )
        {
            return false;
        }

        // Get the number of modes that fit the DXGI_FORMAT_B8G8R8A8_UNORM display format for the adapter output (monitor).
        UINT DisplayModeListNum;
        MENGINE_IF_DX11_CALL( adapterOutput, GetDisplayModeList, (DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &DisplayModeListNum, nullptr) )
        {
            return false;
        }

        // Create a list to hold all the possible display modes for this monitor/video card combination.
        m_DisplayModeList.resize( DisplayModeListNum );

        MENGINE_IF_DX11_CALL( adapterOutput, GetDisplayModeList, (DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &DisplayModeListNum, m_DisplayModeList.data()) )
        {
            return false;
        }

        MENGINE_DX11_RELEASE( adapterOutput );

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

        LOGGER_MESSAGE( "D3D Adapter Description: %ls", AdapterDesc.Description );

        LOGGER_MESSAGE( "D3D Adapter VendorId: %u", AdapterDesc.VendorId );
        LOGGER_MESSAGE( "D3D Adapter DeviceId: %u", AdapterDesc.DeviceId );
        LOGGER_MESSAGE( "D3D Adapter SubSysId: %u", AdapterDesc.SubSysId );
        LOGGER_MESSAGE( "D3D Adapter Revision: %u", AdapterDesc.Revision );

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
        m_factoryRenderSurface = Helper::makeFactoryPoolWithListener<DX11RenderSurface, 2>( this, &DX11RenderSystem::onDestroyRenderSurface_, MENGINE_DOCUMENT_FACTORABLE );

        DX11RenderImageLockedFactoryStorage::initialize( Helper::makeFactoryPool<DX11RenderImageLocked, 64>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::_finalizeService()
    {
        m_deferredCompilePrograms.clear();

        if( this->releaseResources_() == false )
        {
            LOGGER_ERROR( "invalid release resource" );

            return;
        }

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_renderResourceHandlers );

        m_renderResourceHandlers.clear();

        m_pD3DRasterizerState = nullptr;

        // Flush the immediate context to force cleanup
        if( m_pD3DDeviceContext != nullptr )
        {
            m_pD3DDeviceContext->Flush();
            m_pD3DDeviceContext = nullptr;
        }

        m_renderDevice = nullptr;

        if( m_pD3DDevice != nullptr )
        {
            ID3D11Debug * D3DDevice;
            m_pD3DDevice->QueryInterface( __uuidof(ID3D11Debug), reinterpret_cast<void **>(&D3DDevice) );
            D3DDevice->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
            D3DDevice->Release();

            m_pD3DDevice = nullptr;
        }

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
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderSurface );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderDevice );

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
        m_factoryRenderSurface = nullptr;
        m_factoryRenderDevice = nullptr;

        DX11RenderImageLockedFactoryStorage::finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::setRenderDevice( const RenderDeviceInterfacePtr & _device )
    {
        MENGINE_UNUSED( _device );

        //Direct3D device is created together with the service, external device is not supported
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderDeviceInterfacePtr & DX11RenderSystem::getRenderDevice() const
    {
        return m_renderDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::createRenderWindow( const RenderWindowDesc * _windowDesc )
    {
        m_windowResolution = _windowDesc->resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_windowFullscreen = _windowDesc->fullscreen;
        m_waitForVSync = _windowDesc->waitForVSync;
        m_multiSampleCount = _windowDesc->MultiSampleCount;

        if( m_renderSurface == nullptr )
        {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
            Win32PlatformServiceExtensionInterface * win32Extension = PLATFORM_SERVICE()
                ->getUnknown();

            HWND hWnd = win32Extension->getWindowHandle();
#else
#   error "unsupported platform"
#endif

            DX11RenderSurfacePtr windowRenderSurface = this->createRenderSurface_( hWnd, m_windowResolution, 1.f, MENGINE_DOCUMENT_FACTORABLE );

            if( windowRenderSurface == nullptr )
            {
                LOGGER_ERROR( "invalid create window render surface" );

                return false;
            }

            windowRenderSurface->setVSync( m_waitForVSync );

            m_windowRenderSurface = windowRenderSurface;
            m_renderSurface = windowRenderSurface;
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

            program->setDeferredCompile( false );
        }

        m_deferredCompilePrograms.clear();

        LOGGER_MESSAGE( "DirectX11 create render window successfully!" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::destroyRenderWindow()
    {
        m_renderSurface = nullptr;
        m_windowRenderSurface = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderSurfaceInterfacePtr DX11RenderSystem::createRenderSurface( void * _nativeHandle, const Resolution & _resolution, float _dpiScale, const DocumentInterfacePtr & _doc )
    {
        DX11RenderSurfacePtr surface = this->createRenderSurface_( _nativeHandle, _resolution, _dpiScale, _doc );

        return surface;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::setRenderSurface( const RenderSurfaceInterfacePtr & _surface )
    {
        if( m_pD3DDeviceContext == nullptr )
        {
            return false;
        }

        DX11RenderSurfacePtr surface = DX11RenderSurfacePtr::dynamic_from( _surface );

        if( surface == nullptr )
        {
            return false;
        }

        m_renderSurface = surface;

        m_viewport = Viewport( 0.f, 0.f, 0.f, 0.f );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setProjectionMatrix( const mt::mat4f & _projectionMatrix )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        float DirectX11_PerfectPixelOffsetX = CONFIG_VALUE_FLOAT( "DirectX11", "PerfectPixelOffsetX", 0.f );
        float DirectX11_PerfectPixelOffsetY = CONFIG_VALUE_FLOAT( "DirectX11", "PerfectPixelOffsetY", 0.f );

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
    RenderImageInterfacePtr DX11RenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _layers, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        uint32_t maxTexture2DArrayLayers = this->getMaxTexture2DArrayLayers();

        if( _layers == 0 || _layers > maxTexture2DArrayLayers )
        {
            LOGGER_ERROR( "invalid texture layer count %u (max %u)"
                , _layers
                , maxTexture2DArrayLayers
            );

            return nullptr;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderImagePtr renderImage = m_factoryRenderImage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderImage, "invalid create render texture" );

        renderImage->setDirect3D11Device( m_pD3DDevice );

        if( renderImage->initialize( _mipmaps, _width, _height, _layers, _format ) == false )
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

        return renderImage;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX11RenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetTexturePtr renderTargetTexture = m_factoryRenderTargetTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetTexture, "invalid create render target texture" );

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

        return renderTargetTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX11RenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetOffscreenPtr renderTargetOffscreen = m_factoryRenderTargetOffscreen->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetOffscreen, "invalid create render target offscreen" );

        renderTargetOffscreen->setDirect3D11Device( m_pD3DDevice );

        MENGINE_ASSERTION_MEMORY_PANIC( m_renderSurface, "render surface not created" );

        ID3D11RenderTargetView * colorTarget = m_renderSurface->getColorTarget();

        MENGINE_ASSERTION_MEMORY_PANIC( colorTarget, "render surface color target not found" );

        ID3D11Resource * colorResource = nullptr;
        colorTarget->GetResource( &colorResource );

        ID3D11Texture2D * backBufferPtr = nullptr;
        HRESULT backBufferResult = colorResource->QueryInterface( __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBufferPtr) );

        colorResource->Release();

        if( FAILED( backBufferResult ) )
        {
            LOGGER_ERROR( "invalid query render surface back buffer: %s [%x]"
                , Helper::getDX11ErrorMessage( backBufferResult )
                , static_cast<uint32_t>(backBufferResult)
            );

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

        return renderTargetOffscreen;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr DX11RenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetTexturePtr renderTargetTexture = stdex::intrusive_static_cast<DX11RenderTargetTexturePtr>(_renderTarget);

        DX11RenderImageTargetPtr renderImageTarget = m_factoryRenderImageTarget->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderImageTarget, "invalid create render image target" );

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
        if( m_renderSurface == nullptr )
        {
            return false;
        }

        if( m_renderSurface->acquire() == false )
        {
            return false;
        }

        if( m_renderSurface->bind( m_pD3DDeviceContext.Get() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::endScene()
    {
        if( m_renderSurface == nullptr )
        {
            return;
        }

        m_renderSurface->unbind( m_pD3DDeviceContext.Get() );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::swapBuffers()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDeviceContext, "device context not found" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_renderSurface, "render surface not created" );

        EDX11PresentResult presentResult = m_renderSurface->present();

        if( presentResult == EDX11PR_RECOVERABLE && m_renderSurface == m_windowRenderSurface )
        {
            //refresh window
            this->changeWindowMode( m_windowResolution, m_windowFullscreen );
        }

        ++m_frames;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, double _depth, int32_t _stencil )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDeviceContext, "device context not found" );

        MENGINE_ASSERTION_MEMORY_PANIC( m_renderSurface, "render surface not created" );

        ID3D11RenderTargetView * colorTarget = m_renderSurface->getColorTarget();
        ID3D11DepthStencilView * depthTarget = m_renderSurface->getDepthTarget();

        if( (_frameBufferTypes & FBT_COLOR) != 0 && colorTarget != nullptr )
        {
            float color[4];

            color[0] = _color.getR();
            color[1] = _color.getG();
            color[2] = _color.getB();
            color[3] = _color.getA();

            m_pD3DDeviceContext->ClearRenderTargetView( colorTarget, color );
        }

        UINT depthStencilFlags = 0;

        if( (_frameBufferTypes & FBT_DEPTH) != 0 )
        {
            depthStencilFlags |= D3D11_CLEAR_DEPTH;
        }

        if( (_frameBufferTypes & FBT_STENCIL) != 0 )
        {
            depthStencilFlags |= D3D11_CLEAR_STENCIL;
        }

        if( depthStencilFlags != 0 && depthTarget != nullptr )
        {
            float d3d_depthf = (float)_depth;

            m_pD3DDeviceContext->ClearDepthStencilView( depthTarget, depthStencilFlags, d3d_depthf, (UINT8)_stencil );
        }
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

        m_D3DRasterizerStateDesc.ScissorEnable = TRUE;

        m_invalidateRasterizerState = true;

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
        viewport.TopLeftX = StdMath::floorf( _viewport.begin.x + 0.5f );
        viewport.TopLeftY = StdMath::floorf( _viewport.begin.y + 0.5f );

        // Create the viewport.
        m_pD3DDeviceContext->RSSetViewports( 1, &viewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_windowRenderSurface == nullptr )
        {
            m_windowResolution = _resolution;

            mt::vec2f windowSize;
            m_windowResolution.calcSize( &windowSize );

            m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

            this->updateViewport_( m_windowViewport );

            return;
        }

        if( m_windowRenderSurface->setFullscreen( _fullscreen ) == false )
        {
            return;
        }

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

        if( m_windowRenderSurface->resizeTarget( modeDesc ) == false )
        {
            return;
        }
#endif

        float dpiScale = m_windowRenderSurface->getDpiScale();

        if( m_windowRenderSurface->resize( _resolution, dpiScale ) == false )
        {
            return;
        }

        m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );

        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        this->updateViewport_( m_windowViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderSystem::getAvailableTextureMemory() const
    {
        //ToDo

        return 0U;
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
    uint32_t DX11RenderSystem::getMaxTexture2DSize() const
    {
        return (uint32_t)D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderSystem::getMaxTexture2DArrayLayers() const
    {
        return (uint32_t)D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDeviceLostPrepare()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::onDeviceLostRestore()
    {
        //Empty

        return true;
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
            m_textureEnable[index] = false;

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
    RenderVertexBufferInterfacePtr DX11RenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        DX11RenderVertexBufferPtr buffer = m_factoryVertexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create vertex buffer" );

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
        dx11VertexBuffer->enable( m_pD3DDeviceContext );

        m_vertexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr DX11RenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        DX11RenderIndexBufferPtr buffer = m_factoryIndexBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create index buffer" );

        buffer->setDirect3D11Device( m_pD3DDevice );

        if( buffer->initialize( _indexSize, _bufferType ) == false )
        {
            return nullptr;
        }

        m_renderResourceHandlers.push_back( buffer.get() );

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
        dx11IndexBuffer->enable( m_pD3DDeviceContext );

        m_indexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3D11_PRIMITIVE_TOPOLOGY primitiveType = Helper::toD3DPrimitiveType( _desc.primitiveType );

        if( primitiveType == D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED )
        {
            LOGGER_ERROR( "unsupported primitive type [%u]"
                , (uint32_t)_desc.primitiveType
            );

            return;
        }

        if( m_invalidateRasterizerState == true )
        {
            if( this->updateRasterizerState_() == false )
            {
                return;
            }
        }

        if( _stageCache != nullptr )
        {
            DX11RenderMaterialStageCache * dx11MaterialStageCache = _stageCache.getT<DX11RenderMaterialStageCache *>();
            dx11MaterialStageCache->begin( m_pD3DDeviceContext );
        }

        m_pD3DDeviceContext->RSSetState( m_pD3DRasterizerState.Get() );

        m_pD3DDeviceContext->IASetPrimitiveTopology( primitiveType );
        m_pD3DDeviceContext->DrawIndexed( _desc.indexCount, _desc.startIndex, _desc.baseVertexIndex );

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
            extension->bind( m_pD3DDeviceContext, _stage );

            m_textureEnable[_stage] = true;
        }
        else
        {
            ID3D11ShaderResourceView * pShaderResourceViews[1] = {nullptr};
            m_pD3DDeviceContext->PSSetShaderResources( _stage, 1, pShaderResourceViews );

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
        MENGINE_UNUSED( _stage );
        MENGINE_UNUSED( _minification );
        MENGINE_UNUSED( _mipmap );
        MENGINE_UNUSED( _magnification );

        //Empty
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
            program->setDeferredCompile( true );

            m_deferredCompilePrograms.emplace_back( program );
        }

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        if( _program != nullptr )
        {
            DX11RenderProgram * dx11Program = _program.getT<DX11RenderProgram *>();

            dx11Program->enable( m_pD3DDeviceContext );

            const RenderVertexAttributeInterfacePtr & vertexAttribute = dx11Program->getVertexAttribute();
            const RenderVertexShaderInterfacePtr & vertexShader = dx11Program->getVertexShader();
            const RenderFragmentShaderInterfacePtr & fragmentShader = dx11Program->getFragmentShader();

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
        DX11RenderProgramPtr dx11Program = stdex::intrusive_static_cast<DX11RenderProgramPtr>(_program);

        dx11Program->bindMatrix( m_pD3DDeviceContext, m_worldMatrix, m_modelViewMatrix, m_projectionMatrix, m_totalWVPInvMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr DX11RenderSystem::createProgramVariableStatic( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc )
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
    RenderProgramVariableInterfacePtr DX11RenderSystem::createProgramVariableDynamic( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc )
    {
        return this->createProgramVariableStatic( _vertexCount, _pixelCount, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable )
    {
        MENGINE_UNUSED( _program );

        if( _variable == nullptr )
        {
            return true;
        }

        DX11RenderProgramVariable * dx11Variable = _variable.getT<DX11RenderProgramVariable *>();

        bool successful = dx11Variable->apply( m_pD3DDevice, m_pD3DDeviceContext );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setVSync( bool _vSync )
    {
        if( m_waitForVSync == _vSync )
        {
            return;
        }

        m_waitForVSync = _vSync;

        if( m_renderSurface != nullptr )
        {
            m_renderSurface->setVSync( _vSync );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::updateRasterizerState_() const
    {
        // Create the rasterizer state from the description we just filled out.
        ID3D11RasterizerState * rasterizerState;
        MENGINE_IF_DX11_CALL( m_pD3DDevice, CreateRasterizerState, (&m_D3DRasterizerStateDesc, &rasterizerState) )
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
        if( _program->getDeferredCompile() == false )
        {
            _program->release();
        }

        _program->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyVertexBuffer_( DX11RenderVertexBuffer * _buffer )
    {
        _buffer->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyIndexBuffer_( DX11RenderIndexBuffer * _buffer )
    {
        _buffer->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderImage_( DX11RenderImage * _image )
    {
        _image->finalize();
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
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderTargetOffscreen_( DX11RenderTargetOffscreen * _targetOffscreen )
    {
        _targetOffscreen->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderMaterialStageCache_( DX11RenderMaterialStageCache * _materialStageCache )
    {
        _materialStageCache->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderSurface_( DX11RenderSurface * _surface )
    {
        _surface->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::onDestroyRenderDevice_( DX11RenderDevice * _device )
    {
        _device->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderSurfacePtr DX11RenderSystem::createRenderSurface_( void * _nativeHandle, const Resolution & _resolution, float _dpiScale, const DocumentInterfacePtr & _doc )
    {
        DX11RenderSurfacePtr surface = m_factoryRenderSurface->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( surface, "invalid create render surface" );

        if( surface->initialize( m_pD3DDevice.Get(), _nativeHandle, _resolution, _dpiScale ) == false )
        {
            return nullptr;
        }

        return surface;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::updateWVPInvMatrix_()
    {
        mt::mat4f totalWVPMatrix = m_worldMatrix * m_modelViewMatrix * m_projectionMatrix;

        mt::transpose_m4_m4( &m_totalWVPInvMatrix, totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
}
