#include "DX11RenderSurface.h"

#include "DX11RenderErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderSurface::DX11RenderSurface()
        : m_dpiScale( 1.f )
        , m_waitForVSync( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderSurface::~DX11RenderSurface()
    {
        MENGINE_ASSERTION_FATAL( m_swapChain == nullptr, "swap chain is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSurface::initialize( ID3D11Device * _device, void * _nativeHandle, const Resolution & _resolution, float _dpiScale )
    {
        if( _device == nullptr )
        {
            return false;
        }

        if( _nativeHandle == nullptr )
        {
            return false;
        }

        m_resolution = _resolution;
        m_dpiScale = _dpiScale;

        m_device = _device;

        IDXGIFactory2 * factoryRaw = nullptr;
        HRESULT factoryResult = CreateDXGIFactory1( __uuidof(IDXGIFactory2), reinterpret_cast<void **>(&factoryRaw) );

        if( FAILED( factoryResult ) )
        {
            LOGGER_ERROR( "invalid create render surface DXGI factory: %s [%x]"
                , Helper::getDX11ErrorMessage( factoryResult )
                , static_cast<uint32_t>(factoryResult)
            );

            return false;
        }

        IDXGIFactory2Ptr factory;
        factory.Attach( factoryRaw );

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
        swapChainDesc.Width = m_resolution.getWidth();
        swapChainDesc.Height = m_resolution.getHeight();
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = 0;

        HWND hWnd = static_cast<HWND>(_nativeHandle);

        IDXGISwapChain1 * swapChainRaw = nullptr;
        MENGINE_IF_DX11_CALL( factory, CreateSwapChainForHwnd, (m_device.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChainRaw) )
        {
            return false;
        }

        m_swapChain.Attach( swapChainRaw );

        MENGINE_IF_DX11_CALL( factory, MakeWindowAssociation, (hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER) )
        {
            return false;
        }

        if( this->createTargets_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSurface::finalize()
    {
        if( m_swapChain != nullptr )
        {
            this->setFullscreen( false );
        }

        this->releaseTargets_();

        m_swapChain = nullptr;
        m_device = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const Resolution & DX11RenderSurface::getResolution() const
    {
        return m_resolution;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderSurface::getDpiScale() const
    {
        return m_dpiScale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSurface::resize( const Resolution & _resolution, float _dpiScale )
    {
        if( m_swapChain == nullptr )
        {
            return false;
        }

        uint32_t width = _resolution.getWidth();
        uint32_t height = _resolution.getHeight();

        if( width == 0 || height == 0 )
        {
            return false;
        }

        this->releaseTargets_();

        MENGINE_IF_DX11_CALL( m_swapChain, ResizeBuffers, (0, width, height, DXGI_FORMAT_UNKNOWN, 0) )
        {
            return false;
        }

        m_resolution = _resolution;
        m_dpiScale = _dpiScale;

        if( this->createTargets_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSurface::setVSync( bool _vsync )
    {
        m_waitForVSync = _vsync;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSurface::setFullscreen( bool _fullscreen )
    {
        if( m_swapChain == nullptr )
        {
            return false;
        }

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        BOOL fullscreen = _fullscreen == true ? TRUE : FALSE;

        MENGINE_IF_DX11_CALL( m_swapChain, SetFullscreenState, (fullscreen, nullptr) )
        {
            return false;
        }
#else
        MENGINE_UNUSED( _fullscreen );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSurface::resizeTarget( const DXGI_MODE_DESC & _modeDesc )
    {
        if( m_swapChain == nullptr )
        {
            return false;
        }

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        MENGINE_IF_DX11_CALL( m_swapChain, ResizeTarget, (&_modeDesc) )
        {
            return false;
        }
#else
        MENGINE_UNUSED( _modeDesc );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSurface::acquire()
    {
        if( m_swapChain == nullptr )
        {
            return false;
        }

        if( m_colorTarget == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSurface::bind( ID3D11DeviceContext * _context )
    {
        if( m_colorTarget == nullptr )
        {
            return false;
        }

        ID3D11RenderTargetView * colorTargets[1] = {m_colorTarget.Get()};
        _context->OMSetRenderTargets( 1, colorTargets, m_depthTarget.Get() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSurface::unbind( ID3D11DeviceContext * _context )
    {
        ID3D11RenderTargetView * colorTargets[1] = {nullptr};
        _context->OMSetRenderTargets( 1, colorTargets, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    EDX11PresentResult DX11RenderSurface::present()
    {
        if( m_swapChain == nullptr )
        {
            return EDX11PR_DEVICE_LOST;
        }

        UINT syncInterval = 1;
        UINT flags = 0;

        if( m_waitForVSync == false )
        {
            syncInterval = 0;
            flags = DXGI_PRESENT_DO_NOT_WAIT;
        }

        HRESULT result = m_swapChain->Present( syncInterval, flags );

        EDX11PresentResult presentResult = EDX11PR_SUCCESSFUL;

        switch( result )
        {
        case S_OK:
            {
                //Empty
            }break;
        case DXGI_ERROR_WAS_STILL_DRAWING:
            {
                //Empty
            }break;
        case DXGI_ERROR_DEVICE_REMOVED:
            {
                HRESULT reason = m_device->GetDeviceRemovedReason();

                LOGGER_ERROR( "render surface device removed [%x]: %s"
                    , static_cast<uint32_t>(reason)
                    , Helper::getDX11ErrorMessage( reason )
                );

                presentResult = EDX11PR_DEVICE_LOST;
            }break;
        case DXGI_ERROR_DEVICE_RESET:
            {
                LOGGER_ERROR( "render surface device reset" );

                presentResult = EDX11PR_DEVICE_LOST;
            }break;
        default:
            {
                LOGGER_ERROR( "invalid render surface present [%x]: %s"
                    , static_cast<uint32_t>(result)
                    , Helper::getDX11ErrorMessage( result )
                );

                presentResult = EDX11PR_RECOVERABLE;
            }break;
        }

        return presentResult;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11RenderTargetView * DX11RenderSurface::getColorTarget() const
    {
        ID3D11RenderTargetView * colorTarget = m_colorTarget.Get();

        return colorTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11DepthStencilView * DX11RenderSurface::getDepthTarget() const
    {
        ID3D11DepthStencilView * depthTarget = m_depthTarget.Get();

        return depthTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderSurface::createTargets_()
    {
        ID3D11Texture2D * backBufferRaw = nullptr;
        MENGINE_IF_DX11_CALL( m_swapChain, GetBuffer, (0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBufferRaw)) )
        {
            return false;
        }

        ID3D11Texture2DPtr backBuffer;
        backBuffer.Attach( backBufferRaw );

        ID3D11RenderTargetView * colorTargetRaw = nullptr;
        MENGINE_IF_DX11_CALL( m_device, CreateRenderTargetView, (backBuffer.Get(), nullptr, &colorTargetRaw) )
        {
            return false;
        }

        m_colorTarget.Attach( colorTargetRaw );

        D3D11_TEXTURE2D_DESC backBufferDesc;
        backBuffer->GetDesc( &backBufferDesc );

        D3D11_TEXTURE2D_DESC depthDesc;
        ZeroMemory( &depthDesc, sizeof( depthDesc ) );
        depthDesc.Width = backBufferDesc.Width;
        depthDesc.Height = backBufferDesc.Height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthDesc.CPUAccessFlags = 0;
        depthDesc.MiscFlags = 0;

        ID3D11Texture2D * depthTextureRaw = nullptr;
        MENGINE_IF_DX11_CALL( m_device, CreateTexture2D, (&depthDesc, nullptr, &depthTextureRaw) )
        {
            return false;
        }

        m_depthTexture.Attach( depthTextureRaw );

        ID3D11DepthStencilView * depthTargetRaw = nullptr;
        MENGINE_IF_DX11_CALL( m_device, CreateDepthStencilView, (m_depthTexture.Get(), nullptr, &depthTargetRaw) )
        {
            return false;
        }

        m_depthTarget.Attach( depthTargetRaw );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderSurface::releaseTargets_()
    {
        m_depthTarget = nullptr;
        m_depthTexture = nullptr;
        m_colorTarget = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
