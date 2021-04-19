#pragma once

#include "Interface/RenderFragmentShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DX11RenderFragmentShader
        : public RenderFragmentShaderInterface
        , public Factorable
    {
    public:
        DX11RenderFragmentShader();
        ~DX11RenderFragmentShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile );
        void finalize();

    public:
        bool compile( ID3D11Device * _pD3DDevice );

    public:
        void enable(ID3D11DeviceContext * _pD3DDeviceContext );

    protected:
        ID3D11PixelShader * m_pD3DPixelShader;

        ConstString m_name;

        MemoryInterfacePtr m_memory;

        bool m_compile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderFragmentShader, RenderFragmentShaderInterface> DX11RenderFragmentShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}