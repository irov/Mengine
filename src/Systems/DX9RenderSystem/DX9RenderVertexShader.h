#pragma once

#include "Interface/RenderVertexShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX9/DirectX9RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DX9RenderVertexShader
        : public RenderVertexShaderInterface
        , public Factorable
    {
    public:
        DX9RenderVertexShader();
        ~DX9RenderVertexShader() override;

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
        IDirect3DVertexShader9 * m_pD3DVertexShader;

        ConstString m_name;

        MemoryInterfacePtr m_memory;

        bool m_compile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderVertexShader> DX9RenderVertexShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}