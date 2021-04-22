#include "DX11RenderSystem.h"

#include "Interface/RenderServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "DX11RenderImage.h"
#include "DX11RenderImageTarget.h"
#include "DX11RenderTargetTexture.h"
#include "DX11RenderTargetOffscreen.h"

#include "DX11RenderVertexShader.h"
#include "DX11RenderFragmentShader.h"
#include "DX11RenderProgram.h"
#include "DX11RenderVertexAttribute.h"
#include "DX11RenderVertexBuffer.h"
#include "DX11RenderIndexBuffer.h"
#include "DX11RenderProgramVariable.h"

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

#include "Config/StdString.h"

#include "math/uv4.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystem, Mengine::DX11RenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //typedef IDirect3D9 * (WINAPI * PDIRECT3DCREATE9)(UINT);
    //////////////////////////////////////////////////////////////////////////
    DX11RenderSystem::DX11RenderSystem()
        : m_pD3DDevice( nullptr )
        , m_pD3DDeviceContext( nullptr )
        , m_hd3d9( nullptr )
        , m_fullscreen( true )
        , m_depth( false )
        , m_adapterToUse( 0 )
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
		, m_renderTargetView(nullptr)
		, m_depthStencilBuffer(nullptr)
		, m_depthStencilState(nullptr)
		, m_depthStencilView(nullptr)
		, m_rasterState(nullptr)
		, m_blendState(nullptr)

    {
        mt::ident_m4( m_projectionMatrix );
        mt::ident_m4( m_modelViewMatrix );
        mt::ident_m4( m_worldMatrix );
        mt::ident_m4( m_totalWVPInvMatrix );
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

        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef MENGINE_DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_9_1
        };

        // Create the Direct3D 11 API device object and a corresponding context.
        ID3D11Device * device;
        ID3D11DeviceContext * context;
        D3D11CreateDevice(
            nullptr, // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE( featureLevels ),
            D3D11_SDK_VERSION, // UWP apps must set this to D3D11_SDK_VERSION.
            &device, // Returns the Direct3D device created.
            nullptr,
            &context // Returns the device immediate context.
        );

		m_pD3DDevice = device;
		m_pD3DDeviceContext = context;

		HRESULT result;
		IDXGIFactory* factory = nullptr;
		IDXGIAdapter* adapter = nullptr;

		// Create a DirectX graphics interface factory.
		// DirectX 11 (for 11.1 or 11.2 need to use other factory)
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result))
		{
			return false;
		}

		// Use the factory to create an adapter for the primary graphics interface (video card).
		result = factory->EnumAdapters(m_adapterToUse, &adapter);
		if (FAILED(result))
		{
			return false;
		}

		IDXGIOutput* adapterOutput;
		// Enumerate the primary adapter output (monitor).
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result))
		{
			return false;
		}

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &m_DisplayModeListNum, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		m_DisplayModeList = new DXGI_MODE_DESC[m_DisplayModeListNum];
		if (!m_DisplayModeList)
		{
			return false;
		}

		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &m_DisplayModeListNum, m_DisplayModeList);
		if (FAILED(result))
		{
			return false;
		}

		DXRELEASE(adapterOutput);

		DXGI_ADAPTER_DESC AdapterDesc;
		adapter->GetDesc(&AdapterDesc);

        //LOGGER_MESSAGE_RELEASE( "D3D Adapter Driver: %s", AdID.Driver );
        LOGGER_MESSAGE_RELEASE( "D3D Adapter Description: %s", AdapterDesc.Description );
        //LOGGER_MESSAGE_RELEASE( "D3D Adapter DeviceName: %s", AdID.DeviceName );

        /*LOGGER_MESSAGE_RELEASE( "D3D Adapter Version: %hu.%hu.%hu.%hu"
            , HIWORD( AdID.DriverVersion.HighPart )
            , LOWORD( AdID.DriverVersion.HighPart )
            , HIWORD( AdID.DriverVersion.LowPart )
            , LOWORD( AdID.DriverVersion.LowPart )
        );*/

        LOGGER_MESSAGE_RELEASE( "D3D Adapter VendorId: %lu", AdapterDesc.VendorId );
        LOGGER_MESSAGE_RELEASE( "D3D Adapter DeviceId: %lu", AdapterDesc.DeviceId );
        LOGGER_MESSAGE_RELEASE( "D3D Adapter SubSysId: %lu", AdapterDesc.SubSysId );
        LOGGER_MESSAGE_RELEASE( "D3D Adapter Revision: %lu", AdapterDesc.Revision );

		DXRELEASE(adapter);
		DXRELEASE(factory);

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::_finalizeService()
    {
        m_deferredCompileVertexShaders.clear();
        m_deferredCompileFragmentShaders.clear();
        m_deferredCompileVertexAttributes.clear();
        m_deferredCompilePrograms.clear();

        this->release_();

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_renderResourceHandlers );

        m_renderResourceHandlers.clear();

        MENGINE_ASSERTION_FATAL( m_textureCount == 0 );
        MENGINE_ASSERTION_FATAL( m_vertexBufferCount == 0 );
        MENGINE_ASSERTION_FATAL( m_indexBufferCount == 0 );

        if( m_hd3d9 != nullptr )
        {
            ::FreeLibrary( m_hd3d9 );
            m_hd3d9 = nullptr;
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

		delete[] m_DisplayModeList;
		m_DisplayModeList = nullptr;

		DXRELEASE(m_renderTargetView);
		DXRELEASE(m_depthStencilBuffer);
		DXRELEASE(m_depthStencilState);
		DXRELEASE(m_depthStencilView);
		DXRELEASE(m_rasterState);

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
    }
	//////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::createRenderWindow( const Resolution & _resolution
        , uint32_t _bits
        , bool _fullscreen
        , bool _depth
        , bool _waitForVSync
        , int32_t _FSAAType
        , int32_t _FSAAQuality
        , uint32_t _MultiSampleCount )
    {
        MENGINE_UNUSED( _bits );
        MENGINE_UNUSED( _FSAAType );
        MENGINE_UNUSED( _FSAAQuality );

        m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

        m_fullscreen = _fullscreen;
        m_depth = _depth;
        m_waitForVSync = _waitForVSync;

		IDXGIFactory* factory = nullptr;

		// Create a DirectX graphics interface factory.
		// DirectX 11 (for 11.1 or 11.2 need to use other factory)
		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result))
		{
			return false;
		}

        // Create Swap Chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		// Initialize the swap chain description.
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		// Set to a single back buffer.
		swapChainDesc.BufferCount = 2;

		// Set the width and height of the back buffer.
		swapChainDesc.BufferDesc.Width = m_windowResolution.getWidth();
		swapChainDesc.BufferDesc.Height = m_windowResolution.getHeight();

		// Set regular 32-bit surface for the back buffer.
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Set the refresh rate of the back buffer.
		if (m_waitForVSync)
		{
			for (UINT i = 0; i < m_DisplayModeListNum; i++)
			{
				if (m_DisplayModeList[i].Width == (unsigned int)swapChainDesc.BufferDesc.Width &&
					m_DisplayModeList[i].Height == (unsigned int)swapChainDesc.BufferDesc.Height)
				{
					swapChainDesc.BufferDesc.RefreshRate.Numerator = m_DisplayModeList[i].RefreshRate.Numerator;
					swapChainDesc.BufferDesc.RefreshRate.Denominator = m_DisplayModeList[i].RefreshRate.Denominator;
					break;
				}
			}
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		// Set the usage of the back buffer.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		// Set the handle for the window to render to.
		Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()->getPlatformExtention();

		swapChainDesc.OutputWindow = win32Platform->getWindowHandle();

		// Turn multisampling off.
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		// Set to full screen or windowed mode.
		swapChainDesc.Windowed = _fullscreen == false ? true : false;

		// Set the scan line ordering and scaling to unspecified.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Discard the back buffer contents after presenting.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

			// Don't set the advanced flags.
		swapChainDesc.Flags = 0;

		result = factory->CreateSwapChain(m_pD3DDevice, &swapChainDesc, &m_SwapChain);
		if (FAILED(result))
		{
			return false;
		}

		m_SwapChainBufferDesc = swapChainDesc.BufferDesc;

		DXRELEASE(factory);

		// Get the pointer to the back buffer.
		ID3D11Texture2D* backBufferPtr;
		result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
		if (FAILED(result))
		{
			return false;
		}

		// Create the render target view with the back buffer pointer.
		IF_DXCALL(m_pD3DDevice, CreateRenderTargetView, (backBufferPtr, NULL, &m_renderTargetView))
		{
			return false;
		}

		// Release pointer to the back buffer as we no longer need it.
		backBufferPtr->Release();
		backBufferPtr = 0;

		// Initialize the description of the depth buffer.
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

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
		IF_DXCALL(m_pD3DDevice, CreateTexture2D, (&depthBufferDesc, NULL, &m_depthStencilBuffer))
		{
			return false;
		}

		// Initialize the description of the stencil state.
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state.
		IF_DXCALL( m_pD3DDevice, CreateDepthStencilState, (&depthStencilDesc, &m_depthStencilState))
		{
			return false;
		}

		// Set the depth stencil state.
		m_pD3DDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

		// Initialize the depth stencil view.
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		// Set up the depth stencil view description.
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// Create the depth stencil view.
		IF_DXCALL( m_pD3DDevice, CreateDepthStencilView, (m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView))
		{
			return false;
		}

		// Bind the render target view and depth stencil buffer to the output render pipeline.
		m_pD3DDeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

		// Setup the raster description which will determine how and what polygons will be drawn.
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;

		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = true;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out.
		IF_DXCALL(m_pD3DDevice, CreateRasterizerState, (&rasterDesc, &m_rasterState))
		if (FAILED(result))
		{
			return false;
		}

		m_pD3DDeviceContext->RSSetState(m_rasterState);

		D3D11_VIEWPORT viewport;
		// Setup the viewport for rendering.
		viewport.Width = m_windowViewport.getWidth();
		viewport.Height = m_windowViewport.getHeight();
		viewport.TopLeftX = MT_floorf(m_windowViewport.begin.x + 0.5f);
		viewport.TopLeftY = MT_floorf(m_windowViewport.begin.y + 0.5f);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		// Create the viewport.
		m_pD3DDeviceContext->RSSetViewports(1, &viewport);


/*        LOGGER_MESSAGE_RELEASE( "Vertex Shader Version [%lu] [%s]"
            , m_d3dCaps.VertexShaderVersion
            , m_d3dCaps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) ? "true" : "false"
        );

        LOGGER_MESSAGE_RELEASE( "Pixel Shader Version [%lu] [%s] [%s]"
            , m_d3dCaps.PixelShaderVersion
            , m_d3dCaps.PixelShaderVersion < D3DPS_VERSION( 1, 1 ) ? "true" : "false"
            , m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION( 2, 0 ) ? "true" : "false"
        );
*/

        for( const DX11RenderVertexShaderPtr & shader : m_deferredCompileVertexShaders )
        {
            if( shader->compile( m_pD3DDevice ) == false )
            {
                return false;
            }
        }

        m_deferredCompileVertexShaders.clear();

        for( const DX11RenderFragmentShaderPtr & shader : m_deferredCompileFragmentShaders )
        {
            if( shader->compile( m_pD3DDevice ) == false )
            {
                return false;
            }
        }

        m_deferredCompileFragmentShaders.clear();

        for( const DX11RenderVertexAttributePtr & attribute : m_deferredCompileVertexAttributes )
        {
            if( attribute->compile( m_pD3DDevice ) == false )
            {
                return false;
            }
        }

        m_deferredCompileVertexAttributes.clear();

        for( const DX11RenderProgramPtr & program : m_deferredCompilePrograms )
        {
            if( program->compile( m_pD3DDevice ) == false )
            {
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

        float DirectX11_PerfectPixelOffsetX = CONFIG_VALUE( "DirectX11", "PerfectPixelOffsetX", 0.0f );
        float DirectX11_PerfectPixelOffsetY = CONFIG_VALUE( "DirectX11", "PerfectPixelOffsetY", 0.0f );

        float perfect_x = DirectX11_PerfectPixelOffsetX / (m_windowViewport.end.x - m_windowViewport.begin.x);
        float perfect_y = DirectX11_PerfectPixelOffsetY / (m_windowViewport.end.y - m_windowViewport.begin.y);

        mt::mat4f vmperfect;
        mt::make_translation_m4( vmperfect, perfect_x, perfect_y, 0.f );

        mt::mul_m4_m4( m_projectionMatrix, _projectionMatrix, vmperfect );

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
    RenderImageInterfacePtr DX11RenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderImagePtr renderImage = m_factoryRenderImage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderImage, "invalid create render texture" );

        renderImage->setDirect3D11Device( m_pD3DDevice );

        if( renderImage->initialize( _mipmaps, _width, _height, _channels, _depth, _format ) == false )
        {
            LOGGER_ERROR( "can't initialize image %ux%u channels %u depth %u format %d"
                , _width
                , _height
                , _channels
                , _depth
                , _format
            );

            return nullptr;
        }

        LOGGER_INFO( "render", "texture normal created %ux%u format %u channel %u depth %u"
            , renderImage->getHWWidth()
            , renderImage->getHWHeight()
            , renderImage->getHWPixelFormat()
            , renderImage->getHWChannels()
            , renderImage->getHWDepth()
        );

        m_renderResourceHandlers.push_back( renderImage.get() );

#ifdef MENGINE_DEBUG
        bool logcreateimage = HAS_OPTION( "logcreateimage" );

        if( logcreateimage == true )
        {
            LOGGER_STATISTIC( "create texture [%u:%u] channels %u format %u (doc %s)"
                , renderImage->getHWWidth()
                , renderImage->getHWHeight()
                , renderImage->getHWChannels()
                , renderImage->getHWPixelFormat()
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }
#endif

#ifdef MENGINE_DEBUG
        ++m_textureCount;

        uint32_t hwWidth = renderImage->getHWHeight();
        uint32_t hwHeight = renderImage->getHWHeight();
        uint32_t hwChannels = renderImage->getHWChannels();
        uint32_t hwDepth = renderImage->getHWDepth();
        EPixelFormat hwPixelFormat = renderImage->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwChannels, hwDepth, hwPixelFormat );

        m_textureMemoryUse += memoryUse;
#endif

        return renderImage;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX11RenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetTexturePtr renderTargetTexture = m_factoryRenderTargetTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetTexture );

        renderTargetTexture->setDirect3D11Device( m_pD3DDevice );

        if( renderTargetTexture->initialize( _width, _height, _channels, _format ) == false )
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
        uint32_t hwChannels = renderTargetTexture->getHWChannels();
        uint32_t hwDepth = renderTargetTexture->getHWDepth();
        EPixelFormat hwPixelFormat = renderTargetTexture->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwChannels, hwDepth, hwPixelFormat );

        m_textureMemoryUse += memoryUse;
