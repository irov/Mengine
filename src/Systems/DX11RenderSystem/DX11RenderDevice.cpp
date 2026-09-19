#include "DX11RenderDevice.h"

#include "DX11RenderErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderDevice::DX11RenderDevice()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderDevice::~DX11RenderDevice()
    {
        MENGINE_ASSERTION_FATAL( m_device == nullptr, "device is not released" );
        MENGINE_ASSERTION_FATAL( m_deviceContext == nullptr, "device context is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderDevice::initialize( IDXGIAdapter * _adapter )
    {
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(MENGINE_DEBUG)
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0
        };

        ID3D11Device * device;
        ID3D11DeviceContext * deviceContext;
        D3D_FEATURE_LEVEL featureLevel;

        HRESULT result = D3D11CreateDevice(
            _adapter,
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE( featureLevels ),
            D3D11_SDK_VERSION,
            &device,
            &featureLevel,
            &deviceContext
        );

        if( FAILED( result ) )
        {
            LOGGER_ERROR( "invalid create Direct3D device: %s [%x]"
                , Helper::getDX11ErrorMessage( result )
                , static_cast<uint32_t>(result)
            );

            return false;
        }

        m_device.Attach( device );
        m_deviceContext.Attach( deviceContext );

        LOGGER_INFO( "render", "Direct3D feature level: 0x%X"
            , (uint32_t)featureLevel
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderDevice::finalize()
    {
        m_deviceContext = nullptr;
        m_device = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11DevicePtr & DX11RenderDevice::getDirect3D11Device() const
    {
        return m_device;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11DeviceContextPtr & DX11RenderDevice::getDirect3D11DeviceContext() const
    {
        return m_deviceContext;
    }
    //////////////////////////////////////////////////////////////////////////
}
