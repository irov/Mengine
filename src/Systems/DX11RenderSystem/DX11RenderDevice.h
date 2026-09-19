#pragma once

#include "Interface/RenderDeviceInterface.h"

#include "Environment/DirectX11/DX11RenderDeviceExtensionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderDevice
        : public RenderDeviceInterface
        , public DX11RenderDeviceExtensionInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderDevice );
        DECLARE_UNKNOWABLE();

    public:
        DX11RenderDevice();
        ~DX11RenderDevice() override;

    public:
        bool initialize( IDXGIAdapter * _adapter );
        void finalize();

    public:
        const ID3D11DevicePtr & getDirect3D11Device() const override;
        const ID3D11DeviceContextPtr & getDirect3D11DeviceContext() const override;

    protected:
        ID3D11DevicePtr m_device;
        ID3D11DeviceContextPtr m_deviceContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderDevice, RenderDeviceInterface> DX11RenderDevicePtr;
    //////////////////////////////////////////////////////////////////////////
}
