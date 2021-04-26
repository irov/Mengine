#pragma once

#include "Kernel/Mixin.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

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
        void setDirect3D11Device( ID3D11Device * _pD3DDevice );
        ID3D11Device * getDirect3D11Device() const;
        // TODO: getContext

    public:
        virtual void onRenderReset() = 0;
        virtual bool onRenderRestore() = 0;

    protected:
        ID3D11Device * m_pD3DDevice;
        // TODO: context
    };
}