#endif

        return renderTargetTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr DX11RenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetOffscreenPtr renderTargetOffscreen = m_factoryRenderTargetOffscreen->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetOffscreen );

		ID3D11Texture2D* backBufferPtr;
		auto result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
		if (FAILED(result))
		{
			return nullptr;
		}

        renderTargetOffscreen->setDirect3D11Device( m_pD3DDevice );
        if( renderTargetOffscreen->initialize(backBufferPtr) == false )
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
        uint32_t hwChannels = renderTargetOffscreen->getHWChannels();
        uint32_t hwDepth = renderTargetOffscreen->getHWDepth();
        EPixelFormat hwPixelFormat = renderTargetOffscreen->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwChannels, hwDepth, hwPixelFormat );

        m_textureMemoryUse += memoryUse;
#endif

        return renderTargetOffscreen;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr DX11RenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DX11RenderTargetTexturePtr renderTargetTexture = stdex::intrusive_static_cast<DX11RenderTargetTexturePtr>(_renderTarget);

        DX11RenderImageTargetPtr renderImageTarget = m_factoryRenderImageTarget->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( renderImageTarget );

        renderImageTarget->initialize( renderTargetTexture );

        LOGGER_INFO( "render", "render image target created" );

        return renderImageTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::resetDevice_()
    {

		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::beginScene()
    {

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::endScene()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::swapBuffers()
    {
		MENGINE_ASSERTION_MEMORY_PANIC(m_pD3DDeviceContext, "device context not found");

		// Present the back buffer to the screen since rendering is complete.
		if (m_waitForVSync)
		{
			// Lock to screen refresh rate.
			m_SwapChain->Present(1, 0);
		}
		else
		{
			// Present as fast as possible.
			m_SwapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
		}

        ++m_frames;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, float _depth, uint32_t _stencil )
    {
		MENGINE_ASSERTION_MEMORY_PANIC(m_pD3DDeviceContext, "device context not found");

		float color[4];

		// Setup the color to clear the buffer to.
		color[0] = _color.getR();
		color[1] = _color.getG();
		color[2] = _color.getB();
		color[3] = _color.getA();

		// Clear the back buffer.
		m_pD3DDeviceContext->ClearRenderTargetView(m_renderTargetView, color);

		// Clear the depth buffer.
		m_pD3DDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, _depth, _stencil);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setScissor( const Viewport & _viewport )
    {
		MENGINE_UNUSED(_viewport);
	}
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::removeScissor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setViewport( const Viewport & _viewport )
    {
        if( m_viewport.equalViewport( _viewport ) == true )
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
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = MT_floorf(_viewport.begin.x + 0.5f);
		viewport.TopLeftY = MT_floorf(_viewport.begin.y + 0.5f);

		// Create the viewport.
		m_pD3DDeviceContext->RSSetViewports(1, &viewport);

    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
		MENGINE_ASSERTION_MEMORY_PANIC(m_SwapChain, "swap chain not created");

        if( m_windowResolution == _resolution && m_fullscreen == _fullscreen )
        {
            return;
        }

		if (m_fullscreen != _fullscreen)
		{
			IF_DXCALL(m_SwapChain, SetFullscreenState, (_fullscreen, nullptr))
			{
				return;
			}

		}

        m_fullscreen = _fullscreen;

		if (!(m_windowResolution == _resolution))
		{
			m_SwapChainBufferDesc.Width = _resolution.getWidth();
			m_SwapChainBufferDesc.Height = _resolution.getHeight();

			IF_DXCALL(m_SwapChain, ResizeTarget, (&m_SwapChainBufferDesc))
			{
				return;
			}

			IF_DXCALL(m_SwapChain, ResizeBuffers, (2, _resolution.getWidth(), _resolution.getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0))
			{
				return;
			}
		}

		m_windowResolution = _resolution;

        mt::vec2f windowSize;
        m_windowResolution.calcSize( &windowSize );
        m_windowViewport = Viewport( mt::vec2f::identity(), windowSize );

		updateViewport_(m_windowViewport);

        if( this->restore_() == false )
        {
            LOGGER_ERROR( "Graphics change mode failed" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderSystem::getAvailableTextureMemory() const
    {
        return 0;
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
    UnknownPointer DX11RenderSystem::getRenderSystemExtention()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Device * DX11RenderSystem::getDirect3D11Device() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11DeviceContext * DX11RenderSystem::getDirect3D11DeviceContext() const
    {
        return m_pD3DDeviceContext;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::supportTextureFormat( EPixelFormat _format ) const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::supportTextureNonPow2() const
    {
        return true;
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

		MENGINE_UNUSED(_stage);
		MENGINE_UNUSED(_matrix);
	}
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::releaseResources_()
    {
        MENGINE_ASSERTION_MEMORY_PANIC(m_pD3DDeviceContext, "device not created" );

        if( m_vertexBufferEnable == true )
        {
            m_vertexBufferEnable = false;

			m_pD3DDeviceContext->IASetVertexBuffers(0, 1, nullptr, nullptr, nullptr);
        }

        if( m_indexBufferEnable == true )
        {
            m_indexBufferEnable = false;

			m_pD3DDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
        }

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            if( m_textureEnable[i] == false )
            {
                continue;
            }

            m_textureEnable[i] = false;
        }
		m_pD3DDeviceContext->PSSetSamplers(0, MENGINE_MAX_TEXTURE_STAGES, nullptr);

        if( m_vertexShaderEnable == true )
        {
            m_vertexShaderEnable = false;

			m_pD3DDeviceContext->VSSetShader(nullptr, nullptr, 0);
        }

        if( m_fragmentShaderEnable == true )
        {
            m_fragmentShaderEnable = false;

			m_pD3DDeviceContext->PSSetShader(nullptr, nullptr, 0);
        }

        if( m_vertexAttributeEnable == true )
        {
            m_vertexAttributeEnable = false;

			m_pD3DDeviceContext->IASetInputLayout(nullptr);
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSystem::restore_()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::release_()
    {
		if (m_pD3DDevice == nullptr)
		{
			return;
		}

		if (this->releaseResources_() == false)
		{
			LOGGER_ERROR("invalid release resource");
			return;
		}

		ULONG ref;

		ref = m_pD3DDeviceContext->Release();
		MENGINE_UNUSED(ref);
		m_pD3DDeviceContext = nullptr;
		
		ref = m_pD3DDevice->Release();
		MENGINE_UNUSED(ref);
		m_pD3DDevice = nullptr;

    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr DX11RenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentPtr & _doc )
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

			m_pD3DDeviceContext->IASetVertexBuffers(0, 1, nullptr, 0, 0);

            return true;
        }

        _vertexBuffer->enable();

        m_vertexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr DX11RenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentPtr & _doc )
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
			m_pD3DDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

            return true;
        }

        _indexBuffer->enable();

        m_indexBufferEnable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::drawIndexedPrimitive( EPrimitiveType _type, uint32_t _vertexBase,
        uint32_t _minIndex, uint32_t _vertexCount, uint32_t _indexStart, uint32_t _indexCount )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

		D3D11_PRIMITIVE_TOPOLOGY primitiveType = Helper::toD3DPrimitiveType( _type );

        UINT primCount = Helper::getPrimitiveCount( _type, _indexCount );

		if(primitiveType == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			m_pD3DDeviceContext->DrawIndexed(primCount * 3, _minIndex, _vertexBase);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        MENGINE_UNUSED( _program );

        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "no support stage %d (max %d)"
                , _stage
                , MaxCombinedTextureImageUnits
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
			m_pD3DDeviceContext->PSSetShaderResources(_stage, 1, nullptr);

            m_textureEnable[_stage] = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

		// blend state
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		blendDesc.IndependentBlendEnable = _separate;
		blendDesc.RenderTarget[0].SrcBlend = Helper::toD3DBlendFactor(_src);
		blendDesc.RenderTarget[0].DestBlend = Helper::toD3DBlendFactor(_dst);
		blendDesc.RenderTarget[0].BlendOp = Helper::toD3DBlendOp(_op);

		if (_separate)
		{
			blendDesc.RenderTarget[0].SrcBlendAlpha = Helper::toD3DBlendFactor(_separateSrc);
			blendDesc.RenderTarget[0].DestBlendAlpha = Helper::toD3DBlendFactor(_separateDst);
			blendDesc.RenderTarget[0].BlendOpAlpha = Helper::toD3DBlendOp(_separateOp);
		}

		if (m_blendState != nullptr)
		{
			m_blendState->Release();
			m_blendState = nullptr;
		}

		IF_DXCALL(m_pD3DDevice, CreateBlendState, (&blendDesc, &m_blendState))
		{
			return;
		}

		float BlendFactors[4] = { 1, 1, 1, 1 };
		m_pD3DDeviceContext->OMSetBlendState(m_blendState, BlendFactors, 0xffffffff);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "no support stage %d (max %d)"
                , _stage
                , MaxCombinedTextureImageUnits
            );

            return;
        }

        D3DTEXTUREADDRESS adrU = Helper::toD3DTextureAddress( _modeU );
        D3DTEXTUREADDRESS adrV = Helper::toD3DTextureAddress( _modeV );

        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_ADDRESSU, adrU) );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_ADDRESSV, adrV) );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_BORDERCOLOR, _border) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTextureFactor( uint32_t _color )
    {
		MENGINE_UNUSED(_color);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setCullMode( ECullMode _mode )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DCULL mode = Helper::toD3DCullMode( _mode );

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_CULLMODE, mode) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setDepthBufferTestEnable( bool _depthTest )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DZBUFFERTYPE test = _depthTest == true ? D3DZB_TRUE : D3DZB_FALSE;

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZENABLE, test) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DWORD dWrite = _depthWrite == true ? TRUE : FALSE;

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZWRITEENABLE, dWrite) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DCMPFUNC func = Helper::toD3DCmpFunc( _depthFunction );

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ZFUNC, func) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setFillMode( EFillMode _mode )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        D3DFILLMODE mode = Helper::toD3DFillMode( _mode );

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_FILLMODE, mode) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
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

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_COLORWRITEENABLE, value) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        DWORD alphaBlend = _alphaBlend ? TRUE : FALSE;

        DXCALL( m_pD3DDevice, SetRenderState, (D3DRS_ALPHABLENDENABLE, alphaBlend) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_pD3DDevice, "device not created" );

        uint32_t MaxCombinedTextureImageUnits = this->getMaxCombinedTextureImageUnits();

        if( _stage >= MaxCombinedTextureImageUnits )
        {
            LOGGER_ERROR( "no support stage %d (max %d)"
                , _stage
                , MaxCombinedTextureImageUnits
            );

            return;
        }

        D3DTEXTUREFILTERTYPE dx_minification = Helper::toD3DTextureFilter( _minification );
        D3DTEXTUREFILTERTYPE dx_mipmap = Helper::toD3DTextureFilter( _mipmap );
        D3DTEXTUREFILTERTYPE dx_magnification = Helper::toD3DTextureFilter( _magnification );

        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MINFILTER, dx_minification) );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MIPFILTER, dx_mipmap) );
        DXCALL( m_pD3DDevice, SetSamplerState, (_stage, D3DSAMP_MAGFILTER, dx_magnification) );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr DX11RenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentPtr & _doc )
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

        if( m_pD3DDevice != nullptr )
        {
            if( attribute->compile( m_pD3DDevice ) == false )
            {
                LOGGER_ERROR( "invalid compile attribute '%s'"
                    , _name.c_str()
                );

                return nullptr;
            }
        }
        else
        {
            m_deferredCompileVertexAttributes.emplace_back( attribute );
        }

        return attribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr DX11RenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc )
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

        if( m_pD3DDevice != nullptr )
        {
            if( shader->compile( m_pD3DDevice ) == false )
            {
                LOGGER_ERROR( "invalid compile shader '%s'"
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
    RenderVertexShaderInterfacePtr DX11RenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc )
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

        if( m_pD3DDevice != nullptr )
        {
            if( shader->compile( m_pD3DDevice ) == false )
            {
                LOGGER_ERROR( "invalid compile shader '%s'"
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
    RenderProgramInterfacePtr DX11RenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _samplerCount );

        DX11RenderProgramPtr program = m_factoryRenderProgram->createObject( _doc );

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
    void DX11RenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        if( _program != nullptr )
        {
            DX11RenderProgramPtr dx9_program = stdex::intrusive_static_cast<DX11RenderProgramPtr>(_program);

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

                DXCALL( m_pD3DDevice, SetVertexDeclaration, (NULL) );
            }

            if( m_vertexShaderEnable == true && vertexShaderEnable == false )
            {
                m_vertexShaderEnable = false;

                DXCALL( m_pD3DDevice, SetVertexShader, (NULL) );
            }

            if( m_fragmentShaderEnable == true && fragmentShaderEnable == false )
            {
                m_fragmentShaderEnable = false;

                DXCALL( m_pD3DDevice, SetPixelShader, (NULL) );
            }
        }
        else
        {
            if( m_vertexShaderEnable == true )
            {
                m_vertexShaderEnable = false;

                DXCALL( m_pD3DDevice, SetVertexShader, (NULL) );
            }

            if( m_fragmentShaderEnable == true )
            {
                m_vertexShaderEnable = false;

                DXCALL( m_pD3DDevice, SetPixelShader, (NULL) );
            }

            if( m_vertexAttributeEnable == true )
            {
                m_vertexShaderEnable = false;

                DXCALL( m_pD3DDevice, SetVertexDeclaration, (NULL) );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        DX11RenderProgramPtr dx9_program = stdex::intrusive_static_cast<DX11RenderProgramPtr>(_program);

        dx9_program->bindMatrix( m_pD3DDevice, m_worldMatrix, m_modelViewMatrix, m_projectionMatrix, m_totalWVPInvMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr DX11RenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentPtr & _doc )
    {
        DX11RenderProgramVariablePtr variable = m_factoryRenderProgramVariable->createObject( _doc );

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
    bool DX11RenderSystem::setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable )
    {
        if( _variable == nullptr )
        {
            return true;
        }

        DX11RenderProgramVariablePtr dx9_variable = stdex::intrusive_static_cast<DX11RenderProgramVariablePtr>(_variable);

        bool successful = dx9_variable->apply( m_pD3DDevice, _program );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::setVSync( bool _vSync )
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
            LOGGER_ERROR( "Graphics change mode failed"
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::updateVSyncDPP_()
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
        uint32_t hwChannels = _image->getHWChannels();
        EPixelFormat hwPixelFormat = _image->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwChannels, 1, hwPixelFormat );

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
        uint32_t hwChannels = _targetTexture->getHWChannels();
        EPixelFormat hwPixelFormat = _targetTexture->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwChannels, 1, hwPixelFormat );

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
        uint32_t hwChannels = _targetOffscreen->getHWChannels();
        EPixelFormat hwPixelFormat = _targetOffscreen->getHWPixelFormat();

        uint32_t memoryUse = Helper::getTextureMemorySize( hwWidth, hwHeight, hwChannels, 1, hwPixelFormat );

        m_textureMemoryUse -= memoryUse;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSystem::updateWVPInvMatrix_()
    {
        mt::mat4f totalWVPMatrix = m_worldMatrix * m_modelViewMatrix * m_projectionMatrix;

        mt::transpose_m4_m4( m_totalWVPInvMatrix, totalWVPMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
}