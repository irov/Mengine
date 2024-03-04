#pragma once

#include "Environment/DirectX11/DX11RenderIncluder.h"

#include "Kernel/Mixin.h"
#include "Kernel/IntrusiveLinked.h"

namespace Mengine
{
    class DX11RenderResourceHandler
        : public Mixin
        , public IntrusiveLinked<DX11RenderResourceHandler>
    {
    public:
        DX11RenderResourceHandler();
        ~DX11RenderResourceHandler() override;

    public:
        void setDirect3D11Device( const ID3D11DevicePtr & _pD3DDevice );
        const ID3D11DevicePtr & getDirect3D11Device() const;

        ID3D11DeviceContextPtr getDirect3D11ImmediateContext() const;

    public:
        virtual void onRenderReset() = 0;
        virtual bool onRenderRestore() = 0;

    private:
        ID3D11DevicePtr m_pD3DDevice;
    };
}