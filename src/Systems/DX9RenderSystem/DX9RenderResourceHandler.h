#pragma once

#include "Kernel/Mixin.h"

#include "Environment/DirectX9/DirectX9RenderIncluder.h"

#include "stdex/intrusive_linked.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderResourceHandler
        : public Mixin
        , public stdex::intrusive_linked<DX9RenderResourceHandler>        
    {
    public:
        DX9RenderResourceHandler();
        ~DX9RenderResourceHandler() override;

    public:
        void setDirect3DDevice9( IDirect3DDevice9 * _pD3DDevice );
        IDirect3DDevice9 * getDirect3DDevice9() const;

    public:
        virtual void onRenderReset() = 0;
        virtual bool onRenderRestore() = 0;

    protected:
        IDirect3DDevice9 * m_pD3DDevice;
    };
}