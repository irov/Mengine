#pragma once

#include "Interface/RenderFragmentShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX9/DirectX9RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DX9RenderFragmentShader
        : public RenderFragmentShaderInterface
        , public Factorable
    {
    public:
        DX9RenderFragmentShader();
        ~DX9RenderFragmentShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile );
        void finalize();

    public:
        bool compile( IDirect3DDevice9 * _pD3DDevice );

    public:
        void enable( IDirect3DDevice9 * _pD3DDevice );

    protected:
        IDirect3DPixelShader9 * m_pD3DPixelShader;

        ConstString m_name;

        MemoryInterfacePtr m_memory;

        bool m_compile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderFragmentShader, RenderFragmentShaderInterface> DX9RenderFragmentShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